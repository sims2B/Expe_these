#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Solution.hpp"
#include "satisfiability.h"



//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEFINITION /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

typedef IloArray<IloNumVarArray> IloNumVarMatrix;

//********************* SOLVE FUNCTION ************************
template<typename type, typename type2=type>
int Solve(const Problem<type>&,Solution<type,type2>&, 
	  double time_limit, int ERIneq=0);

template<>
int Solve(const Problem<double>& P,Solution<double,double> &s,
	  double time_limit,int ERIneq=0)

template<typename type, typename type2=type>
int Solve(const Problem<type>& P,Solution<type,type2> &s,
	  double time_limit , double& tps);

//TODO _b ou véritable expression (cf. manuscrit)
template<typename type, typename type2=type>
int addERinequalities(const Problem<type>&, IloEnv&, IloModel&, 
IloNumVarMatrix&,IloNumVarMatrix&);



//********************* MODEL CREATION ************************


//************************** CALLBACK ****************************


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// IMPLEMENTATION //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////







template<typename type,typename type2>
int timeModel<type,type2>::Solve(const Problem<type>& P,Solution<type,type2> &s,double time_limit,int ERIneq) {
  try{
    IloNum start,time_exec;
    const int n= P.nbTask;
    IloInt cptCut=0;
  
    IloEnv env;
    IloModel model(env);
  		
    IloNumVarMatrix x(env,n);
    IloNumVarMatrix y(env,n);
    IloNumVarMatrix b(env,n);
    IloNumVarMatrix w(env,n);

    createModel(P,env,model,x,y,b,w);
    IloCplex cplex(model);

    if (ERIneq==1) {
      std::cout << " Starting resolution with ER inequalities at root node\n";
      addERinequalities(P,env,model,x,y);
    }
  
    if (ERIneq==2) {
      std::cout << " Starting resolution with ER inequalities in tree <10\n";
      IloInt nodeDepth=0;
      IloInt maxDepth=10;
      cplex.use(depth(env,nodeDepth));
      cplex.use(energyCuts(env,P,x,y, 0.01, cptCut,nodeDepth,maxDepth));
    }
    else 
      std::cout << " Starting resolution without ER inequalities\n";
				
    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setParam(IloCplex::Threads,2);
    cplex.setOut(env.getNullStream());
    start= cplex.getCplexTime();
    IloInt cpt=0;
    cplex.use(getFirstSolInfo(env,cpt,start));
    // solve !
    if (cplex.solve()) {	 
      time_exec=cplex.getCplexTime()-start;
      std::cout << "Final status: \t"<< cplex.getStatus() << " en " 
		<< time_exec << std::endl;
      std:: cout << "Final objective: " << cplex.getObjValue() 
		 <<"\nFinal gap: \t" << cplex.getMIPRelativeGap()
		 << std::endl;   
      if (ERIneq==2) 
	std::cout << "number of preemp cuts: "
		  << cptCut << "\n";
      modelToSol(P,s,cplex,x,y,b);
      env.end();
      return 0;
    }
    else if (cplex.getStatus()==IloAlgorithm::Infeasible){
      time_exec=cplex.getCplexTime()-start;
      if (ERIneq==2) std::cout << "number of ER cuts: "
			       << cptCut << "\n";
      std::cout << "status: "<< cplex.getStatus() << " en " 
		<< time_exec << std::endl;
    }
    env.end();
    return 1;
  }
  catch (IloException &e) {
    std::cout << "Iloexception in solve" << e << std::endl;
    e.end();
    return 1;
  } 
  catch (...){
    std::cout << "Error unknown\n";
    return 1;
  }
}

