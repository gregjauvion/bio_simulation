#ifndef FUNC_H
#define FUNC_H

#include "parameter.h"
#include "evaluable.h"
#include "sstream"
#include "random"
#include "simulation/generator.h"
#include "cmath"

/*
 * A Func object represents an expression whose evaluation returns a <T> object.
 * Some of its components may point to the different methods of an Evaluable object.
 */

// The generic function
template<typename T>
class Func
{
public:
    // Evaluate the function for an Evaluable object
    virtual T eval(Evaluable* evaluable) = 0;
    // Displays the function
    virtual string toString() = 0;
    // Returns true if the element should be displayed with parentheses
    virtual bool parentheses() = 0;
    // Returns the Func object after having drawn the random values
    virtual Func<T>* drawRandom(Evaluable* evaluable) { (void) evaluable; return this; }
    // Used to display a function (with/without parentheses depending on parenthese() value)
    static string display(Func* f);
};

// A constant function
class Const : public Func<double> {
public:
    Const(double constant_) : constant(constant_) {}
    virtual double eval(Evaluable* evaluable) { (void) evaluable; return constant; }
    virtual string toString() { return to_string(constant); }
    virtual bool parentheses() { return false; }
private:
    double constant;
};

// A function returning the concentration of molecule of index i
class Conc : public Func<double> {
public:
    Conc(int index_) : index(index_) {}
    virtual double eval(Evaluable* evaluable) { return evaluable->getQuantities()[index] / evaluable->getVolume(); }
    virtual string toString() { return "[" + to_string(index) + "]"; }
    virtual bool parentheses() { return false; }
private:
    int index;
};

// A function returning the absolute number of molecule of index i
class Abs : public Func<double> {
public:
    Abs(int index_) : index(index_) {}
    virtual double eval(Evaluable* evaluable) { return evaluable->getQuantities()[index]; }
    virtual string toString() { return "{" + to_string(index) + "}"; }
    virtual bool parentheses() { return false; }
private:
    int index;
};

// A function returning a parameter value
class Param : public Func<double> {
public:
    Param(Parameter* parameter_) : parameter(parameter_) {}
    virtual double eval(Evaluable* evaluable) { (void) evaluable; return parameter->getValue(); }
    virtual string toString() { return parameter->getName(); }
    virtual bool parentheses() { return false; }
private:
    Parameter* parameter;
};

// A function calling the getVolume() method
class Volume : public Func<double> {
public:
    Volume() {}
    virtual double eval(Evaluable* evaluable) { return evaluable->getVolume(); }
    virtual string toString() { return "V"; }
    virtual bool parentheses() { return false; }
};

// A function calling the getVolumeBirth() method
class VolumeBirth : public Func<double> {
public:
    VolumeBirth() {}
    virtual double eval(Evaluable* evaluable) { return evaluable->getVolumeBirth(); }
    virtual string toString() { return "V_birth"; }
    virtual bool parentheses() { return false; }
};

// A function which represents an uniform distribution between lower and upper
class Unif : public Func<double> {
public:
    Unif(Func<double>* lower_, Func<double>* upper_) : lower(lower_), upper(upper_) {}
    virtual double eval(Evaluable* evaluable) { return Generator::unif(lower->eval(evaluable), upper->eval(evaluable)); }
    virtual string toString() { return "U("+lower->toString()+","+upper->toString()+")"; }
    virtual bool parentheses() { return false; }
    virtual Const* drawRandom(Evaluable* evaluable) { return new Const(Generator::unif(lower->eval(evaluable), upper->eval(evaluable))); }
private:
    Func<double>* lower;
    Func<double>* upper;
};

// A function which represents a normal distribution of mean mu and sted sigma
class Normal : public Func<double> {
public:
    Normal(Func<double>* mu_, Func<double>* sigma_) : mu(mu_), sigma(sigma_) {}
    virtual double eval(Evaluable* evaluable) { return Generator::norm(mu->eval(evaluable), sigma->eval(evaluable)); }
    virtual string toString() { return "N("+mu->toString()+","+sigma->toString()+")"; }
    virtual bool parentheses() { return false; }
    virtual Const* drawRandom(Evaluable* evaluable) { return new Const(Generator::norm(mu->eval(evaluable), sigma->eval(evaluable))); }
private:
    Func<double>* mu;
    Func<double>* sigma;
};

// A function which represents a positive normal distribution of mean mu and sted sigma
class NormalPositive : public Func<double> {
public:
    NormalPositive(Func<double>* mu_, Func<double>* sigma_) : mu(mu_), sigma(sigma_) {}
    virtual double eval(Evaluable* evaluable) { return Generator::norm_pos(mu->eval(evaluable), sigma->eval(evaluable)); }
    virtual string toString() { return "Npos("+mu->toString()+","+sigma->toString()+")"; }
    virtual bool parentheses() { return false; }
    virtual Const* drawRandom(Evaluable* evaluable) { return new Const(Generator::norm_pos(mu->eval(evaluable), sigma->eval(evaluable))); }
private:
    Func<double>* mu;
    Func<double>* sigma;
};

