
# Simulation

### Définition d'une simulation

Une simulation est construite par la donnée des éléments suivants :

* Un modèle, tel que décrit dans la partie correspondante
* Une liste de lineages initiaux servant de point de départ à la simulation. Un lineage n'est rien qu'une liste de cellules (initialisé généralement avec une seule cellule)
* Le paramétrage de la simulation

Le paramétrage est constitué des éléments suivants :

* Un booléen indiquant si la simulation est stochastique (en terme des réactions moléculaires)
* Un booléen "mother machine", indiquant si l'on garde une ou les deux cellules après chaque division
* Une fonction booléenne prenant le temps et la population indiquant si l'on arrête la simulation

Les settings d'une simulation permettent aussi de définir une burning period, qui est une période pendant laquelle les quantités de certaines molécules restent nulles (ces molécules sont identifiées dans un tableau de booléens), et pendant laquelle les outputs ne sont pas écrits :

* Durant la burning period, ces molécules restent en quantité nulle (initialisées à 0 et settées à 0 après chaque timestep)
* A la fin de la burning period, i.e dés que le temps dépasse la durée de la burning period spécifiée dans les settings, ces molécules sont initialisées en utilisant le setting quantities_initialization

~~~
De manière générale, il faudrait peut-être détecter les 'trucs bizarres mais pas mortel' pendant la simu et intégrer l'info sous forme de warnings dans les outputs
~~~

L'output de la simulation est un json contenant les clés suivantes :

* data : contient une liste stockant l'ensemble des outputs. Chaque élément de la liste est une liste de la forme [t, V, q1, ..., qn], où t est le temps, V le volume de la cellule et q1, ..., qn les quantités des n molécules de la cellule
* cell_cycles : une liste contenant, pour chaque cycle cellulaire, la liste des indices des outputs dans "data". La taille de la liste est alors le nombre de cycles cellulaires observés dans la simulation
* lineages : donne, pour chaque lineage initial, la liste des indices des cycles cellulaires correspondants

### Serveur de calcul

Les simulations sont lancées à l'aide d'un serveur http (détaillé dans la page Infrastructure) : la requête POST simulation avec les paramètres model et settings lance la simulation d'un modèle pour des settings donnés. Elle renvoie :

* Un json du type {"success": true, "output": {}} en cas de succés de la simulation
* {"success": false} en cas d'échec de la simulation (i.e plantage du code c++)

Une remarque : actuellement le serveur n'est pas multi-threadé, c'est-à-dire qu'il ne peut traiter qu'une requête à la fois. Il est assez simple de le rendre multi-threadé, mais pas forcément utile pour le moment étant donnée la rapidité des simulations.
