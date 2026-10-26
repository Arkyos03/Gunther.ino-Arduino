<div align="center">

# 🚀 Gunther.ino

**La bibliothèque Arduino commune à toutes les cartes Gunther.ino des fusées d'AIR ESIEA**

*by Arkyos03 for AIR ESIEA*

![Arduino](https://img.shields.io/badge/Arduino-IDE%202.x-00979D?logo=arduino&logoColor=white)
![Licence](https://img.shields.io/badge/licence-MIT-green)

</div>

---

## 💡 Le principe

Toutes les cartes Gunther.ino (minifusée, fusée expérimentale...) se programment **de la même façon** :

- **un seul lien** à coller dans l'Arduino IDE, qui propose toutes les cartes ;
- **une seule bibliothèque `Gunther`** : le même programme (`Gunther.begin()`, `Gunther.altitude()`,
  `Gunther.apogee()`, `Gunther.servoOpen(1)`...) fonctionne sur toutes les cartes ;
- les **mêmes noms de broches** (`LED_1`, `JACK_1`, `SERVO_1`, `BUZZER`...) sur toutes les cartes.

Un programme écrit pour la minifusée se reprend donc tel quel sur une autre carte Gunther.ino :
seul le choix de la carte change dans **Outils › Type de carte**.

---

## 🧩 Les cartes

| Carte | Fusée | Microcontrôleur | Paquet à installer | Version | Détails |
|---|---|---|---|---|---|
| **Gunther.ino (Rev A)** | Minifusée | ATSAMD51J20A | Gunther.ino SAMD51 | 0.1.5 | [platforms/samd51](platforms/samd51/README.md) |
| *Carte fusée expérimentale* | Fusée expérimentale | STM32F4 | Gunther.ino STM32 | à venir | [platforms/stm32](platforms/stm32/README.md) |

---

## 📥 Installation (toutes les cartes)

1. Dans l'Arduino IDE, ouvrir **Fichier › Préférences**.
2. Dans **URL de gestionnaire de cartes supplémentaires**, coller :
   ```
   https://raw.githubusercontent.com/Arkyos03/Gunther.ino-Arduino/main/package_gunther_index.json
   ```
3. **Outils › Type de carte › Gestionnaire de cartes** : chercher **Gunther.ino** et installer le paquet de sa carte.
4. Choisir la carte dans **Outils › Type de carte**, puis le port USB.

La bibliothèque `Gunther` est installée automatiquement avec chaque paquet : rien d'autre à faire.

---

## ⚡ Deux façons de programmer

| | Mode simplifié | Mode classique |
|---|---|---|
| Comment | `#include <Gunther.h>` puis `Gunther.begin()`, `Gunther.apogee()`... | Code Arduino standard (`pinMode`, `digitalWrite`, `Wire`, `SPI`...) avec les noms des broches |
| Commun à toutes les cartes | Oui : même bibliothèque, même programme | Les noms de broches oui, les détails matériels dépendent de la carte |
| Exemples | **Fichier › Exemples › Gunther** | **Fichier › Exemples › Gunther Classique** |

```cpp
#include <Gunther.h>

void setup() {
  Gunther.begin();          // capteurs, altitude du sol
  Gunther.servoClose(1);    // trappe fermée
  Gunther.selfTest();       // 3 bips = tout est OK
  Gunther.logBegin();       // nouveau fichier vol_XXX.csv
}

void loop() {
  Gunther.update();
  Gunther.logFlight();      // une ligne de données sur la carte SD
  if (Gunther.apogee()) Gunther.servoOpen(1);
}
```

La liste complète des fonctions et des animations de la LED d'état est dans le README de chaque carte
et dans les manuels (`docs/`).

---

## 🗂️ Organisation du dépôt

```
libraries/                  COMMUN À TOUTES LES CARTES
  Gunther/                  bibliothèque du mode simplifié (même API partout)
    src/Gunther_Port.h      ce qui dépend du microcontrôleur (un fichier par plateforme)
  SdFat/                    carte SD (MIT, Bill Greiman)

platforms/                  UNE PLATEFORME PAR FAMILLE DE MICROCONTRÔLEUR
  samd51/                   cartes SAMD51 : cœur Arduino, brochage, bootloader, Wire, SPI,
                            Servo, EEPROM, Watchdog, exemples du mode classique
  stm32/                    cartes STM32 (à venir, cœur STM32duino)

docs/
  samd51/                   manuels d'utilisation (mode simplifié et mode classique)

tools/
  make_release.py           crée l'archive d'une plateforme (avec les bibliothèques communes)
                            et met à jour le JSON
  test_build.py             compile un sketch SAMD51 sans l'IDE
  build_bootloader.sh       compile le bootloader UF2 de la carte SAMD51

package_gunther_index.json  index lu par l'Arduino IDE (toutes les plateformes)
```

---

## ➕ Ajouter une carte

1. **Même microcontrôleur qu'une plateforme existante** : ajouter la carte dans `platforms/<plateforme>/boards.txt`
   et son brochage dans `platforms/<plateforme>/variants/<carte>/`.
2. **Nouveau microcontrôleur** : créer `platforms/<plateforme>/` (voir [platforms/stm32](platforms/stm32/README.md))
   et, si besoin, un fichier `libraries/Gunther/src/Gunther_Port_<plateforme>.cpp`.

Pour que la bibliothèque `Gunther` marche sur la carte, le brochage (`variant.h`) doit définir :

| Obligatoire | Optionnel |
|---|---|
| `LED_1` … `LED_4`, `SWITCH_1` … `SWITCH_4`, `JACK_1`, `SERVO_1` … `SERVO_4`, `BUZZER`, `VBAT_SENSE`, `VBAT_DIVIDER_RATIO`, `SD_CS`, `BMI088_ACCEL_CS`, `BMI088_GYRO_CS` | `LED_RGB` (LED d'état), `SD_DETECT` + `SD_DETECT_WORKS`, `GUNTHER_BARO_WIRE` / `GUNTHER_IMU_SPI` / `GUNTHER_SD_SPI` (bus utilisés, par défaut `Wire` / `SPI1` / `SPI`) |

---

## 🚀 Publier une nouvelle version d'une plateforme

```
python3 tools/make_release.py samd51
```

1. Créer la release GitHub **`samd51-v<version>`** (ex. `samd51-v0.1.5`) et y déposer `dist/gunther-samd51-<version>.tar.bz2`.
2. Commit + push de `package_gunther_index.json`.

Chaque plateforme a ses propres versions : publier la carte STM32 ne change rien pour la carte SAMD51.
Les anciennes versions restent dans le JSON et peuvent toujours être installées.

---

<div align="center">

Licence MIT · Arkyos03 for AIR ESIEA

</div>
