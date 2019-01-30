#ifndef SIMULATION_H
#define SIMULATION_H

#include "lineage.h"
#include "settings.h"
#include "stop_condition.h"
#include "outputs.h"
#include "modeling/model.h"
#include "modeling/cell_factory.h"
#include "random.h"

class Simulation
{
public:
    /* The simulation is built with a model, some settings and conditions to stop the simulation. */
    Simulation (Model* model_, Settings* settings_, StopCondition* stop_condition_);
    /* Simulation methods */
    Outputs* simulate();
    void simulateTimestep(double delta_T, double time, Lineage* lineage, Outputs* outputs);
    void simulateCellReacStoch(Cell* cell,double delta_T);
    void simulateCellReacDet(Cell* cell,double delta_T);
    double chooseTimestep();
private:
    Model* model;
    Settings* settings;
    StopCondition* stop_condition;
    CellFactory* cell_factory;
    vector<Lineage*> lineages;
};

#endif // SIMULATION_H
