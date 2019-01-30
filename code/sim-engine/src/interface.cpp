
using namespace std;

#include "string"
#include "json.h"
#include "modeling/model.h"
#include "simulation/settings.h"
#include "simulation/stop_condition.h"
#include "simulation/simulation.h"
#include "simulation/outputs.h"

using json = nlohmann::json;

/*
 * This file defines all functions which will be called from outside the cpp library.
 */
extern "C" {
    /* This function takes as inputs :
     * - model : a json with the model description
     * - settings : a json formed the following way {"settings": {"simulation": {...}, "stop_condition": {...}}}
     * It returns the json with the outputs of the simulation.
     * We have to use char* (and not string) in the interface because string is not recognized in python.
     */
    const char* simulate(char* model_char, char* settings_char) {
        string model_str = model_char;
        string settings_str = settings_char;
        // Parse the model
        json model_json = json::parse(model_str);
        Model* model = Model::fromJson(&model_json);
        // Parse the settings
        json settings_json = json::parse(settings_str);
        Settings* settings = Settings::fromJson(&settings_json["simulation"],model);
        StopCondition* stop_condition = StopCondition::fromJson(&settings_json["stop_condition"]);

        // TO-DO: decide how to do the pre-simulation (within simulation.simulate() ? or creation of two simulations ?)

        // Perform the simulation
        Simulation* simulation = new Simulation(model, settings, stop_condition);
        Outputs* simulation_outputs = simulation->simulate();
        // Build char* to return
        string output = simulation_outputs->toJson().dump();
        char* ret = new char[output.size()];
        strcpy(ret, output.c_str());
        return ret;
    }
}
