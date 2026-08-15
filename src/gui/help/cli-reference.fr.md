# Ligne de commande et automatisation

Le paquet livre **deux exécutables construits sur la même bibliothèque de
base** :

| | |
|---|---|
| `argus-gui` | la fenêtre dans laquelle vous lisez ceci |
| `argus` | sans interface ; tout ce que fait la GUI, plus des options d'indexation qu'elle n'expose pas |

Ils partagent toute la logique : un dossier indexé par l'un est lu par l'autre.
Aucun n'enveloppe l'autre — les deux lient le même cœur statique.

---

## Contrat de sortie

Appuyez-vous dessus pour scripter ; il est délibérément stable.

- `--json` écrit du **JSON délimité par des sauts de ligne sur stdout**, un objet
  par ligne. Chaque résultat porte à la fois `rel` (relatif au dossier indexé) et
  `path` (absolu).
- `--paths` écrit des **chemins absolus bruts**, un par ligne, pour les tubes.
  Dans `dupes`, une ligne vide sépare les groupes. L'option implique `--quiet`,
  et la combiner avec `--json` est une erreur plutôt qu'un choix silencieux entre
  les deux.
- La progression, les avertissements et les erreurs vont toujours sur **stderr**,
  jamais sur stdout.
- Les résultats sortent dans un ordre déterministe.
- Codes de retour : **0** quelque chose a été trouvé, **1** rien, **2** une
  erreur.

---

## Commandes

```
argus index  <dir> [--features] [--jobs N] [--ext png,jpg] [--force]
argus vocab  <dir> [--words 2048] [--sample 120000]
argus find   <dir> --image shot.png [--roi x,y,w,h] [--shortlist N] [--top N]
argus dupes  <dir> [--distance 4] [--exact-only] [--near-only]
argus query  <dir> --image ref.png [--top 20]
argus match  --query q.png --asset a.png
argus stats  <dir>
argus doctor [--extract img.png]
argus formats
```

Chaque commande accepte `--db <path>` pour placer l'index ailleurs que dans
`<dir>/.argus`.

### Préparer un dossier pour la recherche d'objet

```
argus index D:/game/Assets --db D:/indexes/game/index.db --features
argus vocab D:/game/Assets --db D:/indexes/game/index.db
```

`index` seul donne la détection de doublons. `--features` ajoute les descripteurs
neuronaux, et `vocab` entraîne par-dessus l'index de présélection ; les deux sont
nécessaires avant que `find` fonctionne. Relancez `vocab` après avoir ajouté un
gros lot d'assets, pas après chaque fichier.

### Rechercher

```
argus find D:/game/Assets --db D:/indexes/game/index.db --image shot.png --json
```

Passez `--roi x,y,w,h` quand vous savez où se trouve l'objet. C'est l'option la
plus efficace de toutes : sur une zone encadrée, la recherche est environ cinq
fois plus rapide et trouve à peu près deux fois plus de points correspondants.

### Les autres branches git

```
argus index D:/game/Assets --branches all
argus index D:/game/Assets --branches release/1.4,feature/new-ui
```

Les images sont lues dans la base d'objets : rien n'est extrait et la copie de
travail reste intacte. L'ensemble nommé est l'ensemble complet : une branche
indexée auparavant et non nommée de nouveau sort de l'index. `--branches all`
prend toutes les branches locales, et `--remote-branches` ajoute celles de suivi
distant.

Réindexer coûte peu. Une branche dont la pointe n'a pas bougé est sautée sans
lister le moindre arbre, et à l'intérieur d'une branche c'est l'identifiant de
blob qui décide — une adresse de contenu, donc une vérification plus solide que
la taille et la date utilisées pour les fichiers.

Les lignes issues d'une branche ne donnent jamais de chemin de système de
fichiers, puisqu'il n'y en a pas. La sortie normale et `--paths` écrivent la
syntaxe `branche:chemin` de git, que `git show` accepte ; `--json` porte `ref`,
`blob` et `rev` à la place de `path`.

Sous Git LFS, les objets sont résolus depuis le dépôt LFS local. Les manquants
sont comptés et signalés une fois, avec `git lfs fetch --all` comme remède ; rien
n'est jamais téléchargé tout seul.

### Enchaîner

```
argus dupes D:/game/Assets --paths > groups.txt
argus find  D:/game/Assets --image shot.png --paths | clip
```

---

## Vérifier l'installation

```
argus doctor
```

Indique les versions d'ONNX Runtime et d'OpenCV, les fournisseurs d'exécution
disponibles, et si une session DirectML peut réellement être créée sur ce pilote —
ce qui n'est pas la même chose que de voir le fournisseur simplement listé.
`--extract <image>` fait passer l'extracteur sur un fichier et indique combien de
points d'intérêt il a trouvés et en combien de temps.

---

## Où se trouve quoi

```
<dossier d'index>/
  index.db              SQLite : fichiers, empreintes, aperçus, décalages de descripteurs
  features/desc.f16     descripteurs, demi-précision
  features/kpts.f32     points d'intérêt
  features/vocab.bin    vocabulaire visuel
  features/bow.bin      index de présélection
```

Supprimer le dossier d'index remet tout à zéro et ne touche à rien d'autre. Les
chemins en base sont stockés **relativement au dossier indexé**, si bien qu'un
index reste valide quand le même stockage est monté ailleurs.

Les modèles sont cherchés dans `models/` à côté de l'exécutable, puis dans les
dossiers parents au-dessus — ainsi un paquet installé comme un arbre de
compilation fonctionnent sans configuration.

---

## Compiler

```
cmake --preset msvc-release -DARGUS_WITH_INFERENCE=ON
cmake --build --preset msvc-release
cmake --install build/msvc-release
```

`ARGUS_WITH_INFERENCE=OFF` compile sans ONNX Runtime ni OpenCV ; la
recherche de doublons fonctionne toujours, la recherche d'objet non.

L'étape d'installation assemble un dossier autonome — chaque dépendance à côté
des exécutables, rien lu depuis `PATH`. Un binaire unique est impossible : ONNX
Runtime avec DirectML n'existe qu'en DLL, et `DirectML.dll` est un redistribuable
qui ne peut pas être lié du tout.
