#include <Arduino.h>

const int BATTERY_PIN = A0;

const float ADC_REF_VOLTAGE = 5.0;
const float DIVIDER_RATIO = 2.0;

// Foreløbig kalibrering til 4 x AA
const float BATTERY_FULL = 6.4;
const float BATTERY_EMPTY = 4.4;

void setup()
{
    Serial.begin(9600);
    delay(2000);

    Serial.println("Battery monitor - FK9");
}

void loop()
{
    int rawValue = analogRead(BATTERY_PIN);

    float measuredVoltage =
        rawValue * (ADC_REF_VOLTAGE / 1023.0);

    float batteryVoltage =
        measuredVoltage * DIVIDER_RATIO;

    float batteryPercent =
        ((batteryVoltage - BATTERY_EMPTY) /
        (BATTERY_FULL - BATTERY_EMPTY)) * 100.0;
       
    // Hold værdien mellem 0 og 100 %
    if (batteryPercent > 100.0)
        batteryPercent = 100.0;

    if (batteryPercent < 0.0)
        batteryPercent = 0.0;

    Serial.print("Battery: ");
    Serial.print(batteryVoltage, 2);
    Serial.print(" V | ");

    Serial.print(batteryPercent, 0);
    Serial.print(" %");

    if (batteryPercent <= 20.0)
    {
        Serial.print(" | WARNING: LOW BATTERY");
    }

    Serial.println();

    delay(1000);
}