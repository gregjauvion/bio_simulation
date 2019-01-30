#include "model.h"

/* Displays as a string and reads from a string */
json Model::toJson() {
    json model;
    // Add molecules, reactions and parameters
    json molecules_json;
    json reactions_json;
    json parameters_json;
    for (auto e : molecules)
        molecules_json.push_back(e->toJson());
    for (auto e : reactions)
        reactions_json.push_back(e->toJson());
    for (auto e : parameters)
        parameters_json.push_back(e->toJson());
    model["molecules"] = molecules_json;
    model["reactions"] = reactions_json;
    model["parameters"] = parameters_json;
    // Add other parameters
    model["growth"] = growth->toString();
    model["division"] = division->toString();
    model["growth_rate"] = growth_rate->toString();
    model["division_trigger"] = division_trigger->toString();
    model["volume_share"] = volume_share->toString();
    return model;
}

Model* Model::fromJson(json* model_ptr) {
    json model = *model_ptr;

    // Parse the parameters
    vector<Parameter*> parameters;
    json parameters_json = model["parameters"];
    for (json::iterator it = parameters_json.begin(); it != parameters_json.end(); it++) {
        string name = (*it)["name"];
        double value = stod((*it)["value"].get<string>());
        parameters.push_back(new Parameter(name, value));
    }

    // Parse the molecules
    vector<Molecule*> molecules;
    int mol_index = 0;
    json molecules_json = model["molecules"];
    for (json::iterator it = molecules_json.begin(); it != molecules_json.end(); it++) {
        string name = (*it)["name"];
        int split = (*it)["split"];
        SplitType split_type;
        switch (split) {
        case SPLIT_EQUAL: split_type = SPLIT_EQUAL;
            break;
        case SPLIT_SHARE_DEPENDENT: split_type = SPLIT_SHARE_DEPENDENT;
            break;
        case SPLIT_IDENTICAL: split_type = SPLIT_IDENTICAL;
            break;
        }
        molecules.push_back(new Molecule(mol_index++, name, split_type));
    }

    // Build the parser
    Parser* parser = new Parser(molecules, parameters);

    // Build the reactions
    vector<Reaction*> reactions;
    json reactions_json = model["reactions"];
    for (json::iterator it = reactions_json.begin(); it != reactions_json.end(); it++) {
        // Build the left molecules
        string left_str = Utils::replace((*it)[0], " ", "");
        vector<Molecule*> left;
        for (auto el : Utils::split(left_str, Reaction::MOL_SEPARATOR))
            for (auto m : parser->getMolecules())
                if (el.compare(m->getName())==0)
                    left.push_back(m);
        // Build the right molecules
        string right_str = Utils::replace((*it)[1], " ", "");
        vector<Molecule*> right;
        for (auto el : Utils::split(right_str, Reaction::MOL_SEPARATOR))
            for (auto m : parser->getMolecules())
                if (el.compare(m->getName())==0)
                    right.push_back(m);
        // Build the rate function
        string rate_str = (*it)[2];
        Func<double>* rate = parser->parseDouble(rate_str);

        reactions.push_back(new Reaction(left, right, rate));
    }

    // Volume share and volume trigger
    string volume_share_str = model["volume_share"];
    string division_trigger_str = model["division_trigger"];
    Func<double>* volume_share = parser->parseDouble(volume_share_str);
    Func<bool>* division_trigger = parser->parseBool(division_trigger_str);

    // Growth
    json growth_json = model["growth"];
    string growth_type = growth_json["type"];
    string growth_parameter = growth_json["parameter"];
    Func<double>* growth_rate = parser->parseDouble(growth_parameter);
    Growth* growth = NULL;
    if (growth_type.compare("exp")==0) {
        growth = new ExpGrowth();
    } else if (growth_type.compare("formula")==0) {
        growth = new FormulaGrowth();
    }

    return new Model(molecules, reactions, growth, new Division(), parameters, growth_rate, division_trigger, volume_share);
}
