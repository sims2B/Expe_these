#include "model_RCPSP_SEE.h"
#include <iostream>
#include <vector>

int solveSEE(const problem& P,solution &s) {
  try{
    IloNum start,time_exec;
    const int nbTask = P.nbTask;
    const int E=nbTask+1;
      
    IloEnv env;
    IloModel model(env);
    
    IloNumVarMatrix x(env,nbTask);
    IloNumVarMatrix y(env,nbTask);
    IloNumVarMatrix r(env,P.R.size());
    IloNumVarArray t(env, E, 0, IloInfinity, ILOFLOAT);
    
    // create cplex model
    createModel(P,env,model,t,x,y,r);

    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setOut(env.getNullStream());
    start= cplex.getCplexTime();
    
    int solve=cplex.solve();
    time_exec=cplex.getCplexTime()-start;
    std::cout << "status: "<< cplex.getStatus() << " en " 
	      << time_exec <<  std::endl;
    if (solve){	
      std::cout << "Obj: " << cplex.getObjValue()<< std::endl;
      fillSolution(P,s,cplex,t,x);
      env.end();
      return 0;
    }
    else {
      env.end();
      return 1;
    }
  }
  catch (IloException &e){
    std::cout << "Iloexception in solve" << e << std::endl;
    e.end();
    return  1;
  }
}


int createModel(const problem& P, IloEnv& env, IloModel& m, IloNumVarArray& t, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& r) {
  try {
    return createVars(P,env,x,y,r) || createConstraints(P,env,m,t,x,y,r);
  }
  catch (IloException& e){
    std::cout << "iloexception in createmodel" << e <<std::endl;
    e.end();
    return  1;
  }
}

int createVars(const problem& P, IloEnv& env, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& r){
  try{
    int i;
    const int nbTask=P.nbTask;
    const int E=nbTask+1;
    for (i=0;i<nbTask;i++){
      x[i]=IloNumVarArray(env, E, 0, 1, ILOINT);
      y[i]=IloNumVarArray(env, E, 0, 1, ILOINT);
    } 
    for (uint k=0;k<P.R.size();++k)
      r[k]=IloNumVarArray(env,E, 0, P.R[k].B, ILOFLOAT);
    return 0;
  }
  catch (IloException &e){
    std::cout << "iloexception in create vars" <<e <<std::endl;
    e.end();
    return  1;
  }
}

int createConstraints(const problem& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& r){
  try{
    const int nbTask=P.nbTask;
    const int E=nbTask+1;
    uint j,k;
    int i,e;
    
    //objectif (min makespan)
    model.add(IloMinimize(env,t[E-1]));

    //contrainte te < te+1
    for (e=0;e<E-1;++e){
      model.add(t[e]<=t[e+1]); 
    }
  
    //contrainte sum xie=1 et sum yie=1
    for (i=0;i<nbTask;++i){
      IloExpr exprX(env);
      IloExpr exprY(env);
      for (e=0;e<E;++e){
	exprX+=x[i][e];
	exprY+=y[i][e];
      }
      model.add(exprX==1);
      model.add(exprY==1);
      exprX.end();
      exprY.end();
    }
  
    //contrainte ES < te < LS
    for (i=0;i<nbTask;i++){
      for (e=0;e<E;e++){
	model.add(t[e]>=P.A[i].tw.t1*x[i][e]);
	model.add(t[e]<= P.A[i].tw.t2*x[i][e]+ 
		  P.A[nbTask-1].tw.t2*(1-x[i][e]));
	model.add(t[e]>=(P.A[i].tw.t1+P.A[i].p)*y[i][e]);
	model.add(t[e]<=(P.A[i].tw.t2+P.A[i].p)*y[i][e]+P.A[nbTask-1].tw.t2*(1-y[i][e]));
      }
    }
    
    model.add(P.A[nbTask-1].tw.t1<=t[E-1]<=P.A[nbTask-1].tw.t2);
  
    //contrainte r_ek<Bk
    for (k=0;k<P.R.size();++k){
      IloExpr expr(env);
      for (i=0;i<nbTask;++i)
	expr+=P.A[i].b[k]*x[i][0];
      model.add(r[k][0]==expr);
      model.add(r[k][0]<=P.R[k].B);
      expr.end();
    }
    

    for (k=0;k<P.R.size();++k){
      for (e=1;e<E;++e){
	IloExpr expr(env);
	for (i=0;i<nbTask;i++)
	  expr+=P.A[i].b[k]*x[i][e]-P.A[i].b[k]*y[i][e];
	model.add(r[k][e]==r[k][e-1]+expr);
	expr.end();
	model.add(r[k][e]<=P.R[k].B);
      }
    }
    
	
    //inégalité separation SE
    for (i=0;i<nbTask;i++){
      for (e=0;e<E;e++) {
	for (int f=e;f<E;f++)
	  model.add(t[f] >= t[e] + P.A[i].p*x[i][e]-P.A[i].p*(1-y[i][f]));
      }
    }
  
    model.add(t[0]==0);

    //contrainte precedence
    for (i=0;i<nbTask;++i){
      for (j=0;j<P.A[i].succ.size();++j){
	for (e=0;e<E;++e){
	  IloExpr expr(env);
	  for (int f=0;f<E;++f)
	    expr= (f<e) ? expr+x[P.A[i].succ[j]][f] : expr+y[i][f];
	  model.add(expr<=1);
	  expr.end();
	}
      }
    }
  
    //contrainte start<end
    for (i=0;i<nbTask;i++){
      for (e=0;e<E;e++){
	IloExpr expr(env);
	for (int f=0;f<=e;++f)
	  expr+= x[i][f]-y[i][f];
	model.add(expr>=0);
	expr.end();
      }
    }    
    
    return 0;
  }
  catch (IloException& e){
    std::cout << "iloexcpetion in createConstraint" << e <<std::endl;
    e.end();
    return  1;
  }
}


int fillSolution(const problem &P,solution &s,IloCplex& cplex,IloNumVarArray& t,IloNumVarMatrix& x){
  const int E=P.nbTask+1;
  for (int i=0;i<P.nbTask;++i){
    for (int e=0;e<E;++e)
      s.st[i]=(cplex.getValue(x[i][e])==1)? cplex.getValue(t[e]) :s.st[i];
  }
  return 0;
}
