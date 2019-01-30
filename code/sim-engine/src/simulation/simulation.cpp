#include "simulation.h"

/* Constructor */
Simulation::Simulation(Model* model_, Settings* settings_, StopCondition* stop_condition_) {
    model = model_;
    settings = settings_;
    stop_condition = stop_condition_;
    // Build the cell factory
    cell_factory = new CellFactory(model->getGrowthRate(), model->getDivisionTrigger(), model->getVolumeShare());
    // Initialize the lineages with one cell only
    for (int i=0; i<settings->getNbLineages(); i++) {
        vector<int> generations = {0};
        // Build a cell and set its volume
        Cell* cell = cell_factory->buildCell(model->getMolecules().size());
        cell->setVolume(settings->getVolumeInitialization()->eval(cell));
        // Set the quantities depending on burning period or not
        bool burn_period = settings->getBurnDuration() > 0;
        for (unsigned int i=0; i<settings->getQuantitiesInitialization().size(); i++) {
            if (burn_period && settings->getZeroDuringBurn()[i]) {
                cell->getQuantities()[i] = 0;
            } else {
                double init = settings->getQuantitiesInitialization()[i]->eval(cell);
                if (settings->getIsQuantityInitConc()[i]) init = init * cell->getVolume();
                if (settings->getStochastic()) init = round(init);
                cell->getQuantities()[i] = init;
            }
        }
        vector<Cell*> cells = {cell};
        lineages.push_back(new Lineage(i, generations, cells));
    }
}

/* Run the simulation until the stopping condition is met, and return the outputs. */
Outputs* Simulation::simulate() {
    // Initialize the outputs
    Outputs* outputs = new Outputs();
    for (auto lineage : lineages) {
        vector<int> l;
        for (auto cell : lineage->getCells()) {
            vector<int> c;
            outputs->getCellCycles().push_back(c);
            l.push_back(cell->getId());
        }
        outputs->getLineages().push_back(l);
    }
    // Set a boolean if this is the burning period
    bool burn_period = settings->getBurnDuration() > 0;
    // Run the simulation
    double time = 0;
    int nb_time_steps = 0;
    while (!stop_condition->stop(time, nb_time_steps, lineages)) {
        // Save the lineages (if not burning period)
        if (!burn_period)
            for (auto lineage : lineages)
                outputs->saveLineage(lineage, time);
        // Simulate a timestep
        double time_step = chooseTimestep();
        for (auto lineage : lineages)
            simulateTimestep(time_step, time, lineage, outputs);
        time += time_step;
        nb_time_steps++;
        // If this is the burning period, reset some quantities to 0
        for (auto lineage : lineages)
            for (auto cell : lineage->getCells())
                for (int i=0; i<model->getMolecules().size(); i++)
                    if (settings->getZeroDuringBurn()[i])
                        cell->getQuantities()[i] = 0;
        // Update burn_period boolean
        if (burn_period && time > settings->getBurnDuration()) {
            burn_period = false;
            // At the end of the burning period, initialize quantities for molecules zero_during_burn
            for (auto lineage : lineages) {
                for (auto cell : lineage->getCells()) {
                    for (int i=0; i<model->getMolecules().size(); i++) {
                        if (settings->getZeroDuringBurn()[i]) {
                            double init = settings->getQuantitiesInitialization()[i]->eval(cell);
                            if (settings->getIsQuantityInitConc()[i]) init = init * cell->getVolume();
                            if (settings->getStochastic()) init = round(init);
                            cell->getQuantities()[i] = init;
                        }
                    }
                }
            }
        }
    }
    return outputs;
}

