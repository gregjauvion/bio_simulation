#include "reaction.h"
#include "parsing/parser.h"

// Separator of the molecules in a reaction
const string Reaction::MOL_SEPARATOR = "+";

/* Displays the reaction as a string. */
json Reaction::toJson() {
    json reaction;
    json left_json;
    json right_json;
    for (auto elem : left)
        left_json.push_back(elem->getName());
    for (auto elem : right)
        right_json.push_back(elem->getName());
    reaction["left"] = left_json;
    reaction["right"] = right_json;
    reaction["rate"] = rate->toString();
    return reaction;
}
