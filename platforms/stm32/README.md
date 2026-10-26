# Gunther.ino : plateforme STM32 (à venir)

Paquet Arduino des cartes Gunther.ino à base de STM32 (première carte prévue : fusée expérimentale, STM32F4).

## Principe

La plateforme réutilise le cœur officiel **STM32duino** (STMicroelectronics, licences libres BSD-3 / Apache-2.0)
au lieu de réécrire un cœur complet. Ce dossier ne contiendra que ce qui est propre aux cartes Gunther.ino :

```
platforms/stm32/
  boards.txt          cartes (build.core=STMicroelectronics:arduino, build.variant=<carte>)
  platform.txt        nom, version, éventuelles options propres
  variants/<carte>/   brochage de la carte (variant.h / variant.cpp, PeripheralPins)
  libraries/          bibliothèques propres à la plateforme (exemples du mode classique...)
  release.json        architecture (stm32gunther), outils nécessaires et bibliothèques
                      communes à inclure : "commonLibraries": ["Gunther", "SdFat"]
```

Côté utilisateur, l'URL du paquet STM32duino devra aussi être ajoutée dans l'IDE
(`https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json`)
et le paquet **STM32 MCU based boards** installé, car le cœur vient de là.

## Bibliothèque commune

La bibliothèque `Gunther` (racine du dépôt, `libraries/Gunther`) fonctionne déjà sur un cœur Arduino
quelconque grâce à `Gunther_Port_Generic.cpp`. Pour la LED RGB adressable et les animations pendant
`delay()`, ajouter `libraries/Gunther/src/Gunther_Port_STM32.cpp` (même fonctions que
`Gunther_Port_SAMD51.cpp`) et exclure le port générique pour cette architecture.

Le brochage de chaque carte doit définir les noms listés dans le README principal.

## Publier

Quand `platform.txt`, `boards.txt` et `release.json` existent :

```
python3 tools/make_release.py stm32
```

puis créer la release GitHub `v<version>` avec l'archive `dist/gunther-stm32-<version>.tar.bz2`.
