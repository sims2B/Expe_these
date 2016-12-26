#ifndef DEF_TIME_MODEL
#define DEF_TIME_MODEL
//#include <fstream>
//#include <limits>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>

#ifndef M
#define M 100000
#endif

#include "Solution.h"

//faire des fonctions d'affichages différentes pour ce que je veux dans le main


typedef IloArray<IloNumVarArray> IloNumVarMatrix;
//const int M=10000;


//********************************************************
//************************ SOLVE *************************
//********************************************************
int Solve(const Problem<double>&,Solution<double,double>&, double time_limit, int ERIneq);

//int Solve(const Problem<double>& P,Solution<double,double> &s,double time_limit,double& tps);

//TODO _b ou véritable expression (cf. manuscrit)
//int addERinequalities(const Problem<double>&, IloEnv&, IloModel&, 
//IloNumVarMatrix&,IloNumVarMatrix&);


int modelToSol(const Problem<double>&,Solution<double,double>&,IloCplex&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&);

int createModel(const Problem<double>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);


//TODO si bi ILOINT => wi ILOINT ou ILOFLOAT?
int createVars(const Problem<double>&,IloEnv&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);

int createB(const Problem<double>&,IloEnv&,
IloNumVarMatrix&);

int createConstraints(const Problem<double>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);
int addERinequalities(const Problem<double>& P, IloEnv& env, IloModel& model, 
IloNumVarMatrix& x,IloNumVarMatrix& y);
#endif
