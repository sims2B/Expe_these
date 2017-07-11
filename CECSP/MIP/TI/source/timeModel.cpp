#include "timeModel.hpp"



template<>
int Solve<int,int>(const Problem<int>& P,Solution<int,int> &s) {
  IloNum start,time_exec;
  const int n= P.nbTask;
  
  IloEnv env;
  IloModel model(env);
  		
  IloNumVarMatrix x(env,n);
  IloNumVarMatrix y(env,n);
  IloNumVarMatrix b(env,n);
  IloNumVarMatrix w(env,n);

  createModel(P,env,model,x,y,b,w);
  IloCplex cplex(model);
  for (int i=0;i<n;++i){
    for (int t=0;t<P.D;++t)
	model.add(IloConversion(env,b[i][t],ILOINT));
    }
  
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads,2);
  cplex.setOut(env.getNullStream());
  start= cplex.getCplexTime();
  // IloInt cpt=0;
  // cplex.use(getFirstSolInfo(env,cpt,start));
  
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
    std::cout << "Final status: \t"<< cplex.getStatus() << " en " 
	      << time_exec << std::endl;
    env.end();
    return 0;
  }
  env.end();
  return 1;
}


template<>
int modelToSol<int,int>(const Problem<int> &P, Solution<int,int> &s,IloCplex& 
			cplex,IloNumVarMatrix& x,IloNumVarMatrix& y, IloNumVarMatrix& b){
  int i,t;
  std::cout << "peut etre une erreur d'arondi\n";
  for (i=0;i<P.nbTask;++i){
    for (t=P.r(i);t<=P.smax(i);++t)
      if (cplex.getValue(x[i][t])==1)
	s.st[i]=t;
    for (t=P.emin(i)-1;t<P.d(i);++t)
      if (cplex.getValue(y[i][t])==1) 
	s.ft[i]=t+1;
    for (t=P.r(i);t<P.d(i);++t)
      s.b[i][t]=(int)((double)cplex.getValue(b[i][t])+0.5);
  }
  for (i=0;i<P.D;++i)
    s.event.push_back(i);
  return 0;
}
