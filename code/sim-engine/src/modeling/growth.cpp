#include "growth.h"

// Exponential growth
void ExpGrowth::grow(Cell *cell, double deltaT) {
    double factor = exp( deltaT * cell->getGrowthFormula()->eval(cell) );
    cell->setVolume( cell->getVolume()*factor );
}

// The growth is derived from the formula
void FormulaGrowth::grow(Cell *cell, double deltaT) {
    (void) deltaT;
    cell->setVolume(cell->getGrowthFormula()->eval(cell));
}
