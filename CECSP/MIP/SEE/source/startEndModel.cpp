#include "startEndModel.h"
#include "pre_proc.h"
#include <iostream>
#include <vector>


ILOINCUMBENTCALLBACK2(getFirstSolInfo, IloInt&, cpt, IloNum, startTime){
  if (cpt <1){
    std::cout << "Sol n°" << cpt+1 << " obj: \t" << getObjValue() << std::endl;
    std::cout << "Sol n°" << cpt+1 << " time: \t" << getCplexTime()-startTime << std::endl;
    std::cout << "Sol n°" << cpt+1 << " gap: \t" << getMIPRelativeGap() << std::endl;
    cpt=cpt+1;
  }    
}

int setCplexParam(IloCplex& cplex, IloEnv& env, int time_limit){
  //Set cplex parameter
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads, 2);
  cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::PreLinear, 0);

  return 0;
}


int LPsolveSE(const Problem<double>& P,const std::vector<int>& config,int time_limite) {  
  try{
    IloNum start,time_exec;
    const int nbTask = P.nbTask;
    const int E=2*nbTask;
    
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix x(env,nbTask),y(env,nbTask);
    IloNumVarMatrix b(env,nbTask), w(env,nbTask);
    IloNumVarArray t(env, E, 0, P.D, ILOFLOAT);
    
    // create cplex model
    createModel(P,env,model,t,x,y,b,w,config);
    for (int i=0;i<nbTask;++i){
      for (int e=0;e<E;++e){
	model.add(IloConversion(env,x[i][e],ILOFLOAT));
	model.add(IloConversion(env,y[i][e],ILOFLOAT));
      }
    }
    IloCplex cplex(model);
    setCplexParam(cplex,env,time_limite);
    start= cplex.getCplexTime();
      
    // solve !
    if (cplex.solve()) {	 
      time_exec=cplex.getCplexTime()-start;
      std::cout << "Final status: \t"<< cplex.getStatus() << std::endl;
      std::cout << "Final time: \t"<< time_exec << std::endl;
      std:: cout << "Final objective: " << cplex.getObjValue() <<"\nFinal gap: \t" 
		 << cplex.getMIPRelativeGap()<< std::endl;   
            env.end();
      return 0;
    }
    else if (cplex.getStatus()==IloAlgorithm::Infeasible){
      time_exec=cplex.getCplexTime()-start;
      std::cout << "status: "<< cplex.getStatus() << " en " << time_exec << std::endl;
    }
    env.end();
    return 1;
  }
  catch (IloException &e){
    std::cout << "Iloexception in solve" << e << std::endl;
    e.end();
    return  1;
  }
}


int solveSE(const Problem<double>& P,Solution<double,double> &s,const std::vector<int>& config) {  
  try  {
    IloNum start,time_exec;
    const int n = P.nbTask;
    
    // create cplex model
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix x(env,n),y(env,n),b(env,n), w(env,n);
    IloNumVarArray t(env, 2*n, 0, P.D, ILOFLOAT);
    createModel(P,env,model,t,x,y,b,w,config);
    IloCplex cplex(model);
    setCplexParam(cplex,env,time_limit);

    start = cplex.getCplexTime();
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
      modelToSol(P,s,cplex,t,x,y,b);
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
    std::cout << "Iloexception in solve" << e << std::endl;
    e.end();
    return 1;
  } 
  catch (...){
    std::cout << "Error unknown\n";
    return 1;
  }
}


//************************************************************
//***********************FILL SOLUTION************************
//************************************************************

int modelToSol(const Problem<double> &P,
	       Solution<double,double> &s,IloCplex& cplex,
	       IloNumVarArray& t,IloNumVarMatrix& x,
	       IloNumVarMatrix& y,IloNumVarMatrix& b){
  const int E=2*P.nbTask;
  for (int i=0;i<P.nbTask;++i){
    for (int e=0;e<E-1;++e){
      if (isEqual(cplex.getValue(x[i][e]),1.0)) s.st[i]=cplex.getValue(t[e]);
      if (isEqual(cplex.getValue(y[i][e]),1.0)) s.ft[i]=cplex.getValue(t[e]);
      if (!isEqual(cplex.getValue(t[e+1]-t[e]),0.0))
	s.b[i][e]=cplex.getValue(b[i][e])/(cplex.getValue(t[e+1]-t[e]));
      else
	s.b[i][e]=0.0;
    }
    if (isEqual(cplex.getValue(y[i][E-1]),1.0)) s.ft[i]=cplex.getValue(t[E-1]);
  }
  for (int e=0;e<E;++e)
    s.event.push_back(cplex.getValue(t[e]));
  return 0;
}


