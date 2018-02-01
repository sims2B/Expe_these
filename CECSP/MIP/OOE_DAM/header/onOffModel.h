#include <iostream>
#include <stdio.h>
#include <vector>
#include <ilcplex/ilocplex.h>
#include "Solution.hpp"


typedef std::tuple<int,double,int> evt;
typedef IloArray<IloNumVarArray> IloNumVarMatrix;


#ifndef time_limit
#define time_limit 1000
#endif
#ifndef DBL_MAX
#define DBL_MAX std::numeric_limits<double>::max()
#endif
#ifndef M
#define M 10000
#endif

#define USERCUTLIMIT 500
//*******************************************************
//********************COMPUTE BOUND**********************
//*******************************************************
//comparateur de deux intervals
bool intervalComp(const Interval<double>&, const Interval<double>&);
//calcul d'une borne sur t_e
int boundEvts(const Problem<double,double>&, std::vector<double>&);
//calcul d'une borne sur t_{e+1}-t_e
int boundSepEvts(const Problem<double> &,std::vector<double>&);


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
IloCplex::Callback EminSmaxConstraint(IloEnv env, const Problem<double>& P, IloNumVarMatrix& z, IloNumVarArray& t, const std::vector<double>&);

//********************************************************
//************************ SOLVE *************************
//********************************************************
int setCplexParam(IloCplex&,IloEnv&);

int LPsolveOO(const Problem<double>& P,const std::vector<int>&);

int solveOO(const Problem<double>& P,Solution<double,double> &s,
const std::vector<int>&);
void displayCVS(const IloCplex& cplex, const std::vector<int>& config,
		const IloNum&  time_exec, const int& cptCut);
void displayCplexResults(const IloCplex& cplex, const std::vector<int>& config,
			 const IloNum&  time_exec, const int& cptCut);

//************************************************************
//***********************FILL SOLUTION************************
//************************************************************
int modelToSol(const Problem<double>&,Solution<double,double>&,IloCplex&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVarMatrix&);

//*************************************************************
//*********************CREATION MODEL**************************
//*************************************************************
int createOOModel(const Problem<double>& P, IloEnv& env, IloModel& m, IloNumVarArray& t,
		  IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix& w,
		  const std::vector<int>&) ;
//************************************************************
//********************CREATION VARIABLE***********************
//************************************************************
int createOOVars(const Problem<double>&,IloEnv&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&);

//****************************************************
//****************contraintes model*******************
//****************************************************
int createOOConstraints(const Problem<double>& P, IloEnv& env, IloModel& model,
			IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b,
			IloNumVarMatrix &w,const std::vector<int>&) ;

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
int createEnergyConstraints(const Problem<double>&, IloEnv&, IloModel&, 
			    IloNumVarArray&, IloNumVarMatrix&,
			   IloNumVarMatrix&, IloNumVarMatrix&);
int createConstraintBmin(const Problem<double>&P, IloModel&, IloNumVarArray&,
			 IloNumVarMatrix&, IloNumVarMatrix&,
			 const std::vector<double>&);
int createConstraintBmin(const Problem<double>&P, IloModel&,
			 IloNumVarArray&,IloNumVarMatrix&, IloNumVarMatrix&);
int createConstraintBmax(const Problem<double>&P, IloModel&, IloNumVarArray&,
			 IloNumVarMatrix&);
int createConstraintNonConsump(const Problem<double>&, IloModel&, 
			       IloNumVarMatrix& , IloNumVarMatrix&);
int createConstraintTimeW(const Problem<double>&,IloModel&, IloNumVarArray&,
			  IloNumVarMatrix&);
int createConstraintEnergyNul(const Problem<double>& P, IloEnv& env, IloModel& model,
			      IloNumVarMatrix& w, IloNumVarMatrix& z);
int createConstraintEnergyNul(const Problem<double>& P, IloEnv& env, IloModel& model,
			      IloNumVarMatrix& w, IloNumVarMatrix& z,
			      const std::vector<double> &M_evt);

//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************
int createConstraintKnapsack(const Problem<double>&, IloEnv&, IloModel&, 
			     IloNumVarMatrix&);
int createConstraintSepEvt(const Problem<double>& ,IloModel& , IloNumVarArray&,
			   const std::vector<double>& );
int createConstraintBoundEvt(const Problem<double>& P,IloModel& model, IloNumVarArray& t,
			     std::vector<double>& bd);
