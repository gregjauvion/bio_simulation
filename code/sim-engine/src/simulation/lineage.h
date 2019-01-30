#ifndef LINEAGE_H
#define LINEAGE_H

#include "modeling/cell.h"

using json = nlohmann::json;

/*
 * For the moment, a lineage is defined by a list of cells, and a list giving the generation of each cell in the lineage.
 * We could imagine a better tree-like structure to represent a lineage.
 */
class Lineage
{
public:
    Lineage(int id_, vector<int>& generations_, vector<Cell*>& cells_): id(id_), generations(generations_), cells(cells_) {}
    /* Getters */
    int getId() { return id; }
    vector<int>& getGenerations() { return generations; }
    vector<Cell*>& getCells() { return cells; }
    /* Setters */
    void setGenerations(vector<int>& generations_) { generations = generations_; }
    void setCells(vector<Cell*>& cells_) { cells = cells_; }
    /* Displays as a json */
    json toJson();
private:
    int id;
    // The current generation for each cel of the lineage
    vector<int> generations;
    vector<Cell*> cells;
};

#endif // LINEAGE_H
