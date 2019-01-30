
#include "Simulation.hpp"


void Simulation::init_population()
{
    Cell* cell = new Cell ;
    cell->V = 1.0 ; cell->V_birth = cell->V ;
    cells.push_back(cell) ;
}

void Simulation::do_one_step (double dt)
{
	do_growth (dt) ;
	do_reactions (dt) ;
    do_divisions () ;
}

void Simulation::do_growth (double dt)
{
	for ( list<Cell*>::iterator it = cells.begin(); it != cells.end(); it++)
	{
		(*it)->grow(dt) ;
	}
}

void Simulation::do_reactions (double dt)
{
	for ( list<Cell*>::iterator it = cells.begin(); it != cells.end(); it++)
	{
		do_reactions_cell ( (*it) , dt ) ;
	}
}

void Simulation::do_divisions ()
{
for ( list<Cell*>::iterator it = cells.begin(); it != cells.end(); it++)
	{
        if ( (*it)->should_divide() )
		{
			(*it)->choose_new_size() ;
			(*it)->do_partitioning() ;
			(*it)->do_birth_update() ;
		}
	}
}


void Simulation::do_reactions_cell (Cell* cell,double dt)
{
	double tau = find_next_reaction_time_cell (cell) ;
	if (tau < dt)
	{
        int reac = choose_reaction () ;
        apply_reaction (cell,reac) ;
	}
	return ;
}

void Simulation::compute_rates_cell (Cell* cell)
{
	sum_rates = 0 ;
	for (int r=0 ; r<num_reactions ; r++)
	{
		rates[r] = compute_reaction_rate (cell,r) ;
		sum_rates += rates[r] ;	
	} 
}

double Simulation::find_next_reaction_time_cell (Cell* cell)
{
	compute_rates_cell (cell) ;
    double tau = -log ( Cell::ran_gen.doub01() ) / sum_rates ;
	return tau ;
}

int Simulation::choose_reaction ()
{
	double u = Cell::ran_gen.doub01() ;
    double cum_rates = 0 ; int r = 0 ;
	while (cum_rates < u * sum_rates)
	{
		cum_rates += rates[r] ;
		r++ ;
	}
	return r-1 ;
}

