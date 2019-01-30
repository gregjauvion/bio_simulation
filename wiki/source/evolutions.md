
# Evolutions

Cette page rassemble toutes les idées d'évolutions, classées par thème.

## Modélisation

* Introduire des modèles plus généraux et plus puissants, mais du coup plus durs à spécifier (et peut-être simuler)

* Environnement dont la composition peut être affectée par les cellules (sinon des paramètres suffisent).

## Simulation

* Améliorations sur le code cpp :
	+ Analyser les fuites mémoire du code (avec walgrind par exemple)

* Ce serait bien de pouvoir 'tester' un paramètre sans le valider

## Interface

A faire pour finir la première version du site :

* Faire un user test avec des beaux modèles (francois)
	
	+ Linear noisy map, single gene expression
	
	+ Simple metabolism (lactase uri alon or kiviet et al)
	
	+ Coarse-grained physiology (we still miss the feature of no growth rate for volume change)
	
	+ Toggle Switch
	
	+ Toggle Switch external control (pour présenter la feature zero_during_burn: toutes les cellules sont dans l'état 1, et a t=0 inducer -> une fraction (mais pas toutes) switchent dans l'état 2)
	
	+ Repressilator

* Remarques sur le site :

	+ Je mettrais bien des onglets (Description/Results), mais peut être galère à faire

		+ Francois: a essayer peut-être, moi j'aime bien comme ça ?

		+ Ce à quoi je pensais : en haut on a en titre un truc du genre "Description   Results", avec celui sélectionné en gras et l'autre grisé mais cliquable, je vais le faire voir ce que ca donne et on choisira! mais bon pas prioritaire non plus...

* Faire une page 'contact' pour laisser un message

A faire plus tard :

* Penser à un refactoring de la partie simulation du cpp :

	+ Notamment, il faudrait pouvoir tester différents timesteps sans modifier toutes les variables. Ca permettra d'utiliser l'option 'formula growth' qui pour l'instant n'est pas utilisée

	+ Créer aussi des objets pour simplifier la partie simulation, car aujourd'hui on a des 'if (stochastic)' un peu moches, ce n'est pas flexible. (là-dessus j'aimerais bien y réfléchir tranquilou pour trouver une structure bien clean)

* Les graphes sont mieux qu'avant mais pas encore ouf ouf. Je me dis que j'aurais une idée plus claire de comment améliorer après avoir bossé sur les 3/4 examples qui seront dans le tutoriel.

* Utiliser Angular JS pour les futures interfaces du même type

* Tester le site sur d'autres types d'écrans / navigateurs pour voir si c'est toujours joli

## Infrastructure

* Voir comment faire des backup de la db
