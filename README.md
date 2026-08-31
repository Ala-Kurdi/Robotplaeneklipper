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