template<>
int timeModel<double,double>::Solve(const Problem<double>& P,Solution<double,double> &s,double time_limit,int ERIneq) {
  try{
    IloNum start,time_exec;
    const int n= P.nbTask;
    IloInt cptCut=0;
  
    IloEnv env;
    IloModel model(env);
  
    IloNumVarMatrix x(env,n);
    IloNumVarMatrix y(env,n);
    IloNumVarMatrix b(env,n);
    IloNumVarMatrix w(env,n);
  
    createModel(P,env,model,x,y,b,w);
    IloCplex cplex(model);
  
    if (ERIneq==1) {
      std::cout << " Starting resolution with ER inequalities at root node\n";
      addERinequalities(P,env,model,x,y);
    }
  
    else if (ERIneq==2) {
      IloInt nodeDepth=0;
      IloInt maxDepth=10;
      std::cout << " Starting resolution with ER inequalities in tree < " << maxDepth << "\n";
      cplex.use(depth(env,nodeDepth));
      cplex.use(energyCuts(env,P,modle,x,y, 0.01,cptCut, nodeDepth,maxDepth));
    }
    else 
      std::cout << " Starting resolution without ER inequalities\n";
  
    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setParam(IloCplex::Threads,2);
    //cplex.setOut(env.getNullStream());
    start= cplex.getCplexTime();
    IloInt cpt=0;
    cplex.use(getFirstSolInfo(env,cpt,start));
    // solve !
    if (cplex.solve()) {
      time_exec=cplex.getCplexTime()-start;
      std::cout << "Final status: \t"<< cplex.getStatus() << " en " 
		<< time_exec << std::endl;
      std:: cout << "Final objective: " << cplex.getObjValue() 
		 <<"\nFinal gap: \t" << cplex.getMIPRelativeGap()
		 << std::endl;   
      modelToSol(P,s,cplex,x,y,b);
      env.end();
      return 0;
    }
    else if (cplex.getStatus()==IloAlgorithm::Infeasible){
      time_exec=cplex.getCplexTime()-start;
      std::cout << "status: "<< cplex.getStatus() << " en " 
		<< time_exec << std::endl;
    }
    env.end();
    return 1;
  }
  catch (IloException &e) {
    std::cout << "Iloexception in solve : " << e << std::endl;
    e.end();
    return 1;
  } 
  catch (...){
    std::cout << "Error unknown\n";
    return 1;
  }
}



template<typename type,typename type2>
int timeModel<type,type2>::addERinequalities(
					     const Problem<type>& P, IloEnv& env,
					     IloModel &model,IloNumVarMatrix& x,
					     IloNumVarMatrix&  y) {
  /*  int t, i;
  IntervalList<type> list;
  computeCheckInterval(list,P);
  Interval<type> current;
  type2 _b;
  for (uint I=0;I<list.size();++I){
    current=list[I];
    if (current.t1 < current.t2){
      type _B=P.totalResourceConsumption(current) ;
      for (i=0;i<P.nbTask;++i){
	_B= _B - P.A[i].resourceConsumption(current);
	_b=P.A[i].resourceConversion(P.W(i),current);
	IloExpr expr(env),expr2(env),expr3(env),expr4(env),expr5(env);
	for (t=0;t<=current.t1;++t)
	  expr-=x[i][t];
	for (t=current.t2+1;t<=P.D;++t)
	  expr-=y[i][t];
	model.add((1+expr)*_b + _B <= P.B*(current.t2 - current.t1));
	
	_b=P.A[i].resourceConversion(std::max(0.0,P.W(i)-P.A[i].Fi(P.bmax(i))*(std::max(0.0,current.t1-P.r(i)) +std::max(0.0, P.d(i) - current.t2))),current);
	model.add((x[i][P.A[i].ri]+ y[i][P.A[i].di]-1)*_b+_B <= P.B*(current.t2 - current.t1));
	
	_b=P.A[i].resourceConversion(P.A[i].leftShift(current),current);
	for (t=current.t1+1;t<current.t2;++t)
	  expr3+=y[i][t];
	model.add((expr3+x[i][P.A[i].ri]-1)*_b + _B <= P.B*(current.t2 - current.t1));
	expr3.end();
	
	_b=P.A[i].resourceConversion(P.A[i].rightShift(current),current);	
	for (t=current.t1;t<current.t2;++t)
	  expr4+=x[i][t];
	model.add((expr4+ y[i][P.A[i].di-1])*_b + _B <= P.B*(current.t2 - current.t1));
	expr4.end();
	
	_b=P.bmin(i)*(current.t2 - current.t1);	
	for (t=0;t<=current.t1;++t)
	  expr5+=x[i][t];
	  for (t=current.t1; t <std::min(P.D,current.t2+1);++t)
	    //		  expr2+=b[i][t];
	for (t=current.t2;t<=P.D;++t)
	  expr5+=y[i][t];
	model.add( (expr5-1)*_b +_B <= P.B*(current.t2 - current.t1));
	expr5.end();
	expr2.end();
	_B = _B + P.A[i].resourceConsumption(current);
      }
    }
    } */
  return 0;
}


