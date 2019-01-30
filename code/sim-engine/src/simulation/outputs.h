#ifndef OUTPUTS_H
#define OUTPUTS_H


using namespace std ;

#include "vector"
#include "sstream"
#include "fstream"
#include "lineage.h"
#include "json.h"

using json = nlohmann::json;

/*
 * We store the outputs as a single vector of 'cell states' existing
 * at different times in the simulation
 *
 * Each row thus also gives the time at which this cell state existed,
 * a CellCycleID, a lineageID in addition to the cell state per se
*/

class Outputs
{
public:
    Outputs() {}
    /* Update the output with a lineage */
    void saveLineage(Lineage* lineage, double time);
    /* Getters */
    vector<vector<double>>& getCellStates() { return cell_states; }
    vector<vector<int>>& getCellCycles() { return cell_cycles; }
    vector<vector<int>>& getLineages() { return lineages; }
    /* Serialization */
    json toJson();
private:
    // the main data 'table'
    vector<vector<double>> cell_states;
    // auxiliary data serving as maps
    vector<vector<int>> cell_cycles; // for each cell cycle, all the cell states corresponding
    vector<vector<int>> lineages; // for each lineage, all the corresponding cell cycles
};

#endif // OUTPUTS_H