//*************************************************************
//*********************CREATION MODEL**************************
//*************************************************************


int createModel(const Problem<double>& P, IloEnv& env, IloModel& m, IloNumVarArray& t, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w,const std::vector<int>& config) {
  try {
    return createVars(P,env,x,y,b,w) || createConstraints(P,env,m,t,x,y,b,w,config);
  }
  catch (IloException& e){
    std::cout << "iloexception in createmodel" << e <<std::endl;
    e.end();
    return  1;
  }
}


int createVars(const Problem<double>& P, IloEnv& env, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w){
  try{
    int i;
    const int nbTask=P.nbTask;
    const int E= 2*nbTask;
    
    for (i=0;i<nbTask;i++){
      x[i]=IloNumVarArray(env, E, 0, 1, ILOINT);
      y[i]=IloNumVarArray(env, E, 0, 1, ILOINT);
      b[i]=IloNumVarArray(env, E-1, 0, ((P.d(i)-P.r(i))*P.bmax(i)), ILOFLOAT);
      w[i]=IloNumVarArray(env, E-1, 0, (P.d(i)-P.r(i))*P.A[i].Fi(P.bmax(i)), ILOFLOAT);
  }  
  return 0;
}
catch (IloException &e){
  std::cout << "iloexception in create vars" <<e <<std::endl;
  e.end();
  return  1;
 }
}


int createConstraints(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w,const std::vector<int>& config){
  try {
    assert(config[1]+config[2]<=1);
    std::vector<std::vector<double>> bound(2*P.nbTask-1);
    std::vector<double> bd(2*P.nbTask,P.D);
    createObj(P,env,model,b);//objective min b_ie
    createConstraintStartBeforeEnd(P,env, model,x,y);
    createConstraintOrd(P,model,t);//contrainte te < te+1
    createConstraintOneStart(P,env,model,x,y);//contrai sum zie>=1
    createConstraintCapacity(P,env,model,t,b);//contrainte capacity
    createConstraintMinDur(P,model,t,x,y);//durée minimale
    createConstraintEnergy(P,env,model,t,b,w);//contrainte Wi
    createConstraintNulEnergy(P,env,model,x,y,w);
    createConstraintBmax(P,model,t,b);//contrainte bmax
    createConstraintNonConsump(P,model,env,x,y,b,config[3]);//bie=0 si zie=0
    //additional constraints
    
    if (config[0]) {
      boundSepEvts(P,bound,config[0]);
      createConstraintSepEvt(P,model,t,bound);
      if (config[1]) 
	createConstraintBmin(P,model,env,t,x,y,b,bound); //contrainte bmin
    }
    else if (config[1]){
      boundSepEvts(P,bound,config[1]);
      createConstraintBmin(P,model,env,t,x,y,b,bound); //contrainte bmin
    }
    else 
      createConstraintBmin(P,model,env,t,x,y,b,config[2]); //contrainte bmin
    
    
    if (config[5])
      boundEvts(P,bd);
    createConstraintTimeW(P,model,t,x,y,bd);// te > ri et tf < di 
    if (config[4]) {
      if (!config[5]) 
	boundEvts(P,bd);
      createConstraintBoundEvt(P,model,t,bd);
    }
    
   if (config[6])
   createConstraintKnapsack(P,env,model,x,y);//knapsack
    return 0;
  }
  catch (IloException& e){
    std::cout << "iloexcpetion in createConstraint" << e <<std::endl;
    e.end();
    return 1;
  }
}


//*******************************************************
//********************OBJECTIVE**************************
//*******************************************************

int createObj(const Problem<double>& P,IloEnv& env, IloModel& model,
	      IloNumVarMatrix& b){
  IloExpr expr(env);
  for (int i=0;i<P.nbTask;++i) {
    for (int e=0;e<2*P.nbTask-1;++e)
      expr+=b[i][e];
  }
  model.add(IloMinimize(env,expr));
  expr.end();
  return 0;  
}

