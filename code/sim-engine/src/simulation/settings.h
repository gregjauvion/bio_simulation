#ifndef SETTINGS_H
#define SETTINGS_H

#include "vector"
#include "json.h"
#include "modeling/model.h"
#include "parsing/func.h"

using namespace std;
using json = nlohmann::json;

class Settings
{
public:
    Settings(int nb_lineages_, int nb_lineages_sample_, bool mother_machine_, bool stochastic_,
             Func<double>* volume_initialization_, vector<Func<double>*>& quantities_initialization_,
             double burn_duration_, vector<bool>& zero_during_burn_, vector<bool>& is_quantity_init_conc_) :
        nb_lineages(nb_lineages_), nb_lineages_sample(nb_lineages_sample_), mother_machine(mother_machine_), stochastic(stochastic_),
        volume_initialization(volume_initialization_), quantities_initialization(quantities_initialization_),
        burn_duration(burn_duration_), zero_during_burn(zero_during_burn_), is_quantity_init_conc(is_quantity_init_conc_)
    {}
    /* Getters */
    int getNbLineages() { return nb_lineages; }
    int getNbLineagesSample() { return nb_lineages_sample; }
    bool getMotherMachine() { return mother_machine; }
    bool getStochastic() { return stochastic; }
    Func<double>* getVolumeInitialization() { return volume_initialization; }
    vector<Func<double>*>& getQuantitiesInitialization() { return quantities_initialization; }
    double getBurnDuration() { return burn_duration; }
    vector<bool>& getZeroDuringBurn() { return zero_during_burn; }
    vector<bool>& getIsQuantityInitConc() { return is_quantity_init_conc; }
    /* Displays as a string and reads from a json */
    json toJson();
    static Settings* fromJson(json* settings,Model* model);

private:
    int nb_lineages;
    int nb_lineages_sample;
    bool mother_machine;
    bool stochastic;
    Func<double>* volume_initialization;
    vector<Func<double>*> quantities_initialization;
    double burn_duration;
    vector<bool> zero_during_burn;
    vector<bool> is_quantity_init_conc;
};

#endif // SETTINGS_H
