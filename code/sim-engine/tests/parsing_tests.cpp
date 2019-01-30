
using namespace std;
#include "iostream"
#include "parsing/func.h"
#include "parsing/parser.h"
#include "parsing/utils.h"

class ParsingTests {
public:
    /* Tests of the parsing utilities. */
    static void UtilsTest() {
        cout << "\n---\nTests of utils.cpp functions.\n---" << endl;

        // Test the split function
        string delim = "--";
        string to_split = "abc--def--fg--ass";
        vector<string> spl = Utils::split(to_split,delim);
        cout << "String to split : " + to_split << endl;
        for (auto e : spl)
            cout << e << endl;
        to_split = "--aa--bb--dezf--";
        spl = Utils::split(to_split, delim);
        cout << "String to split : " + to_split << endl;
        for (auto e : spl)
            cout << e << endl;

        // Test the replace function
        string to_replace = "A beautiful house is beautiful but not so beautiful.beautiful";
        string replaced = Utils::replace(to_replace, "beautiful", "ugly");
        cout << "String before replacement : " + to_replace << endl;
        cout << "String after replacement : " + replaced << endl;
        cout << "Original string : " + to_replace << endl;
    }

    /* Tests of the functions. */
    static void FuncTest() {
        cout << "\n---\nTests of func.cpp functionalities.\n---" << endl ;

        // Build all kinds of functions, print them and evaluate them
        vector<double> quantities{3.2,5.4};
        Cell* cell = new Cell(1, 10.2, quantities);
        Const* constant = new Const(2);
        Conc* conc = new Conc(1);
        Sum* sum = new Sum(constant, conc);
        Div* div = new Div(constant, sum);
        Normal* normal = new Normal(div, constant);
        NegativeBinomial* neg_binomial = new NegativeBinomial(conc, sum);

        cout << "Constant equal to 2 : " + constant->toString() + " ; " + to_string(constant->eval(cell)) << endl;
        cout << "Molecule 0 concentration : " + conc->toString() + " ; " + to_string(conc->eval(cell)) << endl;
        cout << "constant + concentration : " + sum->toString() + " ; " + to_string(sum->eval(cell)) << endl;
        cout << "constant / (constant + concentration) : " + div->toString() + " ; " + to_string(div->eval(cell)) << endl;
        cout << "N(div, constant) : " + normal->toString() + " ; " + to_string(normal->eval(cell)) << endl;
        cout << "NB(conc, sum) : " + neg_binomial->toString() + " ; " + to_string(neg_binomial->eval(cell)) << endl;
    }

    /* Tests of random functions */
    static void RandomTest() {
        cout << "\n---\nTests of the random functions.\n---" << endl ;

        vector<double> quantities{3.2,5.4};
        Cell* cell = new Cell(1, 10.2, quantities);
        /* Create a function with random components */
        Func<double>* f = new Sum(new Normal(new Const(0), new Const(1)), new LogNormal(new Const(2), new Const(3)));

        cout << "Expression : " + f->toString() << endl;
        cout << "3 evaluations : " + to_string(f->eval(cell)) + " / " + to_string(f->eval(cell)) + " / " + to_string(f->eval(cell)) << endl;
        cout << "3 drawRandom : " + f->drawRandom(cell)->toString() + " / " + f->drawRandom(cell)->toString() + " / " + f->drawRandom(cell)->toString() << endl;
    }

    /* Tests of the parser. */
    static void ParseTest() {
        cout << "\n---\nTests of parser.cpp functionalities.\n---" << endl ;

        // Build a parser
        vector<Molecule*> molecules;
        molecules.push_back(new Molecule(1, "M1", SPLIT_SHARE_DEPENDENT));
        molecules.push_back(new Molecule(2, "M2", SPLIT_SHARE_DEPENDENT));
        molecules.push_back(new Molecule(3, "M3", SPLIT_SHARE_DEPENDENT));
        vector<Parameter*> parameters;
        parameters.push_back(new Parameter("a", 4.5));
        parameters.push_back(new Parameter("c", 3.2));
        Parser* parser = new Parser(molecules, parameters);

        // Try to parse some expressions
        string expression = "N(a,c*V+2)^(1.5+(1+V*2.32/(a+c))/(V_birth+1/(3+4.23)))";
        cout << "Expression to parse : " + expression << endl;
        cout << "Result : " + parser->parseDouble(expression)->toString() << endl;

        expression = "(2/(3+[M1]+[M2]+V_birth))*(1+[M3]/(a+c))";
        cout << "Expression to parse : " + expression << endl;
        cout << "Result : " + parser->parseDouble(expression)->toString() << endl;

        expression = "1/ (LN(a,c) + 2^Npos([M1],[M2])) * (V_birth + V + N(1,3.45))";
        cout << "Expression to parse : " + expression << endl;
        cout << "Result : " + parser->parseDouble(expression)->toString() << endl;

        expression = "a * c + c * V + NB((a+c),3.4)";
        cout << "Expression to parse : " + expression << endl;
        cout << "Result : " + parser->parseDouble(expression)->toString() << endl;

        expression = "a * c + c * V + U((a+c),3.4) < V_birth + V + N(1,3.45)";
        cout << "Expression to parse : " + expression << endl;
        cout << "Result : " + parser->parseBool(expression)->toString() << endl;

        expression = "(2/(3+M1+[M2]+V_birth))*(1+M3+M2/(a+c))";
        cout << "Expression to parse : " + expression << endl;
        cout << "Result : " + parser->parseDouble(expression)->toString() << endl;
    }
};
