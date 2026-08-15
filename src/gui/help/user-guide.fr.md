# Utiliser ImageWorker

ImageWorker répond à deux questions au sujet d'un dossier de fichiers `png` et
`jpg` :

- **Où cet asset apparaît-il dans ma capture d'écran ?** — la tâche principale.
- **Quels fichiers de ce dossier sont des doublons ?** — la tâche secondaire.

---

## 1. Désigner un dossier

**Images** est le dossier à explorer. **Index dans** est l'endroit où
ImageWorker garde ses propres données : la base, les aperçus et les
descripteurs.

Par défaut l'index va dans un dossier caché `.imageworker` à l'intérieur du
dossier d'images. Changez **Index dans** lorsque les images vivent sur un
partage réseau ou en lecture seule, lorsque vous préférez ne rien ajouter au
dépôt de quelqu'un d'autre, ou lorsque l'index a sa place sur un disque plus
rapide. **Par défaut** remet tout en place.

Appuyez sur **Indexer**. Le premier passage lit chaque fichier ; les suivants ne
relisent que ceux dont la taille ou la date de modification a changé, si bien que
maintenir l'index à jour ne coûte presque rien.

> Indexer 4600 images pesant environ 400 Mo prend à peu près trois minutes,
> descripteurs neuronaux compris.

---

## 2. Trouver un asset dans une capture d'écran

Ouvrez **Recherche par l'exemple**, puis faites entrer la capture :

- appuyez sur **Ctrl+V** pour la coller depuis le presse-papiers — cela marche
  depuis les deux onglets ;
- faites glisser un fichier image sur la fenêtre ;
- ou utilisez **Parcourir…**.

**Tracez un cadre autour de l'objet cherché.** Cela compte plus qu'il n'y
paraît : sur une image entière, la recherche dépense l'essentiel de son effort
sur le décor. Sur une bibliothèque de 4600 assets, un objet encadré prend environ
**2,6 secondes contre 5,6** pour l'image entière, et trouve à peu près **deux
fois plus de points correspondants**, parce que chacun d'eux tombe sur l'objet et
non sur le décor. Cliquez en dehors du cadre pour l'effacer et chercher de
nouveau sur l'image entière.

Appuyez sur **Rechercher**. Le meilleur résultat est entouré sur la capture, et
tous les résultats apparaissent en dessous, classés.

### Lire les résultats

Chaque résultat affiche un **score** et un **nombre d'inliers**. Les inliers sont
les points appariés qui s'accordent sur un même placement cohérent : c'est là
qu'est la preuve. Le score pèse la *cohérence* de ces appariements, et diminue
quand ils sont peu nombreux.

**Le résultat est une liste classée, pas une réponse par oui ou non.** Un autre
dessin du même personnage passera aussi la vérification, parce que l'essentiel de
l'image correspond réellement. Le bon se classe nettement au-dessus : lisez donc
l'ordre au lieu de chercher un vainqueur unique.

### Méthode

- **Localiser un objet dans l'image** — la recherche neuronale décrite ci-dessus.
  Exige un index contenant des descripteurs ; si l'entrée est grisée, voyez plus
  bas.
- **Similarité de l'image entière** — une empreinte perceptuelle qui compare les
  images dans leur ensemble. Efficace pour retrouver une copie redimensionnée ou
  réencodée de *la même image*, inutile pour trouver un petit objet dans une
  capture.

Si **Localiser un objet** est désactivé, c'est que l'index n'a pas encore de
descripteurs. Construisez-les une fois en ligne de commande :

```
imageworker index "<dossier d'images>" --db "<dossier d'index>\index.db" --features
imageworker vocab "<dossier d'images>" --db "<dossier d'index>\index.db"
```

---

## 3. Trouver les doublons

Ouvrez **Doublons**, réglez **Distance max.** et appuyez sur **Chercher les
doublons**.

- Les groupes **exacts** sont des fichiers identiques octet pour octet. On peut
  agir dessus sans crainte.
- Les groupes **proches** se ressemblent aux yeux d'une empreinte perceptuelle.
  **Examinez-les, ne les supprimez pas à l'aveugle.** L'empreinte décrit la
  composition, donc un même personnage dessiné avec plusieurs expressions atterrit
  dans un seul groupe à toute distance utilisable.

Une distance de 0 signifie des empreintes identiques ; 4 tolère le réencodage et
le redimensionnement. Au-delà de 8, les groupes ne servent qu'à l'examen.

Les groupes sont classés par l'espace qu'ils gaspillent, du plus gros au plus
petit. À l'intérieur d'un groupe, le fichier le plus gros et le mieux défini
vient en premier — en général celui qu'on garde.

---

## 4. Récupérer les chemins

Chaque résultat porte son chemin complet :

- la barre du bas affiche le chemin de la sélection courante, et **Copier** copie
  tous les chemins sélectionnés, un par ligne ;
- la petite icône de copie à côté d'un nom de fichier ne copie que celui-là ;
- le clic droit propose le chemin complet, le chemin relatif au dossier, ou
  l'affichage du fichier dans l'explorateur.

---

## 5. Les autres branches git

Si le dossier d'images se trouve dans un dépôt git, **Branches…** devient
disponible. Cochez les branches voulues et appuyez sur **Indexer** : leurs images
sont lues directement dans le dépôt, rien n'est extrait et votre copie de travail
n'est jamais touchée. La branche que vous avez extraite ne figure pas dans la
liste — elle *est* la copie de travail.

La liste des branches décrit exactement ce que contient l'index. Décocher une
branche et réindexer la retire.

Les résultats venant d'une branche affichent son nom et s'identifient par
`branche:chemin` plutôt que par un chemin de fichier, car ce fichier n'existe pas
sur le disque. **Ouvrir** extrait cette version dans un fichier temporaire pour
que vous puissiez la regarder ; l'afficher dans le gestionnaire de fichiers et
chercher avec elle ne valent que pour les fichiers de la copie de travail.

Réindexer coûte peu : une branche dont la pointe n'a pas bougé est entièrement
sautée, et à l'intérieur d'une branche seuls les fichiers dont le contenu a
réellement changé sont relus.

> **Git LFS** : les images proviennent de votre dépôt LFS local. Si une branche
> n'a jamais été récupérée, ImageWorker indique combien d'objets manquent et que
> `git lfs fetch --all` les rapatrie — il ne télécharge jamais rien de lui-même.

---

## 6. Thèmes et langue

**Affichage → Thème** bascule entre Système, Sci-fi, Material et Floraison
pendant que le programme tourne. Le choix est retenu.

**Affichage → Langue** change l'interface, également sans redémarrage. La liste
contient toutes les langues fournies par cette version ; **Langue du système**
suit le système d'exploitation. Ce manuel est traduit lui aussi, et retombe sur
l'anglais pour une langue dont le manuel n'est pas encore prêt.

---

## Ce qu'il ne sait pas faire

- L'empreinte perceptuelle retrouve **la même image**, jamais simplement un sujet
  voisin.
- La recherche neuronale trouve des assets **réellement présents** dans l'image.
  Ce n'est pas une recherche par description.
- Un asset qui ne produit aucun point d'intérêt — un aplat de couleur, une icône
  minuscule — ne peut pas être localisé. Environ 4 % d'une bibliothèque d'assets
  typique tombent dans cette catégorie.
