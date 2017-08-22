//#include <limits>
#include <vector>
#include <string>
#include <tuple>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include "Solution.h"

const int time_limit=3000;

typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloNumVarMatrix> IloNumVar3DMatrix;
typedef IloArray<IloNumVar3DMatrix> IloNumVar4DMatrix;

typedef std::tuple<double,int,int,int,int> mytuple;
bool comparator(const mytuple& l, const mytuple& r); 

//solve with MILP2
int solveBin(const LotProblem &, Solution &);
//solve with MILP3 and use the solution as a MIP Start for MILP2
double solveBinSimple(const LotProblem &, Solution &, Solution &);
//fill the solution object from the solution of the model
int modelToSolution(const LotProblem&,Solution&,IloCplex&,IloNumVarArray &,
IloNumVar4DMatrix&, IloNumVar4DMatrix&, IloNumVarMatrix&);
//fill the MIP start from a soltuion object
int solutionToModel(IloEnv&,IloModel &,const LotProblem&,Solution&,IloNumVarArray&,IloNumVar4DMatrix&, 
IloNumVar4DMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarArray &,IloNumArray &);

////////////////////////////
////Model creation/////////
///////////////////////////
int createModel(IloEnv&,IloModel&, const LotProblem &,IloNumVar4DMatrix &, 
IloNumVar4DMatrix &, IloNumVarMatrix &, 
IloNumVarMatrix &, IloNumVarMatrix &, IloNumVarMatrix &, 
IloNumVarMatrix &, IloNumVarMatrix &,IloNumVarArray &,IloNumVar &, IloNumVarMatrix &);

int createVar(IloEnv& ,const LotProblem &,IloNumVar4DMatrix &, 
IloNumVar4DMatrix &, IloNumVarMatrix &, 
IloNumVarMatrix &, IloNumVarMatrix &, IloNumVarMatrix &, 
IloNumVarMatrix &, IloNumVarMatrix &, IloNumVarMatrix &);

int createConstraint(IloEnv&, IloModel &, const LotProblem &,
IloNumVar4DMatrix &, IloNumVar4DMatrix &, 
IloNumVarMatrix &, IloNumVarMatrix &, IloNumVarMatrix &, 
IloNumVarMatrix &, IloNumVarMatrix &, IloNumVarMatrix &,IloNumVarArray &,
IloNumVar &, IloNumVarMatrix &);

