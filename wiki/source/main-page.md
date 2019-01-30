
# BioSim webapp: project wiki

Pages centralisant des infos sur le projet: objectifs, documentation, statut, problèmes...

Pour éditer le contenu, modifier les fichiers *.md* dans source, puis relancer le shell script `compile.sh` (pandoc nécessaire, `brew install pandoc`) pour générer le *html*.

~~~~
Une façon simple d'insérer des commentaires à n'importe quel endroit: ~~~ Mon commentaire ~~~.
~~~~

Pour ajouter des pages, créer la page dans source puis coder sa création en éditant le script `compile.sh`.


## Stratégie

Ce qu'on veut faire, pourquoi on veut le faire, quels sont les clients potentiels, quels sont les concurrents...

Un peu prématuré de se casser la tête dessus au stade actuel, essayons d'abord d'avoir un joli prototype (mais il faut commencer à bien définir ce prototype, cf page correspondante).

### [Vision et mission](vision.html)
### [Services / technologies existants](existing-services.html)
### [Clients potentiels et comment les cibler](marketing.html)


## Technologie

Notre techno, qui a plusieurs aspects: la modélisation (en gros l'espace de modèles et comment spécifier des éléments de ce modèle), la simulation de modèles, l'interface web pour créer les modèles, la visualisation des résultats, l'infrastructure (architecture de la webapp, base de données, serveurs, ...).

Une page liste les idées d'évolutions par thème.

### [Modélisation](modeling.html)
### [Simulation](simulation.html)
### [Interface](interface.html)
### [Infrastructure](infrastructure.html)
### [Evolutions](evolutions.html)