// A function which represents a log-normal distribution of mean mu and std sigma
class LogNormal : public Func<double> {
public:
    LogNormal(Func<double>* mu_, Func<double>* sigma_) : mu(mu_), sigma(sigma_) {}
    virtual double eval(Evaluable* evaluable) { return Generator::log_norm(mu->eval(evaluable), sigma->eval(evaluable)); }
    virtual string toString() { return "LN("+mu->toString()+","+sigma->toString()+")"; }
    virtual bool parentheses() { return false; }
    virtual Const* drawRandom(Evaluable* evaluable) { return new Const(Generator::log_norm(mu->eval(evaluable), sigma->eval(evaluable))); }
private:
    Func<double>* mu;
    Func<double>* sigma;
};

// A function which represents a negative-binomial distribution of parameters r and p
class NegativeBinomial : public Func<double> {
public:
    NegativeBinomial(Func<double>* r_, Func<double>* p_) : r(r_), p(p_) {}
    virtual double eval(Evaluable* evaluable) { return Generator::negative_binomial(r->eval(evaluable), p->eval(evaluable)); }
    virtual string toString() { return "NB("+r->toString()+","+p->toString()+")"; }
    virtual bool parentheses() { return false; }
    virtual Const* drawRandom(Evaluable* evaluable) { return new Const(Generator::negative_binomial(r->eval(evaluable), p->eval(evaluable))); }
private:
    Func<double>* r;
    Func<double>* p;
};

// The sum of two functions
class Sum : public Func<double> {
public:
    Sum(Func<double>* f_, Func<double>* g_) : f(f_), g(g_) {}
    virtual double eval(Evaluable* evaluable) { return f->eval(evaluable) + g->eval(evaluable); }
    virtual string toString();
    virtual bool parentheses() { return true; }
    virtual Sum* drawRandom(Evaluable* evaluable);
private:
    Func<double>* f;
    Func<double>* g;
};

// The difference of two functions
class Minus : public Func<double> {
public:
    Minus(Func<double>* f_, Func<double>* g_) : f(f_), g(g_) {}
    virtual double eval(Evaluable* evaluable) { return f->eval(evaluable) - g->eval(evaluable); }
    virtual string toString();
    virtual bool parentheses() { return true; }
    virtual Minus* drawRandom(Evaluable* evaluable);
private:
    Func<double>* f;
    Func<double>* g;
};

// The division of two functions
class Div : public Func<double> {
public:
    Div(Func<double>* f_, Func<double>* g_) : f(f_), g(g_) {}
    virtual double eval(Evaluable* evaluable) { return f->eval(evaluable) / g->eval(evaluable); }
    virtual string toString();
    virtual bool parentheses() { return true; }
    virtual Div* drawRandom(Evaluable* evaluable);
private:
    Func<double>* f;
    Func<double>* g;
};

// The product of two functions
class Prod : public Func<double> {
public:
    Prod(Func<double>* f_, Func<double>* g_) : f(f_), g(g_) {}
    virtual double eval(Evaluable* evaluable) { return f->eval(evaluable) * g->eval(evaluable); }
    virtual string toString();
    virtual bool parentheses() { return true; }
    virtual Prod* drawRandom(Evaluable* evaluable);
private:
    Func<double>* f;
    Func<double>* g;
};

// f^g where f and g are two functions
class Power : public Func<double> {
public:
    Power(Func<double>* f_, Func<double>* g_) : f(f_), g(g_) {}
    virtual double eval(Evaluable* evaluable) { return pow(f->eval(evaluable), g->eval(evaluable)); }
    virtual string toString();
    virtual bool parentheses() { return true; }
    virtual Power* drawRandom(Evaluable* evaluable);
private:
    Func<double>* f;
    Func<double>* g;
};

// A boolean expression of the form f > g
class Greater : public Func<bool> {
public:
    Greater(Func<double>* f_, Func<double>* g_) : f(f_), g(g_) {}
    virtual bool eval(Evaluable* evaluable) { return f->eval(evaluable) > g->eval(evaluable); }
    virtual string toString();
    virtual bool parentheses() { return false; }
    virtual Greater* drawRandom(Evaluable* evaluable);
private:
    Func<double>* f;
    Func<double>* g;
};

// A boolean expression of the form f < g
class Lower : public Func<bool> {
public:
    Lower(Func<double>* f_, Func<double>* g_) : f(f_), g(g_) {}
    virtual bool eval(Evaluable* evaluable) { return f->eval(evaluable) < g->eval(evaluable); }
    virtual string toString();
    virtual bool parentheses() { return false; }
    virtual Lower* drawRandom(Evaluable* evaluable);
private:
    Func<double>* f;
    Func<double>* g;
};

#endif // FUNC_H
