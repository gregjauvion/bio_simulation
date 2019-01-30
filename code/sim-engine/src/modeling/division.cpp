#include "division.h"
#include "simulation/generator.h"

double Division::MIN_VOLUME_SHARE = 0.01;

/* Division functions */
bool Division::isDivision(Cell *cell) {
    return cell->getDivisionTrigger()->eval(cell);
}

vector<Cell*> Division::division(Cell *cell, CellFactory* cell_factory, vector<Molecule*> molecules, bool stochastic, bool mother_machine)
{
    // Get the volume share of the left cell
    double share = cell->getVolumeShare()->eval(cell);
    if (share<Division::MIN_VOLUME_SHARE) share = Division::MIN_VOLUME_SHARE;
    if (share>1-Division::MIN_VOLUME_SHARE) share = 1-Division::MIN_VOLUME_SHARE;
    // Create the quantities arrays
    int size = cell->getQuantities().size();
    vector<double> concLeft;
    vector<double> concRight;
    for (int i=0; i<size; i++) {
        SplitType split = molecules[i]->getSplitType();

        // Compute the shares of quantities in both cells
        double shareLeft = 0;
        double shareRight = 0;
        if (split==SPLIT_EQUAL) {
            shareLeft = 0.5;
            shareRight = 0.5;
        } else if (split==SPLIT_SHARE_DEPENDENT) {
            shareLeft = share;
            shareRight = 1 - share;
        } else if (split==SPLIT_IDENTICAL) {
            shareLeft = 1;
            shareRight = 1;
        }

        // Set the quantities in the daughter cells
        if (!stochastic) {
            concLeft.push_back(cell->getQuantities()[i] * shareLeft);
            if (!mother_machine) concRight.push_back(cell->getQuantities()[i] * shareRight);
        } else {
            // Stochastic simulation
            if (split==SPLIT_EQUAL) {
                // Split equally all molecules, and randomly the remaining molecule if it exists
                concLeft.push_back(floor(cell->getQuantities()[i] * shareLeft));
                if (!mother_machine) concRight.push_back(floor(cell->getQuantities()[i] * shareRight));
                // Take into account the case where odd number to share
                if (floor(cell->getQuantities()[i] * shareLeft) != cell->getQuantities()[i] * shareLeft) {
                    if (Generator::unif()<0.5)
                        concLeft[i]+=1.;
                    else
                        if (!mother_machine)
                             concRight[i]+=1.;
                }
            } else if (split==SPLIT_SHARE_DEPENDENT) {
                // Split each molecule randomly
                concLeft.push_back(0.);
                if (!mother_machine) concRight.push_back(0.);
                for (int j=0; j<cell->getQuantities()[i]; j++) {
                    if (Generator::unif()<shareLeft)
                        concLeft[i]+=1.;
                    else
                        if (!mother_machine)
                            concRight[i]+=1.;
                }
            } else if (split==SPLIT_IDENTICAL) {
                // The quantities are unchanged
                concLeft.push_back(cell->getQuantities()[i] * shareLeft);
                concRight.push_back(cell->getQuantities()[i] * shareRight);
            }
        }
    }
    // Create the cells
    vector<Cell*> ret;
    ret.push_back(cell_factory->buildCell(cell->getVolume()*share, concLeft));
    if (!mother_machine) ret.push_back(cell_factory->buildCell(cell->getVolume()*(1-share), concRight));
    return ret;
}
