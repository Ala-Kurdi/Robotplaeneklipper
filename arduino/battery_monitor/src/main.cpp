#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>

const int BATTERY_PIN = A0;

const float ADC_REF_VOLTAGE = 5.0;
const float DIVIDER_RATIO = 2.0;

const float BATTERY_FULL = 6.4;
const float BATTERY_EMPTY = 4.4;

const char* WIFI_SSID = "prog";
const char* WIFI_PASSWORD = "CHANGE_ME";

const char* MQTT_HOST = "192.168.106.10";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "robot/battery";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void connectWiFi() {
    Serial.print("Forbinder til Wi-Fi");

    while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
        Serial.print(".");
        delay(2000);
    }

    Serial.println();
    Serial.println("Wi-Fi forbundet");
    Serial.print("Uno IP: ");
    Serial.println(WiFi.localIP());
}

void connectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Forbinder til MQTT... ");

        if (mqttClient.connect("uno_r4_battery")) {
            Serial.println("OK");
        } else {
            Serial.print("fejl, state=");
            Serial.println(mqttClient.state());
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(9600);
    delay(2000);

    Serial.println("Battery monitor + MQTT");

    connectWiFi();

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    connectMQTT();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }

    if (!mqttClient.connected()) {
        connectMQTT();
    }

    mqttClient.loop();

    int rawValue = analogRead(BATTERY_PIN);

    float measuredVoltage =
        rawValue * (ADC_REF_VOLTAGE / 1023.0);

    float batteryVoltage =
        measuredVoltage * DIVIDER_RATIO;

    float batteryPercent =
        ((batteryVoltage - BATTERY_EMPTY) /
        (BATTERY_FULL - BATTERY_EMPTY)) * 100.0;

    batteryPercent = constrain(batteryPercent, 0.0, 100.0);

    Serial.print("Battery: ");
    Serial.print(batteryVoltage, 2);
    Serial.print(" V | ");
    Serial.print(batteryPercent, 0);
    Serial.print(" %");

    if (batteryPercent <= 20.0) {
        Serial.print(" | WARNING: LOW BATTERY");
    }

    Serial.println();

    char payload[16];
    snprintf(payload, sizeof(payload), "%.0f", batteryPercent);

    mqttClient.publish(MQTT_TOPIC, payload);

    delay(2000);
}