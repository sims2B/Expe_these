//#include <limits>



#ifndef model_mip_simpl
#define model_mip_simpl

#include <vector>
#include <string>
#include <fstream>
#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include "Solution.h"


const int time_limit_simpl=50;

typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloNumVarMatrix> IloNumVar3DMatrix;
typedef IloArray<IloNumVar3DMatrix> IloNumVar4DMatrix;

//LotProblem readFromFile(std::ifstream &instance);

//solve with MILP3
int solveSimple(const LotProblem &, Solution &);


int printSol(const LotProblem&,IloEnv&,  const IloCplex& , 
	       const IloNumVarMatrix& , const IloNumVar4DMatrix& ,
	       const IloNumVar4DMatrix& , const IloNumVarMatrix& );
 
int fillSolution(const LotProblem &LP, Solution &s,const IloCplex &cplex, 
const IloNumVar4DMatrix &D, const IloNumVar4DMatrix &p,const IloNumVarMatrix &BM,
const IloNumVarMatrix &t);

//remplis la solution en s'aasurant que les evenement "maintenance" coincide
//avec des fins de batches
//int fillSolutionBis(const LotProblem &LP, Solution &s,IloCplex &cplex, 
//const IloNumVar4DMatrix &D, const IloNumVar4DMatrix &p,const IloNumVarMatrix &BM,
//const IloNumVarMatrix &t);

//heuristic to find a trivial first solution
int findSolTrivial(const LotProblem& ,std::vector<double>,
		   std::vector<int>&);

//initialise the model with the solution returned by the heuristic
int initMIP(const LotProblem&, IloEnv&, IloModel&,IloCplex&, const IloNumVarMatrix&,
const IloNumVar4DMatrix&, const IloNumVar4DMatrix&,const IloNumVarMatrix&);
		

		      
int createModel(IloEnv& env,IloModel & m, const LotProblem &LP,IloNumVar4DMatrix &D, 
IloNumVar4DMatrix &p,IloNumVarMatrix &BM,IloNumVarMatrix &t);

int createVar(IloEnv& env, const LotProblem &LP,IloNumVar4DMatrix &D, 
IloNumVar4DMatrix &p,IloNumVarMatrix &BM,IloNumVarMatrix &t);

int createConstraint(IloEnv& env,IloModel & m,const  LotProblem &LP,
const IloNumVar4DMatrix &D, const IloNumVar4DMatrix &p,const IloNumVarMatrix &BM,
const IloNumVarMatrix &t);
#endif
