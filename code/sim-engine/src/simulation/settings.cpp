#include "settings.h"

json Settings::toJson() {
    json settings;
    settings["nb_lineages"] = nb_lineages;
    settings["nb_lineages_sample"] = nb_lineages_sample;
    settings["mother_machine"] = mother_machine;
    settings["stochastic"] = stochastic;
    settings["volume_initialization"] = volume_initialization->toString();
    json quantities_initialization_json;
    for (auto q_init : quantities_initialization)
        quantities_initialization_json.push_back(q_init->toString());
    settings["quantities_initialization"] = quantities_initialization_json;
    settings["burn_duration"] = burn_duration;
    settings["zero_during_burn"] = zero_during_burn;
    settings["is_quantity_init_conc"] = is_quantity_init_conc;
    return settings;
}

Settings* Settings::fromJson(json *settings_ptr, Model* model) {
    json settings = *settings_ptr;

    // Build the parser (needed for expression)
    vector<Molecule*> molecules = model->getMolecules();
    vector<Parameter*> parameters = model->getParameters();
    Parser* parser = new Parser(molecules, parameters);

    // Get the volume initialization
    Func<double>* volume_initialization = parser->parseDouble(settings["volume_initialization"]);

    // Get the quantities initialization array
    vector<Func<double>*> quantities;
    json settings_q = settings["quantities_initialization"];
    for (json::iterator it = settings_q.begin(); it != settings_q.end(); it++) {
        quantities.push_back( parser->parseDouble(*it) );
    }

    // Get zero_during_burn array
    vector<bool> zero_during_burn;
    json settings_z = settings["zero_during_burn"];
    for (json::iterator it = settings_z.begin(); it != settings_z.end(); it++) {
        zero_during_burn.push_back(*it);
    }
    // Get is_quantity_init_conc array
    vector<bool> is_quantity_init_conc;
    json settings_isq = settings["is_quantity_init_conc"];
    for (json::iterator it = settings_isq.begin(); it != settings_isq.end(); it++) {
        is_quantity_init_conc.push_back(*it);
    }
    // Get other parameters
    int nb_lineages = settings["nb_lineages"];
    int nb_lineages_sample = settings["nb_lineages_sample"];
    bool mother_machine = settings["mother_machine"];
    bool stochastic = settings["stochastic"];
    double burn_duration = settings["burn_duration"];
    return new Settings(nb_lineages, nb_lineages_sample, mother_machine, stochastic, volume_initialization, quantities,
                        burn_duration, zero_during_burn, is_quantity_init_conc);
}
