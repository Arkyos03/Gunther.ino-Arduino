<div align="center">

# 🚀 Gunther.ino : carte minifusée (SAMD51)

**Ordinateur de bord standardisé pour minifusées**

Cette carte fait partie de la famille Gunther.ino : elle utilise la [bibliothèque commune `Gunther`](../../README.md).

*by Arkyos03 for AIR ESIEA*

![Version](https://img.shields.io/badge/version-0.1.5-blue)
![Arduino](https://img.shields.io/badge/Arduino-IDE%202.x-00979D?logo=arduino&logoColor=white)
![MCU](https://img.shields.io/badge/MCU-ATSAMD51J20A-orange)
![Licence](https://img.shields.io/badge/licence-MIT-green)

</div>

---

## ✨ La carte

| | |
|---|---|
| 🧠 **Processeur** | ATSAMD51J20A, 120 MHz, 1 Mo de flash |
| 🌡️ **Baromètre** | BMP390 / BMP388 / SPL07 (I2C) : pression, température, altitude |
| 🧭 **Centrale inertielle** | BMI088 (SPI) : accéléromètre + gyroscope |
| 💾 **Stockage** | Carte micro SD |
| ⚙️ **Sorties** | 4 servos, 4 LED, LED RGB d'état, buzzer |
| 🔌 **Entrées** | 4 switchs, 1 jack, mesure de la batterie |
| 🔋 **Alimentation** | Batterie 9 V ou USB-C |

---

## 📥 Installation

1. Dans l'Arduino IDE, ouvrir **Fichier › Préférences**.
2. Dans **URL de gestionnaire de cartes supplémentaires**, coller :
   ```
   https://raw.githubusercontent.com/Arkyos03/Gunther.ino-Arduino/main/package_gunther_index.json
   ```
3. Ouvrir **Outils › Type de carte › Gestionnaire de cartes**, chercher **Gunther.ino SAMD51** et cliquer sur **Installer**.
4. Choisir **Outils › Type de carte › Gunther.ino (Rev A)**, puis le port USB.

C'est prêt : on programme la carte comme un Arduino classique.

---

## 📌 Brochage

| Nom dans le code | Rôle |
|---|---|
| `LED_BUILTIN` | LED RGB de la carte (SK6805, PA08) |
| `LED_1` … `LED_4` | LED sur bornier |
| `SWITCH_1` … `SWITCH_4` | Switchs sur bornier |
| `JACK_1` | Jack de décollage |
| `SERVO_1` … `SERVO_4` | Servomoteurs |
| `BUZZER` | Buzzer de la carte |
| `VBAT_SENSE` (`A0`) | Tension de la pile ÷ 4 |
| `BMP390_INT` | Interruption du baromètre |
| `BMI088_ACCEL_INT` · `BMI088_GYRO_INT` | Interruptions de la centrale |
| `Serial` | Moniteur série (USB) |
| `Serial1` | UART d'extension |
| `Wire` | Bus I2C (BMP390 + capteurs externes) |
| `SPI` · `SD_CS` | Carte micro SD |
| `SPI1` · `BMI088_ACCEL_CS` · `BMI088_GYRO_CS` | Centrale inertielle |
| `SPI1` · `SPI1_CS_EXT` | Capteur SPI externe |

---

## 🧭 Deux façons de programmer la carte

| | Mode simplifié | Mode classique |
|---|---|---|
| Pour qui | Débutants, premiers vols | Ceux qui veulent tout coder eux-mêmes |
| Comment | `#include <Gunther.h>` puis `Gunther.altitude()`, `Gunther.apogee()`... | Code Arduino standard, comme une Nano, avec les noms des broches |
| Exemples | **Fichier › Exemples › Gunther** | **Fichier › Exemples › Gunther Classique** |

On peut aussi mélanger les deux dans le même programme.

### ⚡ Mode simplifié

```cpp
#include <Gunther.h>

void setup() {
  Gunther.begin();
  Gunther.servoClose(1);            // trappe fermée
  Gunther.selfTest();               // 3 bips = tout est OK
  Gunther.logBegin();               // nouveau fichier vol_XXX.csv
}

void loop() {
  Gunther.update();
  Gunther.logFlight();              // altitude, accéléros, gyros... sur la carte SD
  if (Gunther.apogee()) {
    Gunther.servoOpen(1);           // ouverture du parachute
  }
}
```

| Fonction | Effet |
|---|---|
| `Gunther.begin()` | Prépare la carte, démarre les capteurs, prend l'altitude du sol |
| `Gunther.update()` | Lit les capteurs et suit le vol (à appeler dans `loop()`) |
| `Gunther.selfTest()` | Teste capteurs, carte SD et pile (3 bips = OK) |
| `Gunther.altitude()` · `Gunther.pressure()` · `Gunther.temperature()` · `Gunther.verticalSpeed()` | Baromètre |
| `Gunther.accel(x, y, z)` · `Gunther.gyro(x, y, z)` · `Gunther.gForce()` | Centrale inertielle BMI088 |
| `Gunther.liftoff()` · `Gunther.apogee()` · `Gunther.landed()` · `Gunther.flightTime()` · `Gunther.maxAltitude()` | Phases du vol |
| `Gunther.servoOpen(1)` · `Gunther.servoClose(1)` · `Gunther.servoDetach(1)` | Servos d'éjection |
| `Gunther.logBegin()` · `Gunther.logFlight()` · `Gunther.log(a, b...)` | Enregistrement sur la carte SD (`vol_001.csv`...) |
| `Gunther.printFlight(Serial1)` | Même ligne envoyée sur un port série (radio de télémétrie) |
| `Gunther.led(1, true)` · `Gunther.switchClosed(2)` · `Gunther.jackPlugged()` | LED, switchs, jack |
| `Gunther.beep()` · `Gunther.beepAltitude()` · `Gunther.batteryVoltage()` · `Gunther.lowBattery()` | Buzzer et pile |
| `Gunther.rgb(r, v, b)` · `Gunther.statusLed(true)` · `Gunther.setStatusLed(broche)` | LED RGB : couleur libre, retour aux animations, LED d'état externe |

#### 🚦 LED RGB d'état

Après `Gunther.begin()`, la LED RGB de la carte montre toute seule ce qui se passe :

| LED | Signification |
|---|---|
| 🔵 bleu qui respire | Démarrage |
| 🟢 3 flashs verts | Auto-test OK |
| 🔴 flashs rouges | Erreur : 1 = baromètre, 2 = centrale, 3 = carte SD, 4 = pile |
| 🟢 vert qui respire | Prête, enregistrement en cours |
| 🩵 cyan qui respire | Prête, sans enregistrement |
| 🟠 orange qui clignote | Pile faible |
| ⚪ flash blanc | Décollage, apogée |
| 🔵 bleu rapide | Montée |
| 🟣 violet rapide | Descente |
| ⚪ double flash blanc | Posée (balise pour retrouver la fusée) |

### 🔧 Mode classique

```cpp
#include <Servo.h>

Servo trappe;

void setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(JACK_1, INPUT_PULLDOWN);
  trappe.attach(SERVO_1);
}

void loop() {
  if (digitalRead(JACK_1) == LOW) {    // jack arraché
    digitalWrite(LED_1, HIGH);
    tone(BUZZER, 4000, 100);
  }
}
```

| Disponible | |
|---|---|
| Entrées / sorties | `pinMode` · `digitalWrite` · `digitalRead` · `attachInterrupt` · `pulseIn` · `shiftOut` |
| LED RGB | `digitalWrite(LED_BUILTIN, HIGH)` (blanc) · `rgbLedWrite(LED_BUILTIN, r, v, b)` · aussi pour une WS2812 externe |
| Analogique | `analogRead` · `analogReadResolution` · `analogReference` · `analogWrite` (PWM, et vrai analogique sur PA02 / PA05) |
| Temps, sons | `millis` · `micros` · `delay` · `tone` · `noTone` |
| Communication | `Serial` (USB) · `Serial1` (UART) · `Wire` (I2C maître et esclave) · `SPI` (carte SD) · `SPI1` (BMI088 et capteur externe) |
| Bibliothèques incluses | `Servo` · `SdFat` · `EEPROM` (4 Ko, gardée après coupure) · `Watchdog` |

Les bibliothèques **Adafruit BMP3XX** et **Bolder Flight BMI088** du gestionnaire de bibliothèques fonctionnent aussi.

---

## 🧰 Menu Outils

| Menu | Choix |
|---|---|
| **Optimisation** | Petite taille (par défaut), Rapide, Plus rapide, Débogage |
| **Programmateur** | CMSIS-DAP (Raspberry Pi Debug Probe, Atmel-ICE...) ou ST-Link V2 : graver le bootloader, téléverser sans USB, déboguer |
| **Bouton Déboguer** (IDE 2) | Points d'arrêt et registres du SAMD51 avec une sonde SWD (choisir *Optimisation › Débogage*) |

---

## 📚 Documentation

Manuels d'utilisation (Word) dans [`docs/samd51`](../../docs/samd51) : mode simplifié et mode classique.

---

## 🛠️ En cas de problème

Si le téléversement échoue (programme planté) :
**appuyer deux fois rapidement sur RESET**. La LED 1 pulse, la carte est prête : relancer le téléversement.

---

<div align="center">

Licence MIT · Arkyos03 for AIR ESIEA

</div>
