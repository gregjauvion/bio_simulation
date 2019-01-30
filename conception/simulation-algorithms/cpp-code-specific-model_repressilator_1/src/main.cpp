
#include "Simulation.hpp"
#include "iostream"
using namespace std ;

int main ()
{
	Simulation sim ;
    sim.init_population() ;

    for (int s=0 ; s<200 ; s++)
    {
        sim.do_one_step(0.05) ;
        cout << "size cell 1: " << sim.cells.back()->V << endl ;
        cout << "A level cell 1: " << sim.cells.back()->A << endl << endl ;
    }

	return 0 ;
}
