<div align="center">

# 🚀 OBC-STD Minifs

**Ordinateur de bord standardisé pour minifusées**

*by Arkyos03 for AIR ESIEA*

![Version](https://img.shields.io/badge/version-0.1.2-blue)
![Arduino](https://img.shields.io/badge/Arduino-IDE%202.x-00979D?logo=arduino&logoColor=white)
![MCU](https://img.shields.io/badge/MCU-ATSAMD51J20A-orange)
![Licence](https://img.shields.io/badge/licence-MIT-green)

</div>

---

## ✨ La carte

| | |
|---|---|
| 🧠 **Processeur** | ATSAMD51J20A, 120 MHz, 1 Mo de flash |
| 🌡️ **Baromètre** | BMP390 (I2C) : pression, température, altitude |
| 🧭 **Centrale inertielle** | BMI088 (SPI) : accéléromètre + gyroscope |
| 💾 **Stockage** | Carte micro SD |
| ⚙️ **Sorties** | 4 servos, 4 LED, buzzer |
| 🔌 **Entrées** | 4 switchs, 2 jacks, mesure de la batterie |
| 🔋 **Alimentation** | Batterie 9 V ou USB-C |

---

## 📥 Installation

1. Dans l'Arduino IDE, ouvrir **Fichier › Préférences**.
2. Dans **URL de gestionnaire de cartes supplémentaires**, coller :
   ```
   https://raw.githubusercontent.com/Arkyos03/OBC-Arduino/main/package_obc_index.json
   ```
3. Ouvrir **Outils › Type de carte › Gestionnaire de cartes**, chercher **OBC-STD** et cliquer sur **Installer**.
4. Choisir **Outils › Type de carte › OBC-STD Minifs**, puis le port USB.

C'est prêt : on programme la carte comme un Arduino classique.

---

## 📌 Brochage

| Nom dans le code | Rôle |
|---|---|
| `LED_1` … `LED_4` | LED sur bornier |
| `SWITCH_1` … `SWITCH_4` | Switchs sur bornier |
| `JACK_1`, `JACK_2` | Jacks de décollage |
| `SERVO_1` … `SERVO_4` | Servomoteurs |
| `BUZZER` | Buzzer de la carte |
| `Serial` | Moniteur série (USB) |
| `Serial1` | UART d'extension |
| `Wire` | Bus I2C (BMP390 + capteurs externes) |
| `SPI` · `SD_CS` | Carte micro SD |
| `SPI1` · `BMI088_ACCEL_CS` · `BMI088_GYRO_CS` | Centrale inertielle |

---

## ⚡ Exemple

```cpp
#include <OBC.h>
#include <Servo.h>

Servo trappe;

void setup() {
  OBC.begin();
  trappe.attach(SERVO_1);
  trappe.write(0);              // trappe fermée
}

void loop() {
  if (OBC.liftoff()) {          // le jack a été arraché
    delay(6000);
    trappe.write(90);           // ouverture du parachute
    OBC.beep();
  }
}
```

D'autres exemples sont dans **Fichier › Exemples › OBC**.

### Fonctions de la bibliothèque `OBC`

| Fonction | Effet |
|---|---|
| `OBC.begin()` | Prépare la carte |
| `OBC.led(1, true)` | Allume la LED 1 |
| `OBC.switchClosed(2)` | `true` si le switch 2 est fermé |
| `OBC.jackPlugged(1)` | `true` si le jack 1 est branché |
| `OBC.liftoff()` | `true` quand le jack de décollage est arraché |
| `OBC.beep()` | Bip du buzzer |
| `OBC.batteryVoltage()` | Tension de la batterie en volts |
| `OBC.sdCardPresent()` | `true` si une carte SD est insérée |

Les bibliothèques **SdFat**, **Adafruit BMP3XX** et **Bolder Flight BMI088** du gestionnaire de bibliothèques fonctionnent avec la carte.

---

## 🛠️ En cas de problème

Si le téléversement échoue (programme planté) :
**appuyer deux fois rapidement sur RESET**. La LED 1 pulse, la carte est prête : relancer le téléversement.

---

<div align="center">

Licence MIT · Arkyos03 for AIR ESIEA

</div>
