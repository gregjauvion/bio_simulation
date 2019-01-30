#include "parameter.h"
#include "parsing/utils.h"

Parameter::Parameter(string name_, double value_) : name(name_) , value(value_) {}

/* Getters */
string Parameter::getName() { return name; }
double Parameter::getValue() { return value; }

/* Serialization */
json Parameter::toJson() {
    json parameter;
    parameter["name"] = name;
    parameter["value"] = value;
    return parameter;
}
