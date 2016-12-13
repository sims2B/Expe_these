#ifndef DEF_FLOT_MODEL
#define DEF_FLOT_MODEL
#include <fstream>
#include <limits>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include "Problem.h"

//faire des fonctions d'affichages différentes pour ce que je veux dans le main

typedef IloArray<IloNumVarArray> IloNumVarMatrix;
const double M_flot=100000;
const int flot_time_limit=10;
//const int M=100000;

template<typename type>
int flowERTotalTest(Problem<type> &);


template<typename type=double>
int solve(const Problem<type>&);

template<typename type=double>
int flowTotalTest(Problem<type>& P);

template<typename type=double>
int createModel(const Problem<type>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&);

template<typename type=double>
int createVars(const Problem<type>&,IloEnv&,
IloNumVarMatrix&,IloNumVarMatrix&);

template<typename type=double>
int createConstraints(const Problem<type>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&);

#include "flot_model.tpp"
#endif
