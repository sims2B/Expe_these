#include <fstream>
#include <limits>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include "solution.h"
#include "problem.h"



typedef IloArray<IloNumVarArray> IloNumVarMatrix;

const int time_limit=7200;


int solveSEE(const problem&,solution&);

int fillSolution(const problem&,solution&,IloCplex&,
IloNumVarArray&,IloNumVarMatrix&);

int createModel(const problem&,IloEnv&,IloModel&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);

int createVars(const problem&,IloEnv&,IloNumVarMatrix&,
IloNumVarMatrix&,IloNumVarMatrix&);

int createConstraints(const problem&,IloEnv&,IloModel&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);