/* Simulation of the evolution of the system during deltaT. The different steps are, for each cell in the population :
 - Evolution of the concentrations of the molecules in the cell
 - Growth of the cell
 - Division of the cell
*/
void Simulation::simulateTimestep(double time_step, double time, Lineage* lineage, Outputs* outputs) {
    /* These vectors will contain the new generations and cells for this lineage */
    vector<int> new_generations;
    vector<Cell*> new_cells;
    /* Loop on the cells of the lineage and make them grow. */
    vector<Cell*> cells = lineage->getCells();
    for (int i=0; i<cells.size(); i++)
    {
        int generation = lineage->getGenerations()[i];
        Cell* cell = cells[i];
        /* Reactions (evolution of the quantities of the molecules) */
        if (settings->getStochastic())
            simulateCellReacStoch(cell, time_step);
        else
            simulateCellReacDet(cell, time_step);
        /* Growth of the cell */
        model->getGrowth()->grow(cell, time_step);
        /* Division of the cell */
        if (model->getDivision()->isDivision(cell)) {
            // save the state just before division
            //outputs->saveCellState(cell, time+time_step);
            // get the daughter cells states
            vector<Cell*> cells = model->getDivision()->division(cell, cell_factory, model->getMolecules(), settings->getStochastic(), settings->getMotherMachine());
            // Update the new cells
            new_generations.push_back(generation+1);
            new_cells.push_back(cells[0]);
            outputs->getLineages()[lineage->getId()].push_back(cells[0]->getId());
            vector<int> cell_cycle;
            outputs->getCellCycles().push_back(cell_cycle);
            if (!settings->getMotherMachine()) {
                new_generations.push_back(generation+1);
                new_cells.push_back(cells[1]);
                outputs->getLineages()[lineage->getId()].push_back(cells[1]->getId());
                vector<int> cell_cycle_2;
                outputs->getCellCycles().push_back(cell_cycle_2);
            }
        } else {
            new_generations.push_back(generation);
            new_cells.push_back(cell);
        }
    }
    /* Update the lineage cells */
    lineage->setGenerations(new_generations);
    lineage->setCells(new_cells);
}

/*
 * Cell reactions: stochastic version
 */
void Simulation::simulateCellReacStoch(Cell* cell, double time_step)
{
    /* Gillespie algorithm (basic implementation) */
    double t_done = 0;
    // Evaluate the reaction rates. Rates for reactions with one reactant with q=0 are 0
    double sum_rates = 0;
    double* rates = new double[model->getReactions().size()];
    for (unsigned int i=0; i< model->getReactions().size(); i++) {
        rates[i] = model->getReactions()[i]->getRate()->eval(cell);
        // Set the rate to 0 if one of the reactants has q=0
        for (auto molecule : model->getReactions()[i]->getLeft())
            if (cell->getQuantities()[molecule->getIndex()]==0)
                rates[i] = 0;
        sum_rates += rates[i];
    }
    if (sum_rates==0) return;
    // Do reactions until t_done > deltaT
    while (t_done <= time_step) {
        // Sample next reaction time
        t_done += -log (Generator::unif()) / sum_rates;
        if (t_done <= time_step) {
            // Choose reaction
            double u = Generator::unif();
            double cum_rates = 0;
            int r = 0;
            while (cum_rates < u * sum_rates)
                cum_rates += rates[r++];
            Reaction* reaction = model->getReactions()[max(r-1, 0)];
            // Apply reaction
            for (auto molecule : reaction->getRight())
                cell->getQuantities()[molecule->getIndex()] += 1;
            for (auto molecule : reaction->getLeft())
                cell->getQuantities()[molecule->getIndex()] -= 1;
        }
        // Recompute rates
        sum_rates = 0;
        for (unsigned int i=0; i< model->getReactions().size(); i++)
        {
            rates[i] = model->getReactions()[i]->getRate()->eval(cell);
            // Set the rate to 0 if one of the reactants has q=0
            for (auto molecule : model->getReactions()[i]->getLeft())
                if (cell->getQuantities()[molecule->getIndex()]==0)
                    rates[i] = 0;
            sum_rates += rates[i];
        }
        if (sum_rates==0) return;
    }
}

/*
 * Cell reactions: deterministic version (first order Euler scheme)
 */
void Simulation::simulateCellReacDet(Cell* cell, double time_step)
{
    // The rate function gives the number of reactions per unit time, the reaction left/right molecules encode the stoechiometry
    for (auto reaction : model->getReactions())
    {
        double rate = reaction->getRate()->eval(cell);
        // Lower the rate if one of the reactants quantity goes below 0
        for (auto molecule : reaction->getLeft())
            rate = min(rate, cell->getQuantities()[molecule->getIndex()]/time_step);
        // Simulate the reaction
        for (auto molecule : reaction->getRight())
            cell->getQuantities()[molecule->getIndex()] += time_step * rate;
        for (auto molecule : reaction->getLeft())
            cell->getQuantities()[molecule->getIndex()] -= time_step * rate;
    }
}


/*
 * Choice of the timestep based on how fast cells in the population grow.
 */
double Simulation::chooseTimestep()
{
    double max_growth_rate = 0.;
    // iterate on cells to find the max growth rate
    for (auto lineage : lineages)
    {
        for (auto cell : lineage->getCells()) {
            double growth_rate = cell->getGrowthFormula()->eval(cell);
            if (growth_rate>max_growth_rate) max_growth_rate = growth_rate;
        }
    }
    return 0.01/max_growth_rate;
}
