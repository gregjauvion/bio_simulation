
# Interface

Chaque utilisateur a accés à son dashboard lui permettant d'afficher les résultats de ses simulations et d'en lancer de nouvelles. Une barre de navigation sur la gauche liste tous les modèles de l'utilisateur.

On a deux modes d'affichage d'un modèle donné :

* Un mode figé (mode par défaut), où le modèle est présenté mais ne peut pas être modifié
* Un mode édition (accessible avec un bouton), dans lequel le modèle peut être édité. On a alors 2 options : annuler les modifications et repasser en mode figé, ou sauver les modifications, le modèle est alors modifié et la simulation est relancée. Lors de la sauvegarde d'un modèle, on commence par supprimer toutes les simulations existantes pour ce modèle (pour s'assurer que l'on a une seule simulation par modèle)

Suppression d'un modèle : chaque modèle peut être supprimé par l'interface en cliquant sur la corbeille à côté de son nom. Cela supprime également toutes les simulations pour ce modèle.

L'affichage des résultats d'une simulation se fait de la manière suivante :

* S'il n'y a pas de simulation pour ce modèle, cela signifie que le calcul est en cours (il faut alors s'assurer quand on lance la simulation que quelque chose est inséré dans la base même si la simulation a planté)
* Si il y a une simulation dont la valeur associée à "success" est False, cela signifie que la simulation a planté, et on affiche un message d'erreur sur le dashboard
* Si il y a une simulation dont la valeur associée à "success" est True, les résultats de la simulation se trouvent alors dans la valeur associée à "output"
