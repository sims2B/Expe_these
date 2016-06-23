#include <iostream>
#include <stdio.h>
#include <vector>
#include "Problem.h"


//*******************************************************
//********************COMPUTE BOUND**********************
//*******************************************************
bool intervalComp(const Interval<double>&, const Interval<double>&);
int boundEvts(const Problem<double>&, std::vector<double>&);
int boundSepEvts(const Problem<double> &,std::vector<std::vector<double>>&,
const int&);
