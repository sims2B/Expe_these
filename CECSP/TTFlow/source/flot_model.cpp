#include "flot_model.h"

template int solve(const Problem<int>&);

template int flowTotalTest(Problem<int>& P);

template int createModel(const Problem<int>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&);

template int createVars(const Problem<int>&,IloEnv&,
IloNumVarMatrix&,IloNumVarMatrix&);

template int createConstraints(const Problem<int>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&);


