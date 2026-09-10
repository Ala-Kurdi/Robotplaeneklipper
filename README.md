# Robotplæneklipper

IoT og Embedded caseprojekt på AarhusTech.

## Gruppe

- Adem
- Mark

## Projektperiode

2 uger

## Formål

Formålet med projektet er at udvikle en prototype af en selvkørende
robotplæneklipper.

Robotten skal kunne bevæge sig autonomt i et afgrænset testområde,
registrere sin position og retning samt registrere og reagere på
forhindringer.

Robotten skal kommunikere trådløst med en central server, som modtager
måledata og events og gemmer dem i en database. Serveren skal også kunne
sende kommandoer tilbage til robotten.

## Projektstatus

- Kravspecifikation godkendt
- Projektstruktur oprettet
- Hardware afventer
- Implementering ikke startet endnu

## Dokumentation

Projektets dokumentation bliver løbende samlet i `docs/`.

- `docs/wiring/` - wiring og hardware
- `docs/tests/` - test og testresultater
- `docs/fejllog.md` - fejl, fejlfinding og løsninger

## Udvikling

Projektet udvikles trin for trin. Hver større funktion bliver testet,
før vi går videre til næste del.

Første mål er at få hele Must Have-kæden til at fungere, før vi arbejder
med ekstra funktioner.

## Installation

### Prerequisites (Afhængigheder)

Før kompilering skal følgende pakker være installeret på Raspberry Pi'en:

```bash
sudo apt update
sudo apt install g++ make libmosquitto-dev mosquitto-clients nlohmann-json3-dev
```

---

### Kompilering med `make`

Kildekoden kompileres nemt ved hjælp af den medfølgende `Makefile`:

* **Kompiler projektet:**
  ```bash
  make
  ```
  Dette opretter den eksekverbare fil `robot_app`.

* **Rengør kompilerede filer:**
  ```bash
  make clean
  ```

---

### Opsætning som `systemd` Service (Autostart)

For at sikre at programmet kører stabilt i baggrunden og starter automatisk ved opstart, oprettes en systemd service.

1. **Opret service-filen:**
   ```bash
   sudo nano /etc/systemd/system/robot.service
   ```

2. **Indsæt følgende konfiguration:**
   ```ini
   [Unit]
   Description=Robotplæneklipper C++ Control Service
   After=network.target mosquitto.service

   [Service]
   Type=simple
   User=root
   WorkingDirectory=/home/adem/Robotplaeneklipper
   ExecStart=/home/adem/Robotplaeneklipper/robot_app
   Restart=always
   RestartSec=5

   [Install]
   WantedBy=multi-user.target
   ```

3. **Aktiver og start servicen:**
   ```bash
   sudo systemctl daemon-reload
   sudo systemctl enable robot.service
   sudo systemctl start robot.service
   ```

4. **Se live logs fra servicen:**
   ```bash
   sudo journalctl -u robot.service -f
   ```

---

### Aktivering og Styring (START / STOP)

Når `robot_app` kører, står den i en dvaletilstand (`IDLE`), indtil den modtager en `START`-kommando over MQTT-emnet `robot/commands`.

#### Metode A: Via Grafana Dashboard
Tryk på **START** eller **STOP** knapperne i Grafana-panelet. Grafana sender et HTTP POST-kald til Python-bridgen på serveren (`X.X.X.X:PORT`), som videreformidler beskeden til MQTT-brokeren.

#### Metode B: Manuelt fra terminalen
Du kan til enhver tid sende styringskommandoer direkte fra en terminal på netværket eller direkte på Pi'en:

* **Start robotten:**
  ```bash
  mosquitto_pub -h X.X.X.X -t "robot/commands" -m "START"
  ```

* **Stop robotten:**
  ```bash
  mosquitto_pub -h X.X.X.X -t "robot/commands" -m "STOP"
  ```

---

### Projektstruktur

```text
├── arduino/             # arduino kode til batteriprocent
├── Server/              # Server bridge kode til at sende START og STOP kommando fra grafana
├── config.json          # MQTT & hardware konfigurationsfil
├── Makefile             # Kompileringsscript
├── include/             # C++ header-filer (*.hpp)
├── src/                 # C++ kildekode (*.cpp)
│   ├── main.cpp         # Hovedløkke & tilstandsmaskine
│   ├── motor.cpp        # GPIO styring af H-bro / motorer
│   ├── lidar.cpp        # Seriel aflæsning og parsing af LiDAR data
│   ├── navigation.cpp   # Koordinater, gitterkort & grænsetjek
│   └── network.cpp      # MQTT forbindelse og telemetri
└── README.md
```
