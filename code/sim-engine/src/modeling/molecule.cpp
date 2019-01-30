#include "molecule.h"
#include "parsing/utils.h"

const string Molecule::SplitStr[] = {"quant_equal", "conc_equal", "conc_random"};

/* Displays all the attributes of a Molecule. */
json Molecule::toJson() {
    json molecule;
    molecule["name"] = name;
    molecule["split_type"] = Molecule::SplitStr[split_type];
    molecule["index"] = index;
    return molecule;
}
