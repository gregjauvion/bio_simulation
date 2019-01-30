#include "parameter.h"
#include "parsing/utils.h"

Parameter::Parameter(string name_, double value_) : name(name_) , value(value_) {}

/* Getters */
string Parameter::getName() { return name; }
double Parameter::getValue() { return value; }

/* Displays a string : name=value from a Parameter */
string Parameter::toString() {
    return name + " = " + to_string(value);
}
