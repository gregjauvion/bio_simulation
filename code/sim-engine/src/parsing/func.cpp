#include "func.h"

template <typename T>
string Func<T>::display(Func<T> *f) {
    stringstream ss;
    if (f->parentheses())
        ss << "(" + f->toString() + ")";
    else
        ss << f->toString();
    return ss.str();
}

string Sum::toString() { return Func::display(f) + " + " + Func::display(g); }
string Minus::toString() { return Func::display(f) + " - " + Func::display(g); }
string Prod::toString() { return Func::display(f) + " * " + Func::display(g); }
string Div::toString() { return Func::display(f) + " / " + Func::display(g); }
string Power::toString() { return Func::display(f) + " ^ " + Func::display(g); }
string Greater::toString() { return Func<double>::display(f) + " > " + Func<double>::display(g); }
string Lower::toString() { return Func<double>::display(f) + " < " + Func<double>::display(g); }

Sum* Sum::drawRandom(Evaluable* evaluable) { return new Sum(f->drawRandom(evaluable), g->drawRandom(evaluable)); }
Minus* Minus::drawRandom(Evaluable* evaluable) { return new Minus(f->drawRandom(evaluable), g->drawRandom(evaluable)); }
Prod* Prod::drawRandom(Evaluable* evaluable) { return new Prod(f->drawRandom(evaluable), g->drawRandom(evaluable)); }
Div* Div::drawRandom(Evaluable* evaluable) { return new Div(f->drawRandom(evaluable), g->drawRandom(evaluable)); }
Power* Power::drawRandom(Evaluable* evaluable) { return new Power(f->drawRandom(evaluable), g->drawRandom(evaluable)); }
Greater* Greater::drawRandom(Evaluable* evaluable) { return new Greater(f->drawRandom(evaluable), g->drawRandom(evaluable)); }
Lower* Lower::drawRandom(Evaluable* evaluable) { return new Lower(f->drawRandom(evaluable), g->drawRandom(evaluable)); }
