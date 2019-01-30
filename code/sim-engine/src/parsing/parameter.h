#ifndef PARAMETER_H
#define PARAMETER_H

#include "string"
#include "regex"
#include "json.h"

using namespace std;
using json = nlohmann::json;

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
    /* Serialization / deserialization */
    json toJson();
private:
    string name;
    double value;
};

#endif // PARAMETER_H
