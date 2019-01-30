
# Modélisation

Un modèle spécifie l'espace d'états possibles d'une population de cellules (et eventuellement de son environnement) et surtout sa dynamique, c'est-à-dire les règles qui dictent comment cette population de cellules évolue au cours du temps.


## Espace de model actuel et language correspondant

Pour l'instant on travaille sur un espace de modèles 'ni trop gros ni trop petit'.

On considère qu'un seul type de cellules, il n'y a pas d'interactions avec l'environnement, les cellules ne peuvent pas mourir, et on ne représente pas leur forme mais seulement leur taille ainsi que leur contenu moléculaire (seulement pour les molécules prises en compte dans le modèle).

### Langage utilisé pour décrire des formules

La librairie C++ de simulation définit un langage dans lequel l'utilisateur devra écrire des formules (représentant par exemple un taux de réaction, un seuil de division cellulaire, ...). Deux types d'expressions sont définies :

* **Expressions décimales** : expressions qui retournent un double lorsqu'elles sont évaluées (du type 'a+b')
* **Expressions booléennes** : expressions qui retournnent un booléen lorsqu'elles sont évaluées (du type 'a>b')

Ce langage est défini par les éléments suivants :

* **Constantes** : une valeur décimale du type 1.23 ou 5
* **Paramètres** : mu, sigma ou Abc. Un paramètre est toujours associé à une valeur numérique définie par l'utilisateur
* **Nombre absolu d'une molécule** : le nom de la molécule (A par exemple). On suppose alors que la molécule existe dans les réactions décrivant le modèle
* **Concentration d'une molécule** : le nom de la molécule entre crochets ([A] par exemple). On suppose alors que la molécule existe dans les réactions décrivant le modèle
* **Volume initial / volume courant d'une cellule** : V_birth et V représentent respectivement le volume initial et le volume courant d'une cellule. Les valeurs V et V_birth ne peuvent donc être utilisées pour représenter un autre paramètre du modèle.
* **Loi uniforme** : U(a,b) représente une loi uniforme entre a et b, où a et b sont deux formules.
* **Loi normale** : N(f1,f2) représente une loi normale de moyenne f1 et d'écart-type f2, où f1 et f2 sont deux formules.
* **Loi normale positive** : Npos(f1,f2) représente une loi normale positive (i.e on tire des points jusqu'à tomber sur une valeur positive) de moyenne f1 et d'écart-type f2, où f1 et f2 sont deux formules.
* **Loi log-normale** : LN(f1,f2) représente une loi log-normale de moyenne f1 et d'écart-type f2, où f1 et f2 sont deux formules.
* **Loi binomiale négative** : NB(f1,f2) représente une loi binomiale négative de paramètre f1 et f2, où f1 et f2 sont deux formules.
* **Somme / Différence / Produit / Division / Puissance de formules** : +, -, *, / et ^ sont utilisés entre deux formules pour représenter une somme / différence / produit / division / puissance de deux formules. Les opérateurs *, / et ^ sont prioritaires par rapport aux opérateurs + et -.
* **Expressions booléennes** : les expressions du type f1>f2 et f1<f2 sont définies, où f1 et f2 sont deux expressions décimales. L'évaluation de ces expressions renvoie un booléen.

Exemples de formules :

~~~~
1 / k * A
2 / k * A * [B]
3 / (N(a,c) + N([M1],[M2])) * (V_birth + V - N(1,3.45))
~~~~

### Description

On décrit dans cette partie les différentes composantes d'un modèle.

#### Cellule

Chaque cellule a un contenu moléculaire, représenté simplement par la quantité (absolue) de molécules de différent types (A,B,C..).
Différents modèles vont représenter différent types de molécules.

Ces molécules pourront être impliquées dans des réactions chimiques (cf plus bas) qui feront évoluer leur quantité.

Une cellule est caractérisée par :

* son volume initial et son volume courant
* un vecteur donnant les quantités de chaque molécule (en quantité absolue)
* un identifiant unique, l'identifiant de la cellule mère, l'indice de génération (0 initialement, et incrémenté de 1 à chaque division)
* des expressions donnant le taux de croissance exponentiel, le volume seuil pour la division et la proportion de molécules allant dans l'une des cellules filles (la proportion allant dans l'autre est déduite par différence)

#### Cell growth

Il y a deux façons de représenter la croissance des cellules:

* en spécifiant le taux de croissance exponentiel µ = (dV/dt) / V (par une formule tel que décrit plus haut)
	+  Remarque : si le taux de croissance est aléatoire, la partie non déterministe est tirée à la création de la cellule et reste constante durant le cycle cellulaire
