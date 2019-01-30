#ifndef MODEL_H
#define MODEL_H

#include "vector"
#include "parsing/utils.h"
#include "parsing/func.h"
#include "cell_factory.h"
#include "parameter.h"
#include "molecule.h"
#include "reaction.h"
#include "division.h"
#include "growth.h"
#include "regex"
#include "json.h"

/*
 * This class defines a Model, defined by reactions happening and hypothesis in the growth and division of the cells.
 * The Model can be read from a file.
 */
class Model
{
public:
    Model(vector<Molecule*> molecules_, vector<Reaction*> reactions_, Growth* growth_, Division* division_,
          vector<Parameter*> parameters_, Func<double>* growth_rate_, Func<bool>* division_trigger_, Func<double>* volume_share_) :
        molecules(molecules_), reactions(reactions_), growth(growth_), division(division_),
        parameters(parameters_), growth_rate(growth_rate_), division_trigger(division_trigger_), volume_share(volume_share_) {}
    /* Getters */
    vector<Molecule*> getMolecules() { return molecules; }
    vector<Reaction*> getReactions() { return reactions; }
    Growth* getGrowth() { return growth; }
    Division* getDivision() { return division; }
    vector<Parameter*> getParameters() { return parameters; }
    Func<double>* getGrowthRate() { return growth_rate; }
    Func<bool>* getDivisionTrigger() { return division_trigger; }
    Func<double>* getVolumeShare() { return volume_share; }
    /* Displays as a string and reads from a json */
    json toJson();
    static Model* fromJson(json* model);
private:
    vector<Molecule*> molecules;
    vector<Reaction*> reactions;
    Growth* growth;
    Division* division;
    vector<Parameter*> parameters;
    Func<double>* growth_rate;
    Func<bool>* division_trigger;
    Func<double>* volume_share;
};

#endif // MODEL_H