//******************************************************
//*******************CONSTRAINTS************************
//******************************************************

int createConstraintStartBeforeEnd(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarMatrix& x, IloNumVarMatrix& y){ 
  for (int i=0;i<P.nbTask;++i){
    for (int e=0; e < 2*P.nbTask;++e){
      IloExpr expr(env);
      for (int f=0;f<=e ;++ f) 
	expr += y[i][f];
      for (int f=e;f<2*P.nbTask ;++ f) 
	expr += x[i][f];
      model.add(expr <= 1);
    }
  }
  return 0;
}


int createConstraintOrd(const Problem<double>& P,IloModel& model, 
			IloNumVarArray& t){
  for (int e=0;e<2*P.nbTask-1;e++)
    model.add(t[e]<=t[e+1]); 
  return 0;
}

int createConstraintOneStart(const Problem<double>& P, IloEnv& env, 
			     IloModel &model, IloNumVarMatrix& x, IloNumVarMatrix& y) {
     for (int i=0;i<P.nbTask;i++){
      IloExpr exprX(env);
      IloExpr exprY(env);
      for (int e=0;e<2*P.nbTask;e++){
	exprX+=x[i][e];
	exprY+=y[i][e];
      }
      model.add(exprX==1);
      model.add(exprY==1);
      exprX.end();
      exprY.end();
     }
     return 0;
}

int createConstraintTimeW(const Problem<double>& P, IloModel& model,
			  IloNumVarArray& t, IloNumVarMatrix& x,  IloNumVarMatrix& y, const std::vector<double>& M_te){
  const int E=2*P.nbTask;  
  for (int i=0;i<P.nbTask;i++) {
    for (int e=0;e<E;++e){
      model.add(x[i][e]*P.r(i) <= t[e]);
      model.add(t[e] <= x[i][e]*P.smax(i) + 
		(1-x[i][e])*M_te[e]);
      model.add(P.d(i)*y[i][e] + (1-y[i][e])*M_te[e]>= t[e]);
      model.add(t[e] >= y[i][e]*P.emin(i));
    }
  }
  return 0;
}

int createConstraintCapacity(const Problem<double>& P, IloEnv& env, 
IloModel& model, IloNumVarArray& t, IloNumVarMatrix& b){
  for (int e=0;e<2*P.nbTask-1; e++){
    IloExpr expr(env);
    for (int i=0;i<P.nbTask;i++)
      expr+=b[i][e];
    model.add(expr <= P.B*(t[e+1]-t[e]));
    expr.end();
  }
  return 0;
}


int createConstraintMinDur(const Problem<double>& P, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& x, IloNumVarMatrix& y){
  const int E=2*P.nbTask;
  for (int i=0;i<P.nbTask;i++){
    for (int e=1;e<E-1;e++) {
      for (int f=e;f<E;f++)
	model.add(t[f] >= t[e] + x[i][e] * (P.A[i].emin-P.A[i].ri) - (1-y[i][f])*(P.W(i)/P.A[i].Fi(P.bmax(i))));
    }
  }
  return 0;    
}

int createConstraintNulEnergy(const Problem<double>& P, IloEnv& env,IloModel& model, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& w){
  for (int i=0;i<P.nbTask;i++){
    for (int e=0;e<2*P.nbTask-1;e++){	
      IloExpr expr(env);
      for (int f=0; f<= e;f++){
	expr+= x[i][f];
	expr-= y[i][f];
      }
      expr*=P.W(i);
      model.add(expr >= w[i][e]);
      expr.end();
    }
  }

  /*  for (int i=0;i<P.nbTask;++i){
    for (int e=0;e<2*P.nbTask-1;++e){
      model.add(w[i][e] <= z[i][e] * P.W(i));
    }
    }*/
  return 0;
}

int createConstraintEnergy(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& b, IloNumVarMatrix& w){
  for (int i=0;i<P.nbTask;++i){
    IloExpr expr(env);
    for (int e=0;e<2*P.nbTask-1;++e){
      for (int q=0;q<P.A[i].Fi.nbPiece;++q)
	model.add(P.A[i].Fi.F[q].f.a*b[i][e]+
		  P.A[i].Fi.F[q].f.c*(t[e+1]-t[e])>=w[i][e]);
      expr+=w[i][e];
    }
    model.add(expr == P.W(i));
    expr.end();
  }
  return 0;
}

