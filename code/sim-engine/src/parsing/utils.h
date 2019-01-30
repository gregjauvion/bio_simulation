#ifndef UTILS_H
#define UTILS_H

using namespace std;

#include "string"
#include "vector"
#include "fstream"
#include "json.h"

class Utils
{
public:
    /* Replace all occurrences of s1 by s2 in the string. */
    static string replace(string str, string s1, string s2);
    /* Split the string str constituted by strings separated by "delim" in a list of strings. */
    static vector<string> split(string str, string delim);
    /* Reads a file with a json and parses it */
    static nlohmann::json parse_json(string file_name);
};

#endif // UTILS_H
