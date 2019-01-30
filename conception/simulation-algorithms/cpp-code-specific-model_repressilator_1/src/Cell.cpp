
#include "Cell.hpp"

RanGen Cell::ran_gen = RanGen(0) ;


bool Cell::should_divide ()
{
	if (V > target_V_div)
	{
		V_div = V ;
		return true ;
	}
	return false ;
}

void Cell::choose_new_size ()
{
	V_birth = V_div * ran_gen.norm (0.5,Parameters::sigma_size_split) ;
	V = V_birth ;
}

void Cell::do_partitioning ()
{
	double p = V_birth / V_div ;
    double old_A = A ; A = 0 ; for (int i=0 ; i<old_A ; i++) if (ran_gen.doub01()<p) A++ ;
    double old_B = B ; B = 0 ; for (int i=0 ; i<old_B ; i++) if (ran_gen.doub01()<p) B++ ;
    double old_C = C ; C = 0 ; for (int i=0 ; i<old_C ; i++) if (ran_gen.doub01()<p) C++ ;
}

void Cell::do_birth_update ()
{
	target_V_div = Parameters::a * V_birth + Parameters::b + ran_gen.norm(0,Parameters::sigma_size_div) ;
	mu_cell = ran_gen.norm(Parameters::mu_avg,Parameters::sigma_mu) ;
}

void Cell::grow (double dt)
{
	V *= exp ( dt * mu_cell ) ;
}


