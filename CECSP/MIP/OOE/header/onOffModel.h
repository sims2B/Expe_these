#include <iostream>
#include <stdio.h>
#include <vector>
#include <ilcplex/ilocplex.h>
#include "Solution.hpp"


typedef std::tuple<int,double,int> evt;
typedef IloArray<IloNumVarArray> IloNumVarMatrix;


#ifndef time_limit
#define time_limit 100
#endif
#ifndef DBL_MAX
#define DBL_MAX std::numeric_limits<double>::max()
#endif
#ifndef M
#define M 10000
#endif

//********************************************************
//********************** CALLBACK ************************
//********************************************************

IloCplex::Callback loggingCallback(IloEnv env, IloNum lastlog, 
IloNum lastIncumbent, IloNum startTime, IloNum startDetTime);
IloCplex::Callback depth(IloEnv env, IloInt& nodeDepth);
IloCplex::Callback noPreemption_tk(IloEnv env, const Problem<double>& P, 
IloNumVarMatrix& z, IloNum eps, IloInt& nbCut, IloInt& nodeDepth, 
IloInt& maxDepth);
IloCplex::Callback getFirstSolInfo(IloEnv env, IloInt& cpt, IloNum startTime);

//********************************************************
//************************ SOLVE *************************
//********************************************************
int setCplexParam(IloCplex&,IloEnv&);

int LPsolveOO(const Problem<double>& P,const std::vector<double>& bound,
const std::vector<double>& bd,bool addTe=0,bool addSep=0, bool addPreem=0, 
bool addKnapsack=0);

int LPsolveOO(const Problem<double>& P,const std::vector<int>&);

int solveOO(const Problem<double>& P,Solution<double,double> &s,double&);

int solveOO(const Problem<double>& P,Solution<double,double> &s,double&,int,int);

int solveOO(const Problem<double>& P,Solution<double,double> &s,
const std::vector<int>&);


//************************************************************
//***********************FILL SOLUTION************************
//************************************************************
int modelToSol(const Problem<double>&,Solution<double,double>&,IloCplex&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&);

//*************************************************************
//*********************CREATION MODEL**************************
//*************************************************************
int createOOModel(const Problem<double>& P, IloEnv& env, IloModel& m, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix& w,const std::vector<int>&) ;
int createOOModel_BB(const Problem<double>& P, IloEnv& env, IloModel& m, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix& w) ;
//************************************************************
//********************CREATION VARIABLE***********************
//************************************************************
int createOOVars(const Problem<double>&,IloEnv&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&);

//****************************************************
//****************contraintes model*******************
//****************************************************
int createOOConstraints(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix &w,const std::vector<int>&) ;
int createOOConstraints(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix &w) ;

//*******************************************************
//********************OBJECTIVE**************************
//*******************************************************
int createObj(const Problem<double> &, IloEnv&, IloModel&, IloNumVarMatrix&);

//******************************************************
//*******************CONSTRAINTS************************
//******************************************************

int createConstraintOrd(const Problem<double>&,IloModel&,IloNumVarArray&);
int createConstraintOneStart(const Problem<double>&, IloEnv&, IloModel&, 
IloNumVarMatrix&);
int createConstraintTimeW(const Problem<double> &, IloModel&, IloNumVarArray&, 
IloNumVarMatrix&,const std::vector<double>&);
int createConstraintNonPreemp(const Problem<double>&, IloEnv&, IloModel&, 
IloNumVarMatrix&);
int createConstraintCapacity(const Problem<double>&, IloEnv&,IloModel&,IloNumVarArray&, 
IloNumVarMatrix&);
int createConstraintMinDur(const Problem<double>&, IloModel&, IloNumVarArray&, 
IloNumVarMatrix&);
int createConstraintEnergy(const Problem<double>&, IloEnv&, IloModel&, 
			   IloNumVarArray&, 
 IloNumVarMatrix&, IloNumVarMatrix&);
int createConstraintBmin(const Problem<double>&P, IloModel&, IloNumVarArray&, IloNumVarMatrix&, IloNumVarMatrix&,
const std::vector<std::vector<double>>&);
int createConstraintBmax(const Problem<double>&P, IloModel&, IloNumVarArray&, IloNumVarMatrix&);
int createConstraintNonConsump(const Problem<double>&, IloModel&, 
IloNumVarMatrix& , IloNumVarMatrix&, int);
int createConstraintTimeW(const Problem<double>&,IloModel&, IloNumVarArray&, IloNumVarMatrix&);
int createConstraintBmin(const Problem<double>&P, IloModel&,
IloNumVarArray&,IloNumVarMatrix&, IloNumVarMatrix&,const int&);
int createConstraintEnergyNul(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarMatrix& w, IloNumVarMatrix& z);
int createConstraintEnergyNul(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarMatrix& w, IloNumVarMatrix& z,const std::vector<std::vector<double>> &M_evt);

//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************
int createConstraintKnapsackExt(const Problem<double>&, IloEnv&, IloModel&, 
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&);
int createConstraintKnapsack(const Problem<double>&, IloEnv&, IloModel&, 
IloNumVarMatrix&);
int createConstraintSepEvt(const Problem<double>& ,IloModel& , IloNumVarArray&,const std::vector<std::vector<double>>& );
int createConstraintBoundEvt(const Problem<double>& P,IloModel& model, IloNumVarArray& t,const std::vector<double>& bd);
/*int createConstraintOneStartEnd(const Problem<double>&, IloEnv&, IloModel&, IloNumVarMatrix&);
int createConstraintEventEnd(const Problem<double>&, IloEnv&, IloModel&, 
IloNumVarMatrix&);
int createConstraintEventStart(const Problem<double>&, IloEnv&, IloModel&, 
IloNumVarMatrix&);*/
//***********************************************
//******************** CUTS *********************
//***********************************************
/*

int makePreempCuts(const IloNumVarMatrix& vars, IloExprArray& lhs, IloNumArray& rhs);
int makeSymCuts(IloEnv& env,const IloNumVarMatrix& vars, IloExprArray& lhs, IloNumArray& rhs);

IloCplex::Callback SymmetryBreak(IloEnv env, const Problem<double>& P, IloNumVarMatrix& z, IloNum eps, IloInt& cpt, IloInt& nodeDepth);
IloCplex::Callback noPreemption(IloEnv env, const Problem<double>& P, IloNumVarMatrix& z, IloNum eps, IloInt& nbCut, IloInt& nodeDepth);*/

