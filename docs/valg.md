Teknisk Begrundelse for Valget af C++
1. Deterministisk Ydelse og Reaktivitet (NFK2)
Robotten opererer under skrappe tidsrelevante krav, herunder NFK2, som foreskriver, at robotten skal reagere på en forhindring senest 1 sekund efter detektering. Python er et fortolket sprog, der er afhængig af dynamisk typning og en indbygget "Garbage Collector" (hukommelses-oprydning). Garbage Collection kan medføre uforudsigelige mikropauser i programmet (såkaldte pauses), hvilket er kritisk, når man modtager kontinuerlige datapakker fra en LiDAR med en baudrate på 230400. C++ kompileres direkte til maskinkode (native code), hvilket sikrer en deterministic og lynhurtig eksekvering uden uventede afbrydelser.

2. Ressourceoptimering på Embedded Hardware
En Raspberry Pi er en ressourcebegrænset embedded enhed, der kører flere baggrundsprocesser samtidigt (operativsystem, netværksstack, MQTT-broker og sensor-polling). C++ har et minimalt "footprint" på både CPU og RAM sammenlignet med Python, hvilket sikrer, at processoren ikke overbelastes, og at systemet forbliver stabilt over længere driftstider.

3. Synergi og Genbrug af Eksisterende Kodekompetencer
Projektgruppen havde forud for dette projekt opbygget stærke kompetencer i at implementere netværkskommunikation vha. C++ og libmosquitto-biblioteket. Ved at vælge C++ som gennemgående sprog for hele robotlogikken udnyttede vi denne eksisterende erfaring, hvilket minimerede integrationsrisici og undgik den kompleksitet og fejlrisiko, der er forbundet med at blande flere sprog (f.eks. ved at forsøge at køre C++ moduler via Python Foreign Function Interfaces).

4. Nære Hardware- og Systemintegrationer
Autonome robotter kræver tæt integration med systemets I/O-porte og serielle kommunikationskanaler (/dev/serial0). C++ tillader direkte POSIX-kompatibel systemprogrammering (f.eks. via <termios.h> og <fcntl.h> til LiDAR-data og std::system / direkte registerstyring til motorerne), hvilket giver fuld kontrol over hardwarens timing og datastrømme uden overhead fra et bakoversystem.


Valg af Navigationsstrategi og Odometry (Vektor-baseret Dead Reckoning)
For at opfylde FK3 (Position og retning) uden at anvende dyr eller kompleks GPS-hardware, har vi valgt en vektorbaseret odometry-løsning. Ved kontinuerligt at kombinere hjulencodere ($\Delta d$) med robottens aktuelle retningsvektor ($\theta$), beregner systemet relative $X, Y$-koordinater i et kartesisk koordinatsystem. Matematisk anvendes trigonometriske enhedsvektorer til at opdatere positionen dynamisk. Dette giver et præcist og omkostningseffektivt estimat af robottens placering i det afgrænsede testfelt.

Valg af Gitterkort og Procentvis Dækning (FK2 og FK8)
For at understøtte FK2 (Arbejdsområde) og FK8 (Rute/Checkpoints) har vi implementert et logisk gitterkort på $10 \times 10$ felter (svarende til $10\times10$ cm per celle på en $1\times1$ meter bane). Hver gang robotten krydser en ny celle, opdateres dens status i et boolsk array, og det samlede antal unikke besøgte celler oversættes til en procentvis dækning. Dette gør det muligt automatisk at udløse checkpoints for hver 20% dækning, hvilket giver serveren et overskueligt overblik over arbejdets fremdrift.

Forhindringsundgåelse og Reaktiv Arkitektur (FK4)
I overensstemmelse med FK4 skal robotten registrere forhindringer <= 30 cm og reagere prompte. Da komplekse ruteplanlægningsalgoritmer (såsom dynamisk væg-føring) er sårbare over for ukendte forhindringsstørrelser, har vi valgt en reaktiv "Bounce"-strategi. Når LiDAR'en detekterer en forhindring, standser robotten, bakker kontrolleret og udfører en diskret 90-graders drejning væk fra truslen. Dette sikrer robusthed, overholder kravet om reaktionstid (NFK2) og opretholder samtidig opdateringen af det interne gitterkort.

Trådløs Kommunikation og Protokolvalg (FK5, FK6 og FK11)
Kommunikation mellem robot og server er implementeret via MQTT (Message Queuing Telemetry Transport) ved hjælp af C++ og libmosquitto. MQTT er valgt på grund af sit letvægts publish/subscribe-mønster, hvilket minimerer båndbreddeforbruget og sikrer lav latens over Wi-Fi. Alle sensordata, positioner, dækningsgrader og forhindringstællere serialiseres til JSON-payloads og publiceres på emnet robot/telemetry. Samtidig lytter robotten på robot/commands for at eksekvere kritiske kontrolkommandoer som START og STOP (FK11).
