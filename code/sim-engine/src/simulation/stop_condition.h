#ifndef STOP_CONDITION_H
#define STOP_CONDITION_H

#include "lineage.h"
#include "json.h"

using json = nlohmann::json;

/*
 * This class defines the condition to stop the simulation and return the outputs.
 */
class StopCondition
{
public:
    StopCondition(int simulation_time_max_, double time_max_, int nb_time_steps_max_, int nb_generations_max_) :
        simulation_time_begin(time(0)), simulation_time_max(simulation_time_max_),
        time_max(time_max_), nb_time_steps_max(nb_time_steps_max_), nb_generations_max(nb_generations_max_) {}
    /* Returns true if the stop condition is verified. */
    bool stop(double time, int nb_time_steps, vector<Lineage*> lineages);
    /* Serialization / deserialization */
    json toJson();
    static StopCondition* fromJson(json* json_ptr);
private:
    time_t simulation_time_begin; // Initialized at current timestamp
    int simulation_time_max; // Maximum simulation time in seconds
    double time_max; // Maximum time (this is the sum of delta_t on all timesteps)
    int nb_time_steps_max; // Maximum number of time steps
    int nb_generations_max; // Maximum number of generations
};

#endif // STOP_CONDITION_H
