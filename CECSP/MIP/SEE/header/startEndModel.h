#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include "Solution.h"


const double M=10000;
const int time_limit=7200;
typedef IloArray<IloNumVarArray> IloNumVarMatrix;

IloCplex::Callback getFirstSolInfo(IloEnv env, IloInt& cpt, IloNum startTime);


//********************************************************
//************************ SOLVE *************************
//********************************************************
int setCplexParam(IloCplex&,IloEnv&,int);
int LPsolveSE(const Problem<double>& ,const std::vector<double>&,const std::vector<double>&, int time_limite=1000);


int solveSE(const Problem<double>&,Solution<double,double>&,
const std::vector<int>&);



//************************************************************
//***********************FILL SOLUTION************************
//************************************************************
int modelToSol(const Problem<double>&,Solution<double,double>&,IloCplex&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&);


//*************************************************************
//*********************CREATION MODEL**************************
//*************************************************************

int createModel(const Problem<double>&,IloEnv&,IloModel&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&,IloNumVarMatrix&,const std::vector<int>&);

int createVars(const Problem<double>&,IloEnv&,IloNumVarMatrix&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix& );

int createConstraints(const Problem<double>&,IloEnv&,IloModel&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&,
IloNumVarMatrix&,IloNumVarMatrix&,const std::vector<int>&);

//*******************************************************
//********************OBJECTIVE**************************
//*******************************************************
int createObj(const Problem<double> &, IloEnv&, IloModel&, IloNumVarMatrix&);

//******************************************************
//*******************CONSTRAINTS************************
//******************************************************

int createConstraintOrd(const Problem<double>&,IloModel&,IloNumVarArray&);

int createConstraintOneStart(const Problem<double>&, IloEnv&, 			     IloModel&, IloNumVarMatrix&, IloNumVarMatrix&);

int createConstraintTimeW(const Problem<double> &, IloModel&, IloNumVarArray&, IloNumVarMatrix&, IloNumVarMatrix&,const std::vector<double>&);

int createConstraintCapacity(const Problem<double>&, IloEnv&,IloModel&,IloNumVarArray&, 
IloNumVarMatrix&);

int createConstraintMinDur(const Problem<double>&, IloModel&, IloNumVarArray&,IloNumVarMatrix& ,IloNumVarMatrix&);

int createConstraintEnergy(const Problem<double>&, IloEnv&, IloModel&, IloNumVarArray&,  IloNumVarMatrix&, IloNumVarMatrix&);


int createConstraintBmin(const Problem<double>&P, IloModel&, IloEnv&, IloNumVarArray&, IloNumVarMatrix&, IloNumVarMatrix&,IloNumVarMatrix&,
const std::vector<std::vector<double>>&);
int createConstraintBmin(const Problem<double>&P, IloModel&, IloEnv&,
IloNumVarArray&,IloNumVarMatrix&, IloNumVarMatrix&,IloNumVarMatrix&,
const int&);


int createConstraintBmax(const Problem<double>&P, IloModel&, IloNumVarArray&, IloNumVarMatrix&);

int createConstraintNonConsump(const Problem<double>&, IloModel&, IloEnv&, 
IloNumVarMatrix& , IloNumVarMatrix&,IloNumVarMatrix&, int);


//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************

int createConstraintKnapsack(const Problem<double>&, IloEnv&, IloModel&, 
IloNumVarMatrix&,IloNumVarMatrix&);

int createConstraintSepEvt(const Problem<double>& ,IloModel& , 
IloNumVarArray&,const std::vector<std::vector<double>>& );

int createConstraintBoundEvt(const Problem<double>& P,IloModel& model, IloNumVarArray& t,const std::vector<double>& bd);

