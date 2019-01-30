
#include "modeling/model.h"
#include "simulation/lineage.h"
#include "simulation/simulation.h"
#include "simulation/generator.h"
#include "simulation/stop_condition.h"
#include "simulation/settings.h"
#include "parsing/utils.h"
#include "array"
#include <climits>

class SimulationTests {
public:

    /* Random generator */
    static void GenTest() {
        cout << "\n---Test the random generator.---" << endl;
        cout << "U(0, 1) :" << endl;
        cout << Generator::unif() << " " << Generator::unif() << " " << Generator::unif() << endl;
        cout << "U(-10, -5) :" << endl;
        cout << Generator::unif(-10, -5) << " " << Generator::unif(-10, -5) << " " << Generator::unif(-10, -5) << endl;
        cout << "N(4, 2) :" << endl;
        cout << Generator::norm(4, 2) << " " << Generator::norm(4, 2) << " " << Generator::norm(4, 2) << endl;
        cout << "Npos(3, 1) / Npos(-1, 1) / Npos(-100, 1) :" << endl;
        cout << Generator::norm_pos(3, 1) << " " << Generator::norm_pos(-1, 1) << " " << Generator::norm_pos(-100, 1) << endl;
        cout << "LN(0, 1) :" << endl;
        cout << Generator::log_norm(0, 1) << " " << Generator::log_norm(0, 1) << " " << Generator::log_norm(0, 1) << endl;
        cout << "NB(0, 0) :" << endl;
        cout << Generator::negative_binomial(5, 0.5) << " " << Generator::negative_binomial(5, 0.5) << " " << Generator::negative_binomial(5, 0.5) << endl;
    }

    /* Build a cell factory for these tests */
    static CellFactory* buildCellFactory() {
        return new CellFactory(new Const(0.05), new Greater(new Volume(), new Const(3)), new Const(0.7));
    }

    /* Test of the lineage class */
    static void LineageTest() {
        cout << "\n---Test the lineage.cpp.---" << endl;
        CellFactory* cell_factory = buildCellFactory();
        vector<int> generations = {3, 2};
        vector<Cell*> cells = {cell_factory->buildCell(3), cell_factory->buildCell(3)};
        Lineage* lineage = new Lineage(1, generations, cells);
        cout << "Lineage : " + lineage->toJson().dump() << endl;
    }

    /* Test the outputs class */
    static void OutputsTest() {
        cout << "\n---Test outputs.cpp.---" << endl;

        /* Build manually some outputs and displays them. */
        Outputs* outputs = new Outputs();
        outputs->getCellCycles().push_back({1, 2, 3});
        outputs->getCellCycles().push_back({4, 5});
        outputs->getCellCycles()[0].push_back(8);
        cout << "Outputs : " + outputs->toJson().dump() << endl;

        /* Save some lineages in outputs */
        outputs = new Outputs();
        CellFactory* cell_factory = buildCellFactory();
        vector<int> g0 = {0};
        vector<int> g1 = {0};
        vector<Cell*> c0 = {cell_factory->buildCell(3)};
        vector<Cell*> c1 = {cell_factory->buildCell(3)};
        Lineage* l0 = new Lineage(0, g0, c0);
        Lineage* l1 = new Lineage(1, g1, c1);
        outputs->getLineages().push_back({0});
        outputs->getLineages().push_back({1});
        outputs->getCellCycles().push_back({});
        outputs->getCellCycles().push_back({});
        outputs->saveLineage(l0, 0);
        outputs->saveLineage(l0, 1);
        outputs->saveLineage(l1, 1);
        outputs->saveLineage(l1, 2);
        outputs->saveLineage(l0, 3);
        cout << "Outputs : " + outputs->toJson().dump() << endl;
    }

    /* Test settings */
    static void SettingsTest() {
        cout << "\n---Test settings.cpp.---" << endl;
        // Read the settings in the test file
        json settings_json = Utils::parse_json("settings.json");
        json simulation_settings_json = settings_json["simulation"];
        // Read the model
        json model_json = Utils::parse_json("model.json");
        Model* model = Model::fromJson(&model_json);
        Settings* settings = Settings::fromJson(&simulation_settings_json,model);
        // Display the settings
        cout << "Settings : " + settings->toJson().dump() << endl;
    }

    /* Test stop condition */
    static void StopConditionTest() {
        cout << "\n---Test stop_condition.cpp.---" << endl;
        // Read the stop conditions
        json settings_json = Utils::parse_json("settings.json");
        json stop_condition_json = settings_json["stop_condition"];
        StopCondition* stop_condition = StopCondition::fromJson(&stop_condition_json);
        cout << "Stop condition : " + stop_condition->toJson().dump() << endl;
    }

    /* Test a full simulation */
    static void Simulate() {
        cout << "\n---Tests of the simulation.---" << endl;

        // Read the model
        json model_json = Utils::parse_json("model.json");
        Model* model = Model::fromJson(&model_json);
        cout << model->toJson().dump() << endl;
        // Read the settings
        json settings_json = Utils::parse_json("settings.json");
        json simulation_settings_json = settings_json["simulation"];
        Settings* settings = Settings::fromJson(&simulation_settings_json,model);
        json stop_condition_json = settings_json["stop_condition"];
        StopCondition* stop_condition = StopCondition::fromJson(&stop_condition_json);

        // Simulation
        Simulation* simulation = new Simulation(model, settings, stop_condition);
        Outputs* outputs = simulation->simulate();
        cout << "Simulation outputs :" << endl;
        cout << outputs->toJson().dump() << endl;
        ofstream out("data.json");
        out << outputs->toJson().dump();
    }
};
