
#ifndef CELL_HPP
#define CELL_HPP

#include "RanGen.hpp"
#include "Parameters.hpp"

struct Cell
{
	double A ;
	double B ;
	double C ;

	double V ;
	double mu_cell ;

	double target_V_div ;
	double V_div ;
	double V_birth ;

	bool should_divide () ;
    void choose_new_size () ;
	void do_partitioning () ;
	void do_birth_update () ;

	void grow (double dt) ;

	static RanGen ran_gen ;
} ;

#endif
