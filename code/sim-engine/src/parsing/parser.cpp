
#include "iostream"
#include "string.h"
#include "stdio.h"
#include "parser.h"
#include "func.h"
#include "utils.h"

// Expression inside parentheses
const regex Parser::RE_PARENTHESES = regex("(.*?)([(][^()]*[)]).*");
// Expression of the form {n} (replaced expression)
const regex Parser::RE_REPLACED = regex("^\\{[0-9]+\\}$");
// Sum/difference of formulas
const regex Parser::RE_EXPRESSION_PLUS_MINUS = regex("([^+-]+)([+-])(.*)");
// Product/division of formulas
const regex Parser::RE_EXPRESSION_TIMES_DIV_POWER = regex("([^/*^]+)([/*^])(.*)");
// Formula matching a random distribution : U{i}, N{i}, Npos{i}, LN{i}, NB{i} ({i} represents the parameters of the distribution)
const regex Parser::RE_RANDOM_DISTRIBUTION = regex("(U|N|Npos|LN|NB)(\\{[0-9]+\\})");
// Formula like f1,f2 where f1 and f2 are formulas
const regex Parser::RE_COMMA = regex("(.*?),(.*)");
// Boolean formula like f>g or f<g
const regex Parser::RE_EXPRESSION_BOOL = regex("([^><]+)([><])(.*)");

/* Takes an expression and returns :
 - a simple expression where parts inside parentheses have been replaced by a string like {i}
 - a map<string,string> giving the expression corresponding to each {i} */
pair<string,map<string,string>> Parser::simplify(string func_str) {
    smatch match;
    int index = 0;
    // The map which will contain the expression for each replacement of the form "{i}"
    map<string,string> replacements;
    // Replace all parentheses while there are still parentheses in the expression
    while (regex_search(func_str, match, Parser::RE_PARENTHESES)) {
        // match[2] corresponds to the expression in the parentheses with its parentheses
        string to_replace = match[2];
        string replace = "{" + to_string(index++) + "}";
        func_str.replace(func_str.find(to_replace, 0), to_replace.size(), replace);
        // We remove the parentheses when inserting the replaced string
        replacements.insert(pair<string,string>(replace, to_replace.substr(1, to_replace.size()-2)));
    }
    return pair<string,map<string,string>>(func_str, replacements);
}

/* Builds an expression from a string. */
Func<double>* Parser::expression(string str, map<string,string> replacements) {
    smatch match;
    if (regex_search(str, match, Parser::RE_REPLACED)) {
        // This is an expression of the form {n}
        if (replacements.find(str)!=replacements.end())
            return expression(replacements.find(str)->second, replacements);
        else
            // This case should not happen
            return new Const(0);
    } else if (regex_search(str, match, Parser::RE_EXPRESSION_PLUS_MINUS)) {
        // In this case we have an expression of the form (left) (+ -) (right) and we build the function recursively.
        // It is important to consider this case before RE_EXPRESSION_TIMES_DIV to take into account */ priority over +-.
        string left = match[1];
        string oper = match[2];
        string right = match[3];
        // Build the function
        if (strcmp(oper.c_str(), "+")==0) {
            return new Sum(expression(left,replacements), expression(right,replacements));
        } else if (strcmp(oper.c_str(), "-")==0) {
            return new Minus(expression(left,replacements), expression(right,replacements));
        }
    } else if (regex_search(str, match, Parser::RE_EXPRESSION_TIMES_DIV_POWER)) {
        // In this case we have an expression of the form (left) (* /) (right) and we build the function recursively
        string left = match[1];
        string oper = match[2];
        string right = match[3];
        // Build the function
        if (strcmp(oper.c_str(), "*")==0) {
            return new Prod(expression(left,replacements), expression(right,replacements));
        } else if (strcmp(oper.c_str(), "/")==0) {
            return new Div(expression(left,replacements), expression(right,replacements));
        } else if (strcmp(oper.c_str(), "^")==0) {
            return new Power(expression(left,replacements), expression(right,replacements));
        }
    } else if (regex_search(str, match, Parser::RE_RANDOM_DISTRIBUTION)) {
        // In this case we have a function of the form N{i}
        // Look for the replacement which should be of the form f1,f2
        string distribution_type = match[1];
        string parameters = match[2];
        if (replacements.find(parameters)!=replacements.end())
            parameters = replacements.find(parameters)->second;
        // Retrieve the two parameters of the distribution (for the moment all distributions have two parameters)
        string p1;
        string p2;
        if (regex_search(parameters, match, Parser::RE_COMMA)) {
            p1 = match[1];
            p2 = match[2];
        }
        // Return the distribution
        if (strcmp(distribution_type.c_str(), "U")==0) {
            return new Unif(expression(p1,replacements), expression(p2,replacements));
        } else if (strcmp(distribution_type.c_str(), "N")==0) {
            return new Normal(expression(p1,replacements), expression(p2,replacements));
        } else if (strcmp(distribution_type.c_str(), "LN")==0) {
            return new LogNormal(expression(p1,replacements), expression(p2,replacements));
        } else if (strcmp(distribution_type.c_str(), "Npos")==0) {
            return new NormalPositive(expression(p1,replacements), expression(p2,replacements));
        } else if (strcmp(distribution_type.c_str(), "NB")==0) {
            return new NegativeBinomial(expression(p1,replacements), expression(p2,replacements));
        }
    } else {
        // Check if it is a volume function
        if (str.compare("V")==0)
            return new Volume();
        // Check if it is a volume at birth function
        if (str.compare("V_birth")==0)
            return new VolumeBirth();
        // Check if it is a parameter
        for (auto p : parameters)
            if (str.compare(p->getName())==0)
                return new Param(p);
        // Check if it is the concentration of a molecule
        for (auto m : molecules) {
            string to_compare = "[" + m->getName() + "]";
            if (str.compare(to_compare)==0)
                return new Conc(m->getIndex());
        }
        // Check if it is the absolute number of a molecule
        for (auto m : molecules) {
            if (str.compare(m->getName())==0)
                return new Abs(m->getIndex());
        }
        // Otherwise it is a double (we should check that the parsing will be OK)
        return new Const(stod(str));
    }
    error(str);
}

/* Parse the string and returns the corresponding function */
Func<double>* Parser::parseDouble(string str) {
    // Remove all blanks in the string
    string str_replaced = Utils::replace(str, " ", "");
    pair<string,map<string,string>> simplified = simplify(str_replaced);
    return expression(simplified.first, simplified.second);
}
Func<bool>* Parser::parseBool(string str) {
    // Remove all blanks in the string
    string str_replaced = Utils::replace(str, " ", "");
    pair<string,map<string,string>> simplified = simplify(str_replaced);
    string to_parse = simplified.first;
    map<string,string> replacements = simplified.second;
    // Builds the bool expression
    smatch match;
    if (regex_search(to_parse, match, Parser::RE_EXPRESSION_BOOL)) {
        string left = match[1];
        string oper = match[2];
        string right = match[3];
        // Build the function
        if (strcmp(oper.c_str(), "<")==0) {
            return new Lower(expression(left, replacements), expression(right, replacements));
        } else if (strcmp(oper.c_str(), ">")==0) {
            return new Greater(expression(left, replacements), expression(right, replacements));
        }
    }
    error(str);
}

/* If a string can not be parsed by the parser, throw an error. */
void Parser::error(string str) {
    stringstream ss;
    ss << str << " can not be parsed.";
    throw invalid_argument(ss.str());
}
