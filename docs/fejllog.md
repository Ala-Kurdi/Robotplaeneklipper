# Fejllog

Her dokumenterer vi fejl og udfordringer, som opstår under projektet.

For hver relevant fejl beskriver vi:

- Dato
- Hvad vi arbejdede på
- Hvad vi forventede
- Hvad der skete
- Årsag
- Løsning
- Hvad vi lærte

---

## Fejl og udfordringer

### 04-09-2026 - Raspberry Pi og motorstyring

**Hvad vi arbejdede på:**  
Opsætning af Raspberry Pi og test af motorstyring med L9110S.

**Hvad vi forventede:**  
At kunne styre begge motorer frem, tilbage, til venstre, til højre og stoppe dem via GPIO.

**Hvad der skete:**  
Der opstod flere mindre problemer under opsætningen og testen.

#### Forkert hostname ved SSH
Vi prøvede først `raspberrypi.local`, men forbindelsen virkede ikke.

**Årsag:**  
Raspberry Pi'en var konfigureret med hostname `robotpi`.

**Løsning:**  
Vi brugte `robotpi.local`, og SSH-forbindelsen virkede.

**Hvad vi lærte:**  
Hostname skal passe med den værdi, der blev valgt under installationen.

---

#### `pinctrl version` virkede ikke
Kommandoen `pinctrl version` gav fejlen:

`Unknown command "version"`

**Årsag:**  
Den installerede version af `pinctrl` understøtter ikke kommandoen `version`.

**Løsning:**  
Vi brugte `pinctrl get` i stedet, og GPIO kunne læses korrekt.

**Hvad vi lærte:**  
En enkelt ukendt kommando betyder ikke, at GPIO-systemet ikke virker.

---

#### Motor-driver blev først identificeret forkert
Motor-driveren blev først antaget til at være en MX1508.

**Årsag:**  
Printet lignede en anden type motor-driver ved første kontrol.

**Løsning:**  
Efter et nærbillede af chippen blev den identificeret som L9110S.

**Hvad vi lærte:**  
Komponentens mærkning skal kontrolleres, før vi laver den endelige ledningsplan.

---

#### Forvirring omkring motorernes retning
Under de første tests virkede højre motor til at køre i forkert retning.

**Årsag:**  
Der var både forskel på fysisk motorretning og den retning, vi kiggede på robotten fra.

**Løsning:**  
Vi testede hver motor separat og definerede robotens fremadretning set ovenfra.

Den korrekte styring blev:

- Forward:
  - GPIO17 HIGH, GPIO27 LOW
  - GPIO22 HIGH, GPIO23 LOW
- Backward:
  - GPIO17 LOW, GPIO27 HIGH
  - GPIO22 LOW, GPIO23 HIGH
- Left:
  - Venstre motor baglæns
  - Højre motor fremad
- Right:
  - Venstre motor fremad
  - Højre motor baglæns
- Stop:
  - Alle fire GPIO LOW

Alle fem funktioner blev testet fysisk og virkede.

**Hvad vi lærte:**  
Motorretning skal defineres ud fra hele robotens bevægelse og ikke kun ud fra, hvordan et enkelt hjul ser ud til at dreje.

---

#### Motor fortsatte med at køre
Ved en test fortsatte motoren med at køre efter kommandoen.

**Årsag:**  
`pinctrl set ... dh` holder GPIO HIGH, indtil GPIO ændres igen.

**Løsning:**  
Vi satte alle fire motor-GPIO til LOW med en STOP-kommando.

**Hvad vi lærte:**  
GPIO-værdier forbliver aktive, indtil de ændres.

---

#### Git var ikke installeret på Raspberry Pi
Da vi kørte `git status`, fik vi:

`-bash: git: command not found`

**Årsag:**  
Git var ikke installeret på Raspberry Pi OS Lite.

**Løsning:**  
Vi installerede Git med `apt` og kontrollerede installationen med `git --version`.

**Hvad vi lærte:**  
Udviklingsværktøjer er ikke nødvendigvis installeret som standard på et Lite-system.

---

#### Lokal mappe var ikke et Git repository
Efter installation af Git gav `git status`:

`fatal: not a git repository`

**Årsag:**  
Den lokale projektmappe på Raspberry Pi var oprettet manuelt og var ikke klonet fra GitHub.

**Løsning:**  
Vi gemte den lokale mappe midlertidigt, klonede det eksisterende GitHub repository og kopierede vores nye motor-kode ind i det rigtige repository.

**Hvad vi lærte:**  
Det er bedre at arbejde direkte i det eksisterende repository, så Git-historikken ikke bliver delt i to.

---

### Første motorprogram

Vi lavede:

`robot/motor_test.sh`

Programmet indeholder funktionerne:

- forward
- backward
- left
- right
- stop

Vi testede scriptet med:

`./robot/motor_test.sh forward`

og:

`./robot/motor_test.sh stop`

Begge kommandoer virkede korrekt.

#### Git kendte ikke brugerens identitet
Da vi prøvede at lave det første commit på Raspberry Pi, fik vi fejlen:

`Author identity unknown`

**Årsag:**  
Git havde endnu ikke et brugernavn og en email konfigureret på Raspberry Pi.

**Løsning:**  
Vi konfigurerede Git med `user.name` og `user.email`.

**Hvad vi lærte:**  
En ny Git-installation skal have en brugeridentitet, før den kan oprette commits.
