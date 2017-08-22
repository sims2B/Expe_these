//#include <limits>


#ifndef model_mip_se
#define model_mip_se

#include <vector>
#include <string>
#include <tuple>
#include <fstream>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include "Solution.h"

const int time_limit=1000;


typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloNumVarMatrix> IloNumVar3DMatrix;
typedef IloArray<IloNumVar3DMatrix> IloNumVar4DMatrix;



//solve with MILP1
int solveReal(LotProblem &, Solution &);

//solve with MILP3 and uses the solution as a MIP start for MILP1
int solveRealSimple(LotProblem &, Solution &, Solution &);

//fill the solution object from the solution of the model
int modelToSolution(LotProblem&,Solution&,IloCplex&,IloNumVarArray &,
IloNumVar3DMatrix&, IloNumVar3DMatrix&, IloNumVar3DMatrix&, 
IloNumVarMatrix&);


typedef std::tuple<double,int,int,int,int> mytuple;
bool comparator(const mytuple& l, const mytuple& r); 
int solutionToModel(IloEnv& env,IloModel & m,LotProblem&,Solution&,IloNumVar3DMatrix&, 
IloNumVar3DMatrix&,IloNumVarMatrix&);

int createModel(IloEnv& env,IloModel & m, LotProblem &LP,
IloNumVar3DMatrix &D, IloNumVar3DMatrix &F,IloNumVar4DMatrix&, 
IloNumVar4DMatrix&, IloNumVar3DMatrix &a, IloNumVarMatrix &p, 
IloNumVarMatrix &HP, IloNumVarMatrix &AD,IloNumVarMatrix &BF, 
IloNumVarMatrix &O, 
IloNumVarMatrix &BM,IloNumVarArray &t,IloNumVar &P,IloNumVarMatrix&);

int createVar(IloEnv& env,LotProblem &LP,IloNumVar3DMatrix &D, 
IloNumVar3DMatrix &F,IloNumVar4DMatrix&, IloNumVar4DMatrix&, 
IloNumVar3DMatrix &a, IloNumVarMatrix &p, IloNumVarMatrix &HP, 
IloNumVarMatrix &AD,IloNumVarMatrix &BF,IloNumVarMatrix &O, 
IloNumVarMatrix &BM,IloNumVarMatrix&);

int createConstraint(IloEnv& env, IloModel &m, LotProblem &LP,
IloNumVar3DMatrix &D, IloNumVar3DMatrix &F, IloNumVar4DMatrix&, 
IloNumVar4DMatrix&, IloNumVar3DMatrix &a, IloNumVarMatrix &p, 
IloNumVarMatrix &HP, IloNumVarMatrix &AD,IloNumVarMatrix &BF,
IloNumVarMatrix &O, 
IloNumVarMatrix &BM,IloNumVarArray &t,IloNumVar &P,IloNumVarMatrix&);

#endif
