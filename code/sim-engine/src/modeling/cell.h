#ifndef CELL_H
#define CELL_H

#include "parsing/evaluable.h"
#include "parsing/func.h"
#include "string"
#include "sstream"
#include "json.h"

using json = nlohmann::json;

/*
 * A Cell object defines a cell. It can be evaluated by a Func object.
 * generation is used to get the generation index of the cell (incremented after each division)
 */
class Cell : public Evaluable
{
public:
    /* The constructor does not take informations related to growth and division, which are set in the cell factory. */
    Cell(int id_, double volume_, vector<double>& quantities_):
        id(id_), volume_birth(volume_), volume(volume_), quantities(quantities_) {}
    /* Getters */
    int getId() { return id; }
    double getVolumeBirth() { return volume_birth; }
    double getVolume() { return volume; }
    vector<double>& getQuantities() { return quantities; }
    Func<double>* getGrowthFormula() { return growth_formula; }
    Func<bool>* getDivisionTrigger() { return division_trigger; }
    Func<double>* getVolumeShare() { return volume_share; }
    /* Setters */
    void setGrowthFormula(Func<double>* growth_formula_) { growth_formula = growth_formula_; }
    void setDivisionTrigger(Func<bool>* division_trigger_) { division_trigger = division_trigger_; }
    void setVolumeShare(Func<double>* volume_share_) { volume_share = volume_share_; }
    void setVolume(double volume_) { volume = volume_; }
    /* Displays a cell */
    json toJson();

private:
    int id;
    double volume_birth;
    double volume;
    vector<double> quantities;
    // This expression models the growth of the cell : it is either a growth rate, either a formula on the cell variables (depending on the growth type)
    Func<double>* growth_formula;
    Func<bool>* division_trigger;
    Func<double>* volume_share;
};

#endif // CELL_H