* en spécifiant l'expression de V (par une formule), dépendant du contenu moléculaire (les quantités) et éventuellement de paramètres
	+ cela est utile quand par exemple pour les modèles 'whole-cell', ou le contenu moléculaire représenté dans le modèle est censé représenter tout le contenu de la cellule
	+ dans ce cas, ce sont les réactions chimiques qui *in-fine* gouvernent la croissance cellulaire

La croissance du modèle est configurée par un json contenant deux clés :

* **type** donne le type de croissance (vaut exp ou formula)
* **parameter** contient la formule paramétrant la croissance (soit le taux de croissance, soit la formule donnant V directement)

#### Chemical reactions

Une réaction chimique est représentée par sa 'stoechiometry' et son rate.

Le rate décrit le nombre de réactions par unité de temps en fonction de l'état cellulaire actuel (c'est une formule tel que décrit plus haut).

La stoechimétrie décrit ce qui se passe à chaque fois que la réaction se produit: quelles molécules disparaissent ('à gauche') et quelles molécules aparaissent ('à droite').

Examples de stoechiométries:

* A -> B (A se transforme en B)
* -> C (C apparait)
* A + B -> B (A disparait, equivalent à [A -> ], mais le rate peut dépendre de B)
* etc..

A noter que ce cadre est plus général que le cadre loi d'action de masse, où la stoechiométrie impose le rate.
Cela permet une plus grande flexibilité dans la modélisation (mais cela autorise des modèles pas du tout physique).

A noter aussi que ce formalisme se prète aussi bien à une interprétation stochastique des réactions (le rate est alors une propensité) ou déterministe (les rates apparaissent dans des équations différentielles).


#### Cell division: volume trigger

La division cellulaire se déclenche quand une expression booléenne renvoie true. Dans la configuration du modèle, le paramètre division_trigger contient l'expression booléenne qui est évaluée.

#### Cell division: realization

L'expression volume_share décrit le ratio de la taille d'une des cellules filles juste après la division par rapport à la taille de la cellule mère juste avant la division.
Par conservation de la taille totale l'autre cellule reçoit la différence.

Le contenu moléculaire est réparti entre les deux cellules filles de différentes manière selon le type de molécule (choix de modélisation de l'utilisateur) mais aussi du caractère stochastique ou déterministe de la simulation des réactions (pour l'instant choisi par 'nous' (l'outil de simulation) et pas au niveau du modèle).

Il y a deux options pour ce type de molécule:

1. SPLIT_EQUAL (même quantité = 50% dans la mesure du possible, independant du shre)
2. SPLIT_SHARE_DEPENDENT (i.e. quantité 'proportionelle' au share)

Dans le cas split equal, si simulation déterministe -> égal à 50% de la valeur mère.
Si simulation stochastique -> pareil si nombre pair de molécules, sinon n-1 reparti à 50% et proba 50% pour la molécule en surplus.

Dans le cas split share dependent, si simulation deterministe -> proportion = share, dans le cas stochastique -> probabilité = share pour chaque molécule.

### Language by examples

#### Repressilator

~~~~
Francois
Ici j'ai repris le .json de Greg.
Le volume trigger ne semble pas faire la linear noisy map: il faut un mot-clé pour V_{birth}
Il y en a un!
~~~~

* "molecules":
	+ {"name":"A","split":1}
	+ {"name":"B","split":1}
	+ {"name":"C","split":1}

* "parameters":
	+ {"name":"synth_A_max","value":"22.3"},
	+ {"name":"synth_B_max","value":"25.2"},
	+ {"name":"synth_C_max","value":"3.2"},
	+ {"name":"A_conc_rep","value":"3.4"},
	+ {"name":"B_conc_rep","value":"12.3"},
	+ {"name":"C_conc_rep","value":"4.1"},
	+ {"name":"mu","value":"0.02"},
	+ {"name":"a","value":"1"},
	+ {"name":"b","value":"0.1"},
	+ {"name":"split","value":"0.4"}

* "reactions":
	+ "" -> "A", "synth_A_max / ( 1 + [C] / C_conc_rep )"
	+ "" -> "B", "synth_B_max / ( 1 + [A] / A_conc_rep )"
	+ "" -> "C","synth_C_max / ( 1 + [B] / B_conc_rep )"

* "volume_trigger":"a+b"
* "volume_share":"split"
* "growth":"mu"
