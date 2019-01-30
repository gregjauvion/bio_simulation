#include "stop_condition.h"

bool StopCondition::stop(double time, int nb_time_steps, vector<Lineage*> lineages) {
    // Stops if the simulation time is too long
    double simulation_time = std::difftime(std::time(0),simulation_time_begin);
    if (simulation_time > simulation_time_max) return true;
    // Stops if the time or the number of time steps is too high
    if (time>=time_max) return true;
    if (nb_time_steps>=nb_time_steps_max) return true;
    // Get the maximum number of generations in the lineages
    int nb_generations = 0;
    for (auto l : lineages)
        for (auto g : l->getGenerations())
            if (g>nb_generations)
                nb_generations = g;
    if (nb_generations>=nb_generations_max) return true;
    return false;
}

/* Serialization / Deserialization */
json StopCondition::toJson() {
    json stop_condition;
    stop_condition["simulation_time"] = simulation_time_max;
    stop_condition["time"] = time_max;
    stop_condition["nb_timesteps"] = nb_time_steps_max;
    stop_condition["nb_generations"] = nb_generations_max;
    return stop_condition;
}
StopCondition* StopCondition::fromJson(json *json_ptr) {
    json stop_condition = *json_ptr;
    return new StopCondition(stop_condition["simulation_time"], stop_condition["time"], stop_condition["nb_timesteps"], stop_condition["nb_generations"]);
}
