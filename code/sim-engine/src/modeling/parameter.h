#ifndef PARAMETER_H
#define PARAMETER_H

#include "string"
#include "regex"

/*
 * This class represents a Parameter of the model whose value is defined by the user.
 */
class Parameter
{
public:
    Parameter(string name_,double value_);
    /* Getters */
    string getName();
    double getValue();
    /* Displays as string and reads from string */
    string toString();
    static Parameter* fromString(string str);
private:
    string name;
    double value;
};

#endif // PARAMETER_H
