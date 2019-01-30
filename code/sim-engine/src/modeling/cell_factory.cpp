#include "cell_factory.h"

Cell* CellFactory::buildCell(double volume_, vector<double> &quantities_) {
    Cell* cell = new Cell(cell_id++, volume_, quantities_);
    // In the growth and division parameters, we replace all random values by one of their realisations.
    cell->setGrowthFormula(growth_formula->drawRandom(cell));
    cell->setDivisionTrigger(division_trigger->drawRandom(cell));
    cell->setVolumeShare(volume_share->drawRandom(cell));
    return cell;
}

Cell* CellFactory::buildCell(int size) {
    // returns a cell with 0 volume and 0 molecules
    vector<double> quantities(size,0.0);
    return buildCell(0, quantities);
}

json CellFactory::toJson() {
    json out;
    out["cell_id"] = cell_id;
    out["growth_formula"] = growth_formula->toString();
    out["division_trigger"] = division_trigger->toString();
    out["volume_share"] = volume_share->toString();
    return out;
}
