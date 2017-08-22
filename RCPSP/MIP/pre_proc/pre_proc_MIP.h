#include <iostream>
#include <stdio.h>
#include <vector>
#include "problem.h"


//*******************************************************
//********************COMPUTE BOUND**********************
//*******************************************************
bool intervalComp(const interval&, const interval&);
int boundEvts(const problem&, std::vector<type>&);
int boundSepEvts(const problem &,std::vector<type>&);
type maxSepEvts(const std::vector<type> &);
