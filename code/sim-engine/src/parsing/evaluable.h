#ifndef EVALUABLE_H
#define EVALUABLE_H

using namespace std;

#include "vector"

/*
 * Represents an object which can be evaluated by a Func object.
 * This enables to avoid the circular dependency Func <-> Cell.
 */
class Evaluable
{
public:
    virtual vector<double>& getQuantities() = 0;
    virtual double getVolume() = 0;
    virtual double getVolumeBirth() = 0;
};

#endif // EVALUABLE_H
