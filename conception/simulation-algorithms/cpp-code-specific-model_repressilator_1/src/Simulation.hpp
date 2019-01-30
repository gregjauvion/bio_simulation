
#include "Cell.hpp"
#include "Reactions.hpp"
#include "list"
using namespace std ;

struct Simulation
{
    static const int num_reactions = 3 ;
    double rates[num_reactions] ;
	double sum_rates ;

	list<Cell*> cells ;

	void init_population () ;
	
	void do_one_step (double dt) ;
	void do_reactions (double dt) ;
	void do_growth (double dt) ;
    void do_divisions () ;

	void do_reactions_cell (Cell* cell,double dt) ;
	double find_next_reaction_time_cell (Cell* cell) ;
	void compute_rates_cell (Cell* cell) ;
    int choose_reaction () ;


} ;
