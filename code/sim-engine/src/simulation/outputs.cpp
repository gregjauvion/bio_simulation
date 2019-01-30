#include "outputs.h"


void Outputs::saveLineage(Lineage* lineage, double time)
{
    for (auto cell : lineage->getCells()) {
        vector<double> cell_state = {time, cell->getVolume()};
        for (auto quantity : cell->getQuantities())
            cell_state.push_back(quantity);
        cell_states.push_back(cell_state);
        // add the corresponding index to the map for individual cell cycles
        cell_cycles[cell->getId()].push_back(cell_states.size()-1);
    }
}

json Outputs::toJson()
{
    /* made of four parts:
     * the structure for the data (i.e. the column names)
     * the data per se, with "," separation inside a row, ";" btw rows
     * the map of cell cycles
     * the map of the lineages
    */
    json outputs;

    // fill the structure part
    vector<string> structure = {"t", "V"};
    if (cell_states.size()>0) {
        for (unsigned int i=0;i<cell_states[0].size()-2;i++) // a bit ugly :(
        {
            ostringstream q_str;
            q_str << "q_" << i;
            structure.push_back(q_str.str());
        }
    }
    outputs["structure"] = structure;

    // the data part
    outputs["data"] =  cell_states;

    // the cell cycle map
    outputs["cell_cycles"] =  cell_cycles;

    // the lineage map
    outputs["lineages"] = lineages;

    return outputs;
}
