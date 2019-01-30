
#include "iostream"
#include "simulation/simulation.h"
#include "modeling_tests.cpp"
#include "parsing_tests.cpp"
#include "simulation_tests.cpp"

#include "simulation/random.h"

#include "regex"

int main(int argc, char *argv[])
{
    cout << "Run the tests of the simulation engine." << endl ;

    /* Parsing module. */
//    cout << "\n-----\n-- Tests of the parsing module.\n-----" << endl ;
//    ParsingTests::UtilsTest();
//    ParsingTests::FuncTest();
//    ParsingTests::RandomTest();
//    ParsingTests::ParseTest();

    /* Modeling module */
//    cout << "\n-----\n-- Tests of the modeling module.\n-----" << endl;
//    ModelingTests::MoleculeTest();
//    ModelingTests::ReactionTest();
//    ModelingTests::ParameterTest();
//    ModelingTests::CellFactoryTest();
//    ModelingTests::CellTest();
//    ModelingTests::GrowthTest();
//    ModelingTests::DivisionTest();
//    ModelingTests::ModelTest();

    /* Simulation module */
    cout << "\n-----\n-- Tests of the simulation module.\n-----" << endl;
//    SimulationTests::GenTest();
//    SimulationTests::LineageTest();
//    SimulationTests::OutputsTest();
//    SimulationTests::SettingsTest();
//    SimulationTests::StopConditionTest();
    SimulationTests::Simulate();

    return 0 ;
}
