
#include "generator.h"

// Maximum number of iterations to generate a normal positive value
int Generator::MAX_ITERATIONS = 100;

// Maximum number of draws when generating negative binomial values
int Generator::MAX_DRAWS = 10000;

Ran* Generator::ran_unif = new Ran(123456789L);

Normaldev* Generator::ran_norm = new Normaldev(987654321L);

// Draws values from the normal distribution and returns the first positive one.
// We limit to MAX_ITERATIONS the number of iterations, otherwise we return 0.
double Generator::norm_pos(double mu, double sigma) {
    double ret = -1;
    int nb = 0;
    while (ret<0 && nb<Generator::MAX_ITERATIONS) {
        ret = Generator::norm(mu, sigma);
        nb += 1;
    }
    if (nb==Generator::MAX_ITERATIONS) return 0;
    return ret;
}

/*
 * The negative binomial with parameters n and p is the distribution of the number of failures before observing n successes.
 * p is the success probability.
 */
double Generator::negative_binomial(double n, double p) {
    // This distribution is not well defined if n=0 or p=0
    if (n==0 || p==0) return 0;
    int nb_successes = 0;
    int nb_draws = 0;
    while (nb_successes<n && nb_draws<Generator::MAX_DRAWS) {
        bool success = Generator::unif() < p;
        if (success) nb_successes++;
        nb_draws++;
    }
    return nb_draws-nb_successes;
}
