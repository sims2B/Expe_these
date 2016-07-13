#ifndef DEF_TIMEMODEL
#define DEF_TIMEMODEL
//#include <fstream>
//#include <limits>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>

#include "Solution.h"

//faire des fonctions d'affichages différentes pour ce que je veux dans le main


typedef IloArray<IloNumVarArray> IloNumVarMatrix;
//const int M=10000;

template<typename type, typename type2=type>
struct timeModel{
  int sol=0;


//********************************************************
//************************ SOLVE *************************
//********************************************************
int Solve(const Problem<type>&,Solution<type,type2>&, double time_limit, int ERIneq=0);

int Solve(const Problem<type>& P,Solution<type,type2> &s,double time_limit,double& tps);

//TODO _b ou véritable expression (cf. manuscrit)
int addERinequalities(const Problem<type>&, IloEnv&, IloModel&, 
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&);


int modelToSol(const Problem<type>&,Solution<type,type2>&,IloCplex&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&);

int createModel(const Problem<type>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);


//TODO si bi ILOINT => wi ILOINT ou ILOFLOAT?
int createVars(const Problem<type>&,IloEnv&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);

int createB(const Problem<type>&,IloEnv&,
IloNumVarMatrix&);

int createConstraints(const Problem<type>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);
};
#include "timeModel.tpp"
#endif
