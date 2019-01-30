#ifndef MOLECULE_H
#define MOLECULE_H

using namespace std ;
#include "string"
#include "regex"
#include "json.h"

using json = nlohmann::json;

/* Defines the splitting methodology for the molecule :
 - SPLIT_EQUAL means that molecules will be equal'y splitted, independently of volume_share
   (but if stochastic reactions, when uneven #, modulo 1 molecule)
 - SPLIT_SHARE_DEPENDENT means that molecules are splitted proportionally to volume_share (randomly if reactions
   are simulated stochastically)
 - SPLIT_IDENTICAL means that the quantity in each of the daughter cells is the quantity in the mother cell
*/
enum SplitType { SPLIT_SHARE_DEPENDENT=0, SPLIT_EQUAL=1, SPLIT_IDENTICAL=2 } ;

/* Represents a Molecule which can react in a reaction. */
class Molecule
{
public:
    Molecule(int index_, string name_, SplitType split_type_) :
        index(index_), name(name_), split_type(split_type_) {}
    /* Getters */
    int getIndex() { return index; }
    string getName() { return name; }
    SplitType getSplitType() { return split_type; }
    /* Displays */
    static const string SplitStr[];
    json toJson();
private:
    // The index of the molecule, from 0 to (n-1) where n is the number of molecules in the system
    int index;
    string name;
    SplitType split_type;
};

#endif // MOLECULE_H
