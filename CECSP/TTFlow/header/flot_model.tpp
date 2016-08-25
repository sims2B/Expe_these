#include "flot_model.h"
//#include "ttdr.h"
#include <iostream>
#include <vector>
#include <algorithm>

template<typename type>
int flowERTotalTest(Problem<type> & P){
  int ERTest=intervalTotalTest(P);
  int flowTest=flowTotalTest(P);
  if (!flowTest || !ERTest)
    return 0;
  else return std::max(flowTest,ERTest);
}


template<typename type>
int solve(const Problem<type>& P) {
  const int n= P.nbTask;
  
  IloEnv env;
  IloModel model(env);
			
  IloNumVarMatrix b(env,n);
  IloNumVarMatrix w(env,n);
 
  // create cplex model
  createModel(P,env,model,b,w);
				
  IloCplex cplex(model);
  cplex.setParam(IloCplex::TiLim, flot_time_limit);
  cplex.setParam(IloCplex::Threads, 2);
  cplex.setOut(env.getNullStream());

  // solve !
  if (cplex.solve() || cplex.getStatus()==IloAlgorithm::Unknown) {
    env.end();
    return 1;
  }
  else{
    env.end();
    return 0;
  }
}

/*template<typename type>
int flowTotalTest(Problem<type>& P){
    Problem<type> Pf(P.nbTask);
  computeFreepart(P,Pf);
  std::vector<fonction<type>> TT;
  TT.resize(2*P.nbTask);
  computeTimeTable(P,TT);
  std::vector<type> pushing,pushed;
  computeSet(P,Pf,TT,pushing,pushed);
  if (solve(P)){
    if (TTDRLeft(P,Pf,TT,pushing,pushed) || TTDRRight(P,Pf,TT,pushing,pushed))
      return 2;
    else 
      return 1;
      }
  return 0;
}*/

template<typename type>
int flowTotalTest(Problem<type>& P){
  return (solve(P));
}

template<typename type>
int createModel(const Problem<type>& P, IloEnv& env, IloModel& m, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  return createVars(P,env,b,w) || createConstraints(P,env,m,b,w);
}

//TODO changer borne wiq	
template<typename type>
int createVars(const Problem<type>& P, IloEnv& env, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  int i;
  const int n=P.nbTask;
  
  for (i=0;i<n;i++){
    b[i]=IloNumVarArray(env, 4*n-1, 0, (P.d(i)-P.r(i))*P.bmax(i), ILOFLOAT);
    w[i]=IloNumVarArray(env, 4*n-1 , 0,(P.d(i)-P.r(i))*P.A[i].Fi(P.bmax(i)), ILOFLOAT);
  }
  /*
  char namevar[24];
  for (i=0;i<n;++i) {
    for (int e=0;e<4*n-1;++e) {
      sprintf(namevar,"b_%d_%d",i,e);
      b[i][e].setName(namevar);
      sprintf(namevar,"w_%d_%d",i,e);
      w[i][e].setName(namevar);
    }
  }*/
  return 0;
}
template<typename type>	
int createConstraints(const Problem<type>& P, IloEnv& env, IloModel& model, IloNumVarMatrix& b, IloNumVarMatrix& w) {	
  int i,q,k;
  std::vector<type> t;
  const int n=P.nbTask;	
  t.reserve(4*n); 

  for (int i=0;i<n;++i){
      t.push_back(P.r(i));
      t.push_back(P.d(i));
      t.push_back(P.smax(i));
      t.push_back(P.emin(i));
    }
  std::sort(t.begin(),t.end());
 
    

  //contrainte bmin
  for (i=0;i<n;++i) {
    for (q=0;q<4*n-1;++q) {
      if (t[q]>=P.smax(i) && t[q+1]<=P.emin(i))
	model.add(P.bmin(i)*(t[q+1]-t[q]) <= b[i][q]);
    }
  }
  
  //Contrainte Wi
  for (i=0;i< n;++i) {
    IloExpr expr(env);
    for (q=0;q<4*n-1;++q) 
      expr+=w[i][q];
    model.add(expr >= P.W(i));
    expr.end();
    }

  //a virer si function verifie f(0) = 0
  for (i=0;i<n;++i) {
    for (q=0;q<4*n-1;++q) 
      model.add(b[i][q]*M_flot>=w[i][q]);
  }
  
  for (i=0;i<n;++i) {
    for (q=0;q<4*n-1;++q) {
      for (k=0;k<P.A[i].Fi.nbPiece;++k)
	model.add(P.A[i].Fi.F[k].f.a*b[i][q]+P.A[i].Fi.F[k].f.c*(t[q+1]-t[q])>=w[i][q]);
    }
  }
  
  //contrainte ressource
  for (q=0;q<4*n-1;++q) {
    IloExpr expr(env);
    for (i=0;i<n;i++)
      expr+=b[i][q];
    model.add(expr <= P.B*(t[q+1]-t[q]));
    expr.end();
  }
  

  //contrainte bmax 
  for (i=0;i<n;++i) {
      for (q=0;q<4*n-1;++q) { 
	model.add(b[i][q]<= P.bmax(i)*(t[q+1]-t[q]));
      }
  }
  
  
  //contrainte bit=0 si t < start
  for (i=0;i<n;i++) { 
    for (q=0;q<4*n-1;++q) { 
      if (t[q]<P.r(i) || t[q]>=P.d(i))
	model.add(b[i][q]==0);
      
    }
  }
  
  return 0;
}

