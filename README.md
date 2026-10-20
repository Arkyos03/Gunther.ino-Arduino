# OBC-STD Minifs : paquet Arduino

Paquet Arduino IDE pour l'ordinateur de bord standardisé des minifusées (AIR ESIEA), basé sur l'**ATSAMD51J20A**.
Une fois installé, la carte apparaît dans *Outils > Type de carte* et se programme comme un Arduino classique.

Version **0.1.2** : première version, compilée et testée à la compilation. **Elle n'a pas encore été testée sur une carte réelle.**

## Contenu

```
samd51obc/                 <- la plateforme Arduino (ce qui est installé)
  platform.txt             recettes de compilation / téléversement
  boards.txt               définition de la carte
  programmers.txt          sondes SWD (pour graver le bootloader)
  cores/obc/               cœur Arduino écrit pour la carte
  variants/obc_std/        brochage de la carte + scripts de link
  libraries/               SPI, Wire, Servo, OBC (fonctions faciles) + exemples
  bootloaders/obc_std/     configuration du bootloader UF2
tools/
  make_release.py          fabrique l'archive + le fichier JSON du gestionnaire de cartes
  test_build.py            compile un sketch sans l'IDE (tests)
```

## Brochage (noms à utiliser dans le code)

| Nom | Broche | Rôle |
|---|---|---|
| `LED_1` … `LED_4` | PB10, PB11, PB22, PB23 | sorties LED (PWM sur LED_1/LED_2) |
| `SWITCH_1` … `SWITCH_4` | PB04 … PB07 | entrées switch (interruptions, analogique) |
| `JACK_1`, `JACK_2` | PB08, PB09 | entrées jack (interruptions, analogique) |
| `SERVO_1` … `SERVO_4` | PA18 … PA21 | servos (bibliothèque `Servo`) |
| `BUZZER` | PB30 | buzzer piézo (`tone()`) |
| `VBAT_SENSE` / `A0` | PB00 | tension batterie / 4 |
| `Serial` | USB | moniteur série |
| `Serial1` | PA22 TX / PA23 RX | UART d'extension |
| `Wire` | PB02 SDA / PB03 SCL | I2C : BMP390 (0x76) + capteurs externes |
| `SPI` | PA12 / PA13 / PA14, CS = `SD_CS` | carte micro SD |
| `SPI1` | PB12 / PB13 / PB14 | IMU BMI088 (`BMI088_ACCEL_CS`, `BMI088_GYRO_CS`) |
| `BMP390_INT`, `BMI088_ACCEL_INT`, `BMI088_GYRO_INT` | PB15, PB16, PB17 | interruptions capteurs |
| `SD_DETECT` | PA27 | HIGH = carte SD présente |

## Fonctions disponibles

* Arduino standard : `pinMode`, `digitalWrite/Read`, `analogRead` (10 bits par défaut, jusqu'à 12), `analogWrite`,
  `delay`, `millis`, `micros`, `attachInterrupt`, `tone`, `pulseIn`, `shiftOut`, `map`, `random`, `String`, `Serial.printf`…
* Bibliothèques fournies : `SPI`, `Wire`, `Servo`, `OBC`.
* `OBC` (pour les débutants) : `OBC.begin()`, `OBC.led(1, true)`, `OBC.switchClosed(2)`, `OBC.jackPlugged(1)`,
  `OBC.liftoff()`, `OBC.beep()`, `OBC.beepCount(3)`, `OBC.batteryVoltage()`, `OBC.sdCardPresent()`.
* Compatible (testé à la compilation) avec les bibliothèques du gestionnaire : **SdFat** (Bill Greiman),
  **Adafruit BMP3XX**, **Bolder Flight BMI088**.

`Serial.print()` ne bloque jamais le programme quand aucun PC n'est branché (important en vol).

## Installer (utilisateurs)

1. Arduino IDE > *Fichier > Préférences* > *URL de gestionnaire de cartes supplémentaires* : coller l'URL du fichier `package_obc_index.json`.
2. *Outils > Type de carte > Gestionnaire de cartes* : chercher **OBC-STD** > Installer.
3. Choisir *OBC-STD Minifs (Rev A)* et le port, puis Téléverser.

Si le téléversement ne marche pas (programme planté) : **double appui rapide sur RESET** → la LED 1 pulse, la carte est en mode bootloader, relancer le téléversement.

## Première mise en service d'une carte neuve (une seule fois)

Le SAMD51 neuf est vide : il faut graver le bootloader UF2 avec une sonde SWD.
Deux types de sondes sont gérés dans *Outils > Programmateur* :

* **CMSIS-DAP** : Raspberry Pi Debug Probe, Pico avec *debugprobe*, Atmel-ICE, MCU-Link…
* **ST-Link V2** (original ou clone). Si la connexion échoue, maintenir le bouton RESET de la carte
  et le relâcher au lancement d'OpenOCD (la broche RST des clones ne fonctionne souvent pas).

Câblage sur le connecteur SWD : SWCLK, SWDIO, GND, 3V3 (référence) et RESET.

1. Compiler le bootloader : copier `samd51obc/bootloaders/obc_std/uf2-board/` dans `uf2-samdx1/boards/obc_std/`,
   lancer `make BOARD=obc_std`, copier le `.bin` obtenu en `samd51obc/bootloaders/obc_std/bootloader-obc_std.bin`.
2. Arduino IDE : *Outils > Programmateur* > **CMSIS-DAP** ou **ST-Link V2**, puis *Outils > Graver la séquence d'initialisation*.

## Publier une version (mainteneur)

```
python3 tools/make_release.py https://github.com/<compte>/obc-arduino/releases/download/v0.1.2
```
Mettre en ligne les deux fichiers de `dist/` à cette adresse (par ex. une *release* GitHub).

## Avant de distribuer

* **VID/PID USB** : le code utilise `0x1209:0x0001`, le PID *de test* de pid.codes. Demander un PID gratuit
  (pid.codes pour un projet open-source, ou le programme de sous-licence VID de Microchip) et le changer dans
  `boards.txt`, `cores/obc/usb.c` et la config du bootloader.
* **Licence** : code original sous MIT (voir `LICENSE`), composants tiers sous Apache-2.0 / MIT (voir `THIRD_PARTY_NOTICES.md`).
  Ces licences permettent de distribuer librement, y compris commercialement. Il faut simplement garder leurs textes de licence.

## Limites de la version 0.1.0

* Pas encore validé sur carte réelle (horloges, USB, ADC, PWM, I2C, SPI à vérifier).
* Compilé avec GCC 13 ; l'IDE utilisera GCC 7-2017q4 (outil Arduino) : à vérifier au premier essai.
* Pas de mode esclave I2C, pas de DMA sur le SPI (vitesse SD ~1 Mo/s max), pas de DAC (`analogWrite` sur PA02).
* Le numéro de version et l'URL du site sont à compléter.
