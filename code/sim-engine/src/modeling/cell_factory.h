#ifndef CELL_FACTORY_H
#define CELL_FACTORY_H

#include "parsing/func.h"
#include "modeling/cell.h"
#include "json.h"

using json = nlohmann::json;

/*
 * A CellFactory is in charge of building cells and set their growth rate, division trigger and volume share.
 * - The method buildCell(int size) returns a cell with volume and quantities (of size size) equal to 0
 * - The method buildCell(double volume, vector<double> q) sets the volume and quantities
 */
class CellFactory {
public:
    CellFactory(Func<double>* growth_formula_, Func<bool>* division_trigger_, Func<double>* volume_share_) :
        cell_id(0),
        growth_formula(growth_formula_), division_trigger(division_trigger_), volume_share(volume_share_) {}
    Cell* buildCell(int size);
    Cell* buildCell(double volume_, vector<double> &quantities_);
    /* Serialization */
    json toJson();
private:
    // The id of the cell created by this cell factory
    int cell_id;
    Func<double>* growth_formula;
    Func<bool>* division_trigger;
    Func<double>* volume_share;
};

#endif // CELL_FACTORY_H
