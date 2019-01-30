#ifndef GENERATOR_H
#define GENERATOR_H

// I need to include this otherwise it does not compile...
#include "stdlib.h"

#include "simulation/random.h"

/*
 * This class contains the random generators used in the code :
 * - Uniform between 0 and 1
 * - N(mu,sigma)
 * The seeds are fixed, if we want we can make them depend on the clock..
 */
class Generator {
public:
    /* Uniform distribution. */
    static double unif() {return ran_unif->doub();}
    static double unif(double a, double b) {return a + (b - a) * Generator::unif();}
    /* Normal distribution */
    static double norm(double mu, double sigma) {return ran_norm->dev(mu, sigma);}
    static int MAX_ITERATIONS;
    static double norm_pos(double mu, double sigma);
    /* Log-normal distribution */
    static double log_norm(double mu, double sigma) {return exp(ran_norm->dev(mu, sigma));}
    /* Binomial negative distribution */
    // Not implemented yet
    static int MAX_DRAWS;
    static double negative_binomial(double n, double p);

private:
    static Ran* ran_unif;
    static Normaldev* ran_norm;
};

#endif // GENERATOR_H
