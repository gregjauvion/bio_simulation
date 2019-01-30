#ifndef REACTION_H
#define REACTION_H

#include "string"
#include "molecule.h"
#include "parsing/func.h"
#include "parsing/utils.h"
#include "parsing/parser.h"
#include "map"
#include "regex"
#include "json.h"

using json = nlohmann::json;

/*
 * A Reaction from "left" molecules to "right" molecules.
 * The rate function returns the rate of the reaction.
 *
 */
class Reaction
{
public:
    Reaction(vector<Molecule*> left_, vector<Molecule*> right_, Func<double>* rate_) :
        left(left_), right(right_), rate(rate_) {}
    /* Getters */
    vector<Molecule*> getLeft() { return left; }
    vector<Molecule*> getRight() { return right; }
    Func<double>* getRate() { return rate; }
    /* Displays the reaction */
    json toJson();
    // The molecules separator in a reaction
    static const string MOL_SEPARATOR;
private:
    vector<Molecule*> left ;
    vector<Molecule*> right ;
    Func<double>* rate;
};

#endif // REACTION_H
