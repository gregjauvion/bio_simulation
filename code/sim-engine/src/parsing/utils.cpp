#include "utils.h"

// TODO this function loops until infinity if s1==s2, should find a way to avoid this.
string Utils::replace(string str, string s1, string s2) {
    size_t found = str.find(s1);
    while (found!=string::npos) {
        str.replace(found, s1.length(), s2);
        found = str.find(s1);
    }
    return str;
}

vector<string> Utils::split(string str, string delim) {
    // The list which will contain all sub-strings.
    vector<string> ret;
    size_t index = 0;
    size_t found = str.find(delim);
    while (found!=string::npos) {
        // Add the sub-string in the list
        ret.push_back(str.substr(index, found));
        index += found + delim.length();
        found = str.substr(index).find(delim);
    }
    // Add the last part
    ret.push_back(str.substr(index, str.length()-index));
    return ret;
}

nlohmann::json Utils::parse_json(string file_name) {
    // Reads the file
    ifstream file;
    file.open(file_name);
    string str;
    getline(file, str);
    return nlohmann::json::parse(str);
}
