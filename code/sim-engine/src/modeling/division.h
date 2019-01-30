#ifndef DIVISION_H
#define DIVISION_H

#include "cell.h"
#include "cell_factory.h"
#include "molecule.h"

#include "simulation/random.h"

/*
 * A Division object defines a cellular division.
 */
class Division
{
public:
    Division() {}
    /* The division functions. */
    // Returns true if the cell should be divided
    bool isDivision(Cell* cell);
    // Returns the vector of the two cells after division
    vector<Cell*> division(Cell* cell, CellFactory* cell_factory, vector<Molecule*> molecules, bool stochastic, bool mother_machine);
    /* Displays a Division */
    string toString() { return "division"; }
    // The minimum value of the volume share
    static double MIN_VOLUME_SHARE;
};

#endif // DIVISION_H
