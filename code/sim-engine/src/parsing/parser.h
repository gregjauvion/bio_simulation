#ifndef PARSER_H
#define PARSER_H

#include "func.h"
#include "modeling/molecule.h"
#include "map"
#include "regex"
#include "iostream"

/*
 * This class defines a parser of algebric expressions of the form : "1 / (1+ mu * [C]/2)".
 */
class Parser {
public:
    Parser(vector<Molecule*>& molecules_, vector<Parameter*>& parameters_) :
        molecules(molecules_), parameters(parameters_) {}
    /* Getters */
    vector<Molecule*>& getMolecules() { return molecules; }
    vector<Parameter*>& getParameters() { return parameters; }
    // Parse a string representing an expression returning double
    Func<double>* parseDouble(string func_str);
    // Parse a string representing an expression returning bool
    Func<bool>* parseBool(string func_str);
    /* Regexps used to parse the expressions. */
    static const regex RE_PARENTHESES;
    static const regex RE_REPLACED;
    static const regex RE_EXPRESSION_PLUS_MINUS;
    static const regex RE_EXPRESSION_TIMES_DIV_POWER;
    static const regex RE_RANDOM_DISTRIBUTION;
    static const regex RE_COMMA;
    static const regex RE_EXPRESSION_BOOL;
    // Returns the expression without parentheses where the parts in parentheses have been replaced by {i}, and the map giving the expression for each {i}.
    pair<string,map<string,string>> simplify(string func_str);
    // Builds an expression from a string, the map with the expressions corresponding to {i}.
    Func<double>* expression(string str, map<string,string> replacements);
private:
    // The list of molecules, needed to get read expressions like [A]
    vector<Molecule*> molecules;
    // The list of parameters needed to read expressions with parameters
    vector<Parameter*> parameters;
    // Define here the actions to perform when parsing str has failed
    static void error(string str);
};

#endif // PARSER_H
