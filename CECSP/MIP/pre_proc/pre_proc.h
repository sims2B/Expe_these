#include <iostream>
#include <stdio.h>
#include <vector>
#include "Problem.hpp"


//*******************************************************
//********************COMPUTE BOUND**********************
//*******************************************************
//comparateur de deux intervals
bool intervalComp(const Interval<double>&, const Interval<double>&);
//calcul d'une borne sur t_e
int boundEvts(const Problem<double,double>&, std::vector<double>&);
//calcul d'une borne sur t_{e+1}-t_e
int boundSepEvts(const Problem<double> &,std::vector<std::vector<double>>&,
const int&);