int createConstraintBmin(const Problem<double>&P, IloModel& model, IloEnv& env, 
			 IloNumVarArray& t, IloNumVarMatrix& x,IloNumVarMatrix& y, IloNumVarMatrix& b, 
			 const std::vector<std::vector<double>> &M_evt){
  for (int i=0;i<P.nbTask;i++){
    for (int e=0;e<2*P.nbTask-1;e++){
      IloExpr expr(env);
      for (int f=0;f<=e;f++){
	expr-= x[i][f];
	expr+= y[i][f];
      }
      expr+=1;
      expr*=-M_evt[e][0]*P.bmin(i);
      expr+=(t[e+1]-t[e])*P.A[i].bmin;
      model.add(expr <= b[i][e]);
      expr.end();
    }
  }
  return 0;
}

int createConstraintBmin(const Problem<double>&P, IloModel& model, IloEnv& env, IloNumVarArray& t, IloNumVarMatrix& x, IloNumVarMatrix& y,IloNumVarMatrix& b, const int& bigM){
  for (int i=0;i<P.nbTask;i++){
    for (int e=0;e<2*P.nbTask-1;e++){
      IloExpr expr(env);
    for (int f=0;f<=e;f++){
      expr-= x[i][f];
      expr+= y[i][f];
    }
    expr+=1;
    if (bigM)
      expr*=-M;
    else 
      expr*= - P.bmin(i)*(P.d(i)-P.r(i));
    expr+=(t[e+1]-t[e])*P.A[i].bmin;
    model.add(expr <= b[i][e]);
    expr.end();
    }
  }
  
  return 0;
}


int createConstraintBmax(const Problem<double>&P, IloModel& model, 
			 IloNumVarArray& t, IloNumVarMatrix& b){
  for (int i=0;i<P.nbTask;i++){
    for (int e=0;e<2*P.nbTask-1;e++)
      model.add((t[e+1]-t[e])*P.bmax(i) >= b[i][e]);
  }
  return 0;
}


int createConstraintNonConsump(const Problem<double>& P, IloModel& model, IloEnv& env,
			       IloNumVarMatrix& x, IloNumVarMatrix& y , IloNumVarMatrix& b, int bigM){
  for (int i=0;i<P.nbTask;i++){
    for (int e=0;e<2*P.nbTask-1;e++){	
      IloExpr expr(env);
      for (int f=0;f<=e;f++){
	expr+= x[i][f];
	expr-= y[i][f];
      }
      if (!bigM)
	expr*=P.bmax(i)*(P.d(i)-P.r(i));
      else 
	expr*=M;
      model.add(expr >=b[i][e]);
      expr.end();
    }
  }
  return 0;
}


//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************

int createConstraintKnapsack(const Problem<double>& P, IloEnv& env, 
			     IloModel& model, IloNumVarMatrix& x, IloNumVarMatrix& y){
  for (int e=0;e<2*P.nbTask-1;++e){
    IloExpr expr(env);
    for (int i=0;i<P.nbTask;++i){   
      if (P.bmin(i)!=0)  {
	IloExpr expr2(env);
	for (int f=0;f<=e;f++){
	  expr2+=x[i][f];
	  expr2-=y[i][f];
	}
	expr+=expr2*P.bmin(i);
      }
    }
    model.add(expr<=P.B);
    expr.end();
  }
  return 0;
}
  


int createConstraintBoundEvt(const Problem<double>& P,IloModel& model, 
			     IloNumVarArray& t,const std::vector<double>& bd){
  for (int e=0;e<2*P.nbTask-1;++e)
    model.add(t[e]<=bd[e]);
  return 0;
}

int createConstraintSepEvt(const Problem<double>& P,IloModel& model, 
			   IloNumVarArray& t,const std::vector<std::vector<double>>& bound){
  for (int e=0;e<2*P.nbTask-1;++e)
    for (uint f=1;f<bound[e].size();++f)
      model.add(t[e+f]-t[e]<=bound[e][f-1]);
  return 0;
}
