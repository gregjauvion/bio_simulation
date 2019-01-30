
#include "Reactions.hpp"

double compute_reaction_rate (Cell* cell,int reac) 
{
	switch (reac)
	{
		case 0 :
                return Parameters::synth_A_max / ( 1. + (cell->C/cell->V) / Parameters::C_conc_rep ) ;
				break ;
		case 1 :
                return Parameters::synth_B_max / ( 1. + (cell->A/cell->V) / Parameters::A_conc_rep ) ;
				break ;
		case 2 :
                return Parameters::synth_C_max / ( 1. + (cell->B/cell->V) / Parameters::B_conc_rep ) ;
				break ;
	} 
    return 0 ;
}

void apply_reaction (Cell* cell,int reac)
{
	switch (reac)
	{
		case 0 :
				cell->A++ ;
				break ;
		case 1 :
				cell->B++ ;
				break ;
		case 2 :
				cell->C++ ;
				break ;
	} 	
}
