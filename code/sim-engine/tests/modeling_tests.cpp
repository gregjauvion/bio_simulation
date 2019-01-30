using namespace std;

#include "iostream"
#include "fstream"
#include "modeling/cell.h"
#include "modeling/molecule.h"
#include "modeling/reaction.h"
#include "modeling/division.h"
#include "modeling/growth.h"
#include "modeling/model.h"
#include "parsing/func.h"
#include "parsing/parser.h"
#include "parsing/utils.h"
#include "regex"

class ModelingTests {
public:
    /* Builds a molecule and displays it */
    static void MoleculeTest() {
        cout << "\n---Tests of molecule.cpp.---" << endl;
        Molecule* m1 = new Molecule(0, "Ab", SPLIT_EQUAL);
        Molecule* m2 = new Molecule(1, "Bc", SPLIT_SHARE_DEPENDENT);
        cout << "Molecule 1 : " + m1->toJson().dump() << endl;
        cout << "Molecule 2 : " + m2->toJson().dump() << endl;
    }

    /* Builds a reaction and displays it */
    static void ReactionTest() {
        cout << "\n---Tests of reaction.cpp.---" << endl;
        // Builds a reaction and displays it
        vector<Molecule*> left;
        vector<Molecule*> right;
        left.push_back(new Molecule(0, "Abo", SPLIT_SHARE_DEPENDENT));
        left.push_back(new Molecule(1, "De", SPLIT_SHARE_DEPENDENT));
        right.push_back(new Molecule(2, "HGd", SPLIT_SHARE_DEPENDENT));
        right.push_back(new Molecule(3, "Rez", SPLIT_SHARE_DEPENDENT));
        Reaction* reaction = new Reaction(left, right, new Const(3));
        cout << "Reaction : " + reaction->toJson().dump() << endl;
    }

    /* Builds a parameter and displays it */
    static void ParameterTest() {
        cout << "\n---Test of parameter.cpp.---" << endl;
        // Builds a parameter and displays it
        Parameter* mu = new Parameter("mu", 3.45);
        cout << "Parameter : " + mu->toJson().dump() << endl;
    }

    /* Builds a cell factory and returns it (the cell factory will be used in some other tests) */
    static CellFactory* buildCellFactory() {
        // Set the parameters
        Func<double>* volume_initialization = new Const(2);
        vector<Func<double>*> quantities_initialization = vector<Func<double>*>({new Const(2.5), new Const(3), new Const(1.5)});
        Func<double>* growth_formula = new Const(0.05);
        Func<bool>* division_trigger = new Greater(new Volume(), new Const(3));
        Func<double>* volume_share = new Const(0.7);
        vector<bool> zero_during_burn(quantities_initialization.size(),false);
        // Return the cell factory
        return new CellFactory(growth_formula, division_trigger, volume_share);
    }

    /* Displays the cell factory */
    static void CellFactoryTest() {
        cout << "\n---Test of cell_factory.cpp.---" << endl;
        CellFactory* cell_factory = buildCellFactory();
        cout << "Cell factory : " + cell_factory->toJson().dump() << endl;
    }

    /* Build some cells using the cell factory */
    static void CellTest() {
        cout << "\n---Test of cell.cpp.---" << endl;
        CellFactory* cell_factory = buildCellFactory();
        // Build cells using initialization method (without arguments)
        cout << "Init cell 1 : " + cell_factory->buildCell(3)->toJson().dump() << endl;
        cout << "Init cell 2 : " + cell_factory->buildCell(3)->toJson().dump() << endl;
        // Build some cells with arguments
        vector<double> q = vector<double>({4, 3, 2});
        cout << "Cell 1 : " + cell_factory->buildCell(1, q)->toJson().dump() << endl;
    }

    /* Test growth */
    static void GrowthTest() {
        cout << "\n---Test of growth.cpp.---" << endl;
        CellFactory* cell_factory = buildCellFactory();
        // Test exp. growth
        cout << "Test exponential growth :" << endl;
        double v1,v2,v3;
        Growth* growth = new ExpGrowth();
        Cell* cell = cell_factory->buildCell(3);
        v1 = cell->getVolume();
        growth->grow(cell, 1);
        v2 = cell->getVolume();
        growth->grow(cell, 2);
        v3 = cell->getVolume();
        cout << "Volume at times 0, 1 and 3 : " + to_string(v1) + " " + to_string(v2) + " " + to_string(v3) << endl;
        // Test formula growth
        cout << "\nTest formula growth :" << endl;
        growth = new FormulaGrowth();
        cell = cell_factory->buildCell(3);
        cell->setGrowthFormula(new Const(10));
        growth->grow(cell, 10);
        cout << "Volume after formula growth : " + to_string(cell->getVolume()) << endl;
    }

    /* Test division */
    static void DivisionTest() {
        cout << "\n---Test of division.cpp---" << endl;
        CellFactory* cell_factory = buildCellFactory();
        Cell* cell = cell_factory->buildCell(3);
        Division* division = new Division();
        // Build some molecules
        vector<Molecule*> mols;
        mols.push_back(new Molecule(0, "", SPLIT_SHARE_DEPENDENT));
        mols.push_back(new Molecule(1, "", SPLIT_EQUAL));
        mols.push_back(new Molecule(2, "", SPLIT_SHARE_DEPENDENT));
        // Test the division
        cout << "Cell : " + cell->toJson().dump() << endl;
        cout << "isDivision() : " + to_string(division->isDivision(cell)) << endl;
        cell->setVolume(4);
        cout << "isDivision() : " + to_string(division->isDivision(cell)) << endl;
        vector<Cell*> div = division->division(cell, cell_factory, mols, false, false);
        cout << "Cells after division : " << endl;;
        for (unsigned long i=0; i<div.size(); i++)
            cout << div[i]->toJson().dump() << endl;
    }

    /* Test a model */
    static void ModelTest() {
        cout << "\n---Test of model.cpp---" << endl;
        json model_json = Utils::parse_json("repressilator.json");
        Model* model = Model::fromJson(&model_json);
        cout << "Model : " + model->toJson().dump() << endl;
    }
};
