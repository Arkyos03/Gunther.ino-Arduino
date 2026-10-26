<div align="center">

# 🚀 Gunther.ino

**Les cartes électroniques des fusées d'AIR ESIEA, programmables avec l'Arduino IDE**

*by Arkyos03 for AIR ESIEA*

![Arduino](https://img.shields.io/badge/Arduino-IDE%202.x-00979D?logo=arduino&logoColor=white)
![Licence](https://img.shields.io/badge/licence-MIT-green)

</div>

---

## 👋 C'est quoi ?

Gunther.ino, c'est la famille de cartes électroniques qu'on utilise dans les fusées d'AIR ESIEA :
la carte des minifusées pour commencer, et bientôt une carte plus costaude pour les fusées expérimentales.

L'idée de ce dépôt, c'est que **toutes ces cartes se programment de la même manière**. Il y a un seul lien
à ajouter dans l'Arduino IDE, une seule bibliothèque `Gunther`, et les mêmes noms de broches partout
(`LED_1`, `JACK_1`, `SERVO_1`, `BUZZER`...). Un programme écrit pour la minifusée peut donc être repris
sur une autre carte Gunther.ino sans tout réécrire : on change juste la carte dans **Outils › Type de carte**.

---

## 🧩 Les cartes

| Carte | Pour quelle fusée | Microcontrôleur | Où en est-on | Détails |
|---|---|---|---|---|
| **Gunther.ino (Rev A)** | Minifusée | ATSAMD51J20A | Disponible, v0.1.5 | [platforms/samd51](platforms/samd51/README.md) |
| *Carte expérimentale* | Fusée expérimentale | STM32F4 | En préparation | [platforms/stm32](platforms/stm32/README.md) |

---

## 📥 Installer les cartes dans l'Arduino IDE

1. Ouvre **Fichier › Préférences**.
2. Dans **URL de gestionnaire de cartes supplémentaires**, colle ce lien :
   ```
   https://raw.githubusercontent.com/Arkyos03/Gunther.ino-Arduino/main/package_gunther_index.json
   ```
3. Va dans **Outils › Type de carte › Gestionnaire de cartes**, cherche **Gunther.ino** et installe le paquet de ta carte.
4. Sélectionne ta carte dans **Outils › Type de carte**, choisis le port USB, et c'est parti.

Pas besoin d'installer la bibliothèque `Gunther` à part : elle arrive avec le paquet.

---

## ⚡ Programmer la carte

Tu as deux façons de faire, et tu peux mélanger les deux dans le même programme.

**Le mode simplifié**, pour aller vite : tu inclus `Gunther.h` et chaque action tient en une ligne.
La lecture des capteurs, la détection du décollage et de l'apogée, l'enregistrement sur la carte SD :
tout est déjà écrit et testé. C'est ce qu'on conseille pour un premier vol.

```cpp
#include <Gunther.h>

void setup() {
  Gunther.begin();          // démarre la carte et prend l'altitude du sol
  Gunther.servoClose(1);    // trappe fermée
  Gunther.selfTest();       // 3 bips = tout va bien
  Gunther.logBegin();       // nouveau fichier vol_XXX.csv sur la carte SD
}

void loop() {
  Gunther.update();
  Gunther.logFlight();      // une ligne de données par tour
  if (Gunther.apogee()) Gunther.servoOpen(1);   // on ouvre le parachute
}
```

**Le mode classique**, si tu préfères tout coder toi-même : la carte se programme comme une Arduino Nano,
avec `pinMode`, `digitalWrite`, `Wire`, `SPI`... et les noms des broches de la carte.

Les exemples sont dans **Fichier › Exemples › Gunther** (mode simplifié) et **Gunther Classique** (mode classique).
Toutes les fonctions sont détaillées dans le README de chaque carte et dans les manuels du dossier `docs/`.

---

## 🗂️ Comment le dépôt est rangé

```
libraries/        ce qui est commun à toutes les cartes
  Gunther/        la bibliothèque du mode simplifié
  SdFat/          la gestion de la carte SD

platforms/        une plateforme par famille de microcontrôleur
  samd51/         tout ce qui est propre au SAMD51 (cœur Arduino, brochage, bootloader...)
  stm32/          la future plateforme STM32

docs/             les manuels d'utilisation
tools/            les scripts pour compiler et publier
package_gunther_index.json    le fichier que lit l'Arduino IDE
```

La bibliothèque `Gunther` est écrite une seule fois pour toutes les cartes. Ce qui dépend vraiment du
microcontrôleur (la LED RGB, la mesure de la pile...) est isolé dans de petits fichiers `Gunther_Port_*.cpp`,
un par plateforme.

---

## ➕ Ajouter une nouvelle carte

- **Si elle utilise un microcontrôleur déjà géré**, il suffit de l'ajouter dans le `boards.txt` de sa plateforme
  et de décrire son brochage dans `variants/`.
- **Si c'est un nouveau microcontrôleur**, on crée un nouveau dossier dans `platforms/`
  (le README de [platforms/stm32](platforms/stm32/README.md) explique comment).

Pour que la bibliothèque `Gunther` fonctionne dessus, le brochage de la carte doit définir au minimum :
`LED_1` à `LED_4`, `SWITCH_1` à `SWITCH_4`, `JACK_1`, `SERVO_1` à `SERVO_4`, `BUZZER`, `VBAT_SENSE`,
`VBAT_DIVIDER_RATIO`, `SD_CS`, `BMI088_ACCEL_CS` et `BMI088_GYRO_CS`.

Selon la carte, on peut aussi définir `LED_RGB` (LED d'état), `SD_DETECT` avec `SD_DETECT_WORKS`,
et les bus utilisés : `GUNTHER_BARO_WIRE`, `GUNTHER_IMU_SPI`, `GUNTHER_SD_SPI`
(par défaut `Wire`, `SPI1` et `SPI`).

---

## 🚀 Publier une nouvelle version

```
python3 tools/make_release.py samd51
```

Le script prépare l'archive de la plateforme dans `dist/` et met à jour `package_gunther_index.json`. Ensuite :

1. Sur GitHub, crée une release nommée **`v<version>`** (par exemple `v0.1.5`) et ajoute-y l'archive.
   Si deux plateformes sortent avec le même numéro de version, leurs archives vont dans la même release.
2. Fais un commit et un push de `package_gunther_index.json`.

Chaque plateforme avance à son rythme : sortir une nouvelle version de la carte STM32 ne touche pas à celle
de la minifusée, et les anciennes versions restent installables.

---

<div align="center">

Licence MIT · Arkyos03 for AIR ESIEA

</div>
