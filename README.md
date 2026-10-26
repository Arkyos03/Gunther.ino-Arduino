<div align="center">

# Gunther.ino

**Les cartes électroniques des fusées d'AIR ESIEA, programmables avec l'Arduino IDE**

*by Arkyos03 for AIR ESIEA*

![Arduino](https://img.shields.io/badge/Arduino-IDE%202.x-00979D?logo=arduino&logoColor=white)
![Licence](https://img.shields.io/badge/licence-MIT-green)

</div>

---

## C'est quoi ?

Gunther.ino, c'est la famille de cartes électroniques qu'on utilise dans les fusées d'AIR ESIEA :
la carte des minifusées pour commencer, et bientôt une carte plus costaude pour les fusées expérimentales.

Toutes ces cartes se programment de la même manière : un seul lien à ajouter dans l'Arduino IDE,
une seule bibliothèque `Gunther`, et les mêmes noms de broches partout (`LED_1`, `JACK_1`, `SERVO_1`,
`BUZZER`...). Un programme écrit pour la minifusée peut donc être repris sur une autre carte Gunther.ino
sans tout réécrire : on change juste la carte dans **Outils › Type de carte**.

---

## Les cartes

| Carte | Pour quelle fusée | Où en est-on | Détails |
|---|---|---|---|
| **Gunther.ino (Rev A)** | Minifusée | Disponible | [Fiche de la carte](platforms/samd51/README.md) |
| *Carte expérimentale* | Fusée expérimentale | En préparation | – |

---

## Installer les cartes dans l'Arduino IDE

1. Ouvre **Fichier › Préférences**.
2. Dans **URL de gestionnaire de cartes supplémentaires**, colle ce lien :
   ```
   https://raw.githubusercontent.com/Arkyos03/Gunther.ino-Arduino/main/package_gunther_index.json
   ```
3. Va dans **Outils › Type de carte › Gestionnaire de cartes**, cherche **Gunther.ino** et clique sur **Installer**.
4. Sélectionne ta carte dans **Outils › Type de carte**, choisis le port USB, et c'est parti.

Pas besoin d'installer la bibliothèque `Gunther` à part : elle arrive avec le paquet.
Pour les mises à jour, l'IDE te prévient tout seul quand une nouvelle version est disponible.

---

## Programmer la carte

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

Des exemples prêts à téléverser sont dans **Fichier › Exemples › Gunther** (mode simplifié)
et **Fichier › Exemples › Gunther Classique** (mode classique).

---

## Documentation

- La [fiche de la carte minifusée](platforms/samd51/README.md) : brochage, liste des fonctions, signification des couleurs de la LED.
- Les manuels d'utilisation (mode simplifié et mode classique) dans le dossier [docs](docs/samd51).

---

## En cas de problème

**Le téléversement ne marche plus** (programme planté, port USB introuvable) : appuie deux fois rapidement
sur le bouton **RESET** de la carte. Elle passe en mode bootloader, un lecteur **GUNTHERBOOT** apparaît
sur le PC : relance simplement le téléversement.

**La carte n'apparaît pas dans le gestionnaire de cartes** : vérifie que le lien des préférences est copié
en entier (il doit finir par `.json`), puis ferme et rouvre l'IDE.

Pour toute autre question, contacte Arkyos03 ou l'association : air-esiea@et.esiea.fr

---

<div align="center">

Licence MIT · Arkyos03 for AIR ESIEA

</div>
