#ifndef GROWTH_H
#define GROWTH_H

#include "cell.h"
#include "math.h"

/* This class defines the methods needed to make a cell grow. */
class Growth
{
public:
    /* Grow the cell during a time dt */
    virtual void grow(Cell* cell, double deltaT) = 0;
    /* Displays as a string */
    virtual string toString() = 0;
};

/* This class defines an exponential growth. The growth rate is an attribute of the cell. */
class ExpGrowth : public Growth
{
public:
    ExpGrowth() {}
    virtual void grow(Cell* cell, double deltaT);
    virtual string toString() { return "exp_growth"; }
};

/* This class is used if the growth is derived from the sum of the quantities in the cells. */
class FormulaGrowth : public Growth {
public:
    FormulaGrowth() {}
    virtual void grow(Cell* cell, double deltaT);
    virtual string toString() { return "formula_growth"; }
};

#endif // GROWTH_H
