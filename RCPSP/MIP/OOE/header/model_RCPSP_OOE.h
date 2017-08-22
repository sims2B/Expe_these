#include <fstream>
#include <limits>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include "solution.h"
#include "problem.h"



typedef IloArray<IloNumVarArray> IloNumVarMatrix;

const int time_limit=1000;

int setCplexParam(IloCplex&,IloEnv&,int,int);
int solve_OOE_LP(const problem&, type&);
int solve_OOE_MIP(const problem&,solution&,type&,int nodeLimit= -1, 
int nbSol=-1,bool preempCutOn=0, bool SymCutOn=0);
int solveOOE(const problem&,solution&);

int printSolCplex(const IloEnv&,const IloCplex&,const IloNumVarMatrix&,const IloNumVarArray&);

//******************************************************
//*******************OBJECTIVE************************
//******************************************************

int MinMakespan(IloEnv&, IloModel&,IloNumVar&);

//******************************************************
//*******************CONSTRAINTS************************
//******************************************************

int createConstraintOrd(const problem&,IloModel&, 
			IloNumVarArray&);
int createConstraintOneStart(const problem&, IloEnv&, 
			     IloModel &model, IloNumVarMatrix&) ;
int createConstraintTimeW(const problem&, IloModel&, IloNumVarArray&, 
			  IloNumVarMatrix&, IloNumVar&,const std::vector<type>&);
int createConstraintNonPreemp(const problem&, IloEnv&, 
			      IloModel&, IloNumVarMatrix&);
int createConstraintCapacity(const problem&, IloEnv&, IloModel&, 
			     IloNumVarMatrix&);
int createConstraintDur(const problem&, IloModel&, IloNumVarArray&, IloNumVarMatrix&, IloNumVar&);
int createConstraintPrec(const problem&, IloEnv&,IloModel&, IloNumVarMatrix&); 

//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************


int createConstraintOneStartEnd(const problem&, IloEnv&, IloModel&, IloNumVarMatrix&);
int createConstraintSepEvt(const problem&,IloModel&, IloNumVarArray&,const std::vector<type>& );


//******************************************************
//*******************FILL SOLUTION************************
//******************************************************

int fillSolution(const problem&,solution&,IloCplex&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVar&);

int createModel(const problem&,IloEnv&,IloModel&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVar&);

int createVars(const problem&,IloEnv&,IloNumVarMatrix&);

int createConstraints(const problem&,IloEnv&,IloModel&,
IloNumVarArray&,IloNumVarMatrix&,IloNumVar&);

