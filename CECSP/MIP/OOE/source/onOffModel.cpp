#include "onOffModel.h"
#include "pre_proc.h"
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#define USERCUTLIMIT 500
//ATTENTION: pour utiliser dans le BB, changer function solve pour
// qu'elle renvoie 0 que en cas d'optimalité

ILOMIPINFOCALLBACK4(loggingCallback,
                    IloNum,         lastLog, 
                    IloNum,         lastIncumbent,
                    IloNum,         startTime,
                    IloNum,         startDetTime)
{
  int newIncumbent = 0;
  int nodes = getNnodes();

  if ( hasIncumbent()                                  && 
       fabs(lastIncumbent - getIncumbentObjValue())
       > 1e-5*(1.0 + fabs(getIncumbentObjValue())) ) {
    lastIncumbent = getIncumbentObjValue();
    newIncumbent = 1;
  }
     
  if ( nodes >= lastLog + 100  ||  newIncumbent ) {  

    if ( !newIncumbent )  lastLog = nodes;
    getEnv().out() << "Time = " << getCplexTime() - startTime
		   << "  Dettime = " << getDetTime() - startDetTime
		   << "  Nodes = " << nodes
		   << '(' << getNremainingNodes() << ')'
		   << "  Best objective = " << getBestObjValue();

    if ( hasIncumbent() ) {
      getEnv().out() << "  Incumbent objective = " << getIncumbentObjValue()
		     << std::endl;
    }
    else {
      getEnv().out() << std::endl;
    }

  }
  if ( newIncumbent ) {
    getEnv().out() << "New incumbent found " << std::endl;
  }
}

ILONODECALLBACK1(depth, IloInt& , nodeDepth) {
  nodeDepth=getDepth(0);
}


ILOUSERCUTCALLBACK6(noPreemption_tk, const Problem<double>&, P, IloNumVarMatrix&, z, IloNum, eps, IloInt&, nbCut, IloInt&, nodeDepth, IloInt , depthMax) {
  if ( !isAfterCutLoop() )
    return;

  if ( nodeDepth <= depthMax && nbCut < USERCUTLIMIT ) {
    const int n = P.nbTask;
    const int E = 2 * n - 1;
    double *z_j = new double[E];
    int *pi = new int[E];
    int *pathPred = new int[E];
    double *F = new double[E];
    /* minValues[i][k] = the minimum value in among {x[z[j][i+1]],...,x[z[j][k]]} */
    double *minValues = new double[E*E];
    int *minIdx = new int[E*E];

    for(int j = 0; j<n && nbCut <= USERCUTLIMIT; ++j) {
      // read variable values
      for(int e = 0; e<E; ++e) {
	z_j[e] = getValue(z[j][e]);
      }
      bool started = false;
      bool feasible = true;
      // check feasibility
      for(int e=0; e<E && feasible; ++e) {
	if( z_j[e] >= 0.00001 && z_j[e] <= 0.99999 ) {
	  feasible = false;
	  break;
	}
	if( z_j[e] >= 0.99999 && 0 == e ) started = true;
	if( e>= 1 && z_j[e] >= 0.99999 && z_j[e-1] < 0.00001 ) {
	  if( started ) {
	    feasible = false;
	    break;
	  }
	  else {
	    started = true;
	  }
	}
      }

      if( feasible ) continue;

      double maxCost = 1.0;
      // this piece of code determines the minimum value and the corresponding index 
      for(int e=0; e<E; ++e) {
	double m = std::numeric_limits<double>::max();
	int idx = -1;
	for(int k=e+1; k<E; ++k) {
	  double z_jk = z_j[k];
	  if( m > z_jk ) {
	    idx = k;
	    m = z_jk;
	  }

	  minValues[E*e+k] = m; 
	  minIdx[E*e+k] = idx;
	}
      }

      for(int e=0; e<E; ++e) {
	pathPred[e] = -1;
      }
      F[0] = F[1] = 0;
      for(int k=2; k<E; ++k) {
	F[k] = 0;
	for(int e=0; e<k-2; ++e) {
	  double arccost = z_j[e] - minValues[E*e + k-1];
	  if( F[e] + arccost > F[k] ) {
	    F[k] = F[e] + arccost;
	    pathPred[k] = e;
	  }
	}
	if( F[k] + z_j[k] > maxCost + 0.001 ) {
	  memset(pi, 0, E*sizeof(int));
	  maxCost = F[k] + z_j[k];
	  if( pathPred[k] != -1 ) {
	    int t=k;
	    for( ; pathPred[t] != -1; t = pathPred[t]) {
	      int i = pathPred[t];
	      pi[t] = 1;
	      pi[minIdx[E*i + t-1]] =-1;
	    }
	    pi[t] = 1;
	  }
	}
      }

      if( maxCost > 1.0 + eps) {
	// addCut
	IloExpr lhs(getEnv());
	for(int e=0; e<E; ++e) {
	  lhs += pi[e] * z[j][e];
	}
	IloNum rhs = 1.0;

	IloRange cut;
	try{
	  cut= (lhs <= rhs);
	  ++nbCut;
	  add(cut, IloCplex::UseCutPurge).end();
	}
	catch (...){
	  cut.end();
	  throw;
	}

      }
    }
    ///		std::cerr << "total number of cuts added: " << nbCut << std::endl;
    delete [] minIdx;
    delete [] minValues;
    delete [] F;
    delete [] pathPred;
    delete [] pi;
    delete [] z_j;
  }
}



ILOINCUMBENTCALLBACK2(getFirstSolInfo, IloInt&, cpt, IloNum, startTime){
  if (cpt <1){
    std::cout << "Sol n°" << cpt+1 << " obj: \t" << getObjValue() << std::endl;
    std::cout << "Sol n°" << cpt+1 << " time: \t" << getCplexTime()-startTime << std::endl;
    std::cout << "Sol n°" << cpt+1 << " gap: \t" << getMIPRelativeGap() << std::endl;
    cpt=cpt+1;
  }
    
}

//********************************************************
//************************ SOLVE *************************
//********************************************************
int setCplexParam(IloCplex& cplex, IloEnv& env, int time_limit){
  //Set cplex parameter
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads, 2);
  cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::PreLinear, 0);

  return 0;
}

int LPsolveOO(const Problem<double>& P,const std::vector<double>& bound,const std::vector<double>& bd,bool addTe,bool addSep, bool addPreem, bool addKnapsack, int time_limit){
    try  {
      /*  IloNum start,time_exec;
    const int n = P.nbTask;
    
    // create cplex model
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix z(env,n),b(env,n), w(env,n);
    IloNumVarArray t(env, 2*n, 0, P.D, ILOFLOAT);
    
    createOOModel(P,env,model,t,z,b,w,bound,bd,addTe,addSep,addKnapsack);
    IloCplex cplex(model);
    for (int i=0;i<n;++i){
      for (int e=0;e<2*n-1;++e)
	model.add(IloConversion(env,z[i][e],ILOFLOAT));
    }
    setCplexParam(cplex,env,time_limit);
    start= cplex.getCplexTime();
    if (addPreem){
      IloInt cptCut=0;
      IloInt nodeDepth=0;
      cplex.use(noPreemption_tk(env,P,z, 0.01, cptCut,nodeDepth,10));
    }
    
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
    env.end();*/
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

int solveOO(const Problem<double>& P,Solution<double,double> &s, double& time){
  try  {
    const int n = P.nbTask;
    IloNum start,time_exec;

    // create cplex model
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix z(env,n),b(env,n), w(env,n);
    IloNumVarArray t(env, 2*n, 0, P.D, ILOFLOAT);
    createOOModel_BB(P,env,model,t,z,b,w);
    IloCplex cplex(model);
    
    setCplexParam(cplex,env,time_limit);
    cplex.setParam(IloCplex::EpRHS,0.00001);
        
    start=cplex.getCplexTime();
    // solve !
    if (cplex.solve()) {	 
      time_exec=cplex.getCplexTime()-start;
      time+=(double)time_exec;
      modelToSol(P,s,cplex,t,z,b);
      env.end();
      return 0;
    }
    else {
      time_exec=cplex.getCplexTime()-start;
      time+=(double)time_exec;
      env.end();
      return 1;
    }
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


int solveOO(const Problem<double>& P,Solution<double,double> &s, double& time, int nbThread,int output, int time_limite){
  try  {
    const int n = P.nbTask;
    IloNum start,time_exec;

    // create cplex model
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix z(env,n),b(env,n), w(env,n);
    IloNumVarArray t(env, 2*n, 0, P.D, ILOFLOAT);
    createOOModel_BB(P,env,model,t,z,b,w);
    IloCplex cplex(model);
    
  cplex.setParam(IloCplex::TiLim, time_limite);
  cplex.setParam(IloCplex::Threads, nbThread);
  if (!output)
    cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::EpRHS,0.00001);
        
    start=cplex.getCplexTime();
    // solve !
    if (cplex.solve()) {	 
      time_exec=cplex.getCplexTime()-start;
      time+=(double)time_exec;
      modelToSol(P,s,cplex,t,z,b);
      env.end();
      return 0;
    }
    else {
      time_exec=cplex.getCplexTime()-start;
      time+=(double)time_exec;
      env.end();
      return 1;
    }
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


int solveOO(const Problem<double>& P,Solution<double,double> &s,const std::vector<int>& config){
  try  {
    IloNum start,time_exec;
    const int n = P.nbTask;
    IloInt cptCut=0;

    // create cplex model
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix z(env,n),b(env,n), w(env,n);
    IloNumVarArray t(env, 2*n, 0, P.D, ILOFLOAT);
    createOOModel(P,env,model,t,z,b,w,config);
    IloCplex cplex(model);


    if (config[0]){
      //creation callback
      IloInt nodeDepth=0;
      cplex.use(depth(env,nodeDepth));
      cplex.use(noPreemption_tk(env,P,z, 0.01, cptCut,nodeDepth,10));
    }
    
    setCplexParam(cplex,env,time_limit);
    cplex.setParam(IloCplex::MIPDisplay, 2);
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
      if (config[0]) std::cout << "number of preemp cuts: "
			       << cptCut << "\n";
      modelToSol(P,s,cplex,t,z,b);
      env.end();
      return 0;
    }
    else if (cplex.getStatus()==IloAlgorithm::Infeasible){
      time_exec=cplex.getCplexTime()-start;
      if (config[0]) std::cout << "number of preemp cuts: "
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




//************************************************************
//***********************FILL SOLUTION************************
//************************************************************


int modelToSol(const Problem<double> &P,Solution<double,double> &s,IloCplex& cplex,IloNumVarArray& t,IloNumVarMatrix& z,IloNumVarMatrix& b) {
  const int E=2*P.nbTask;
  for (int i=0;i<P.nbTask;++i) {
    if (isEqual(cplex.getValue(z[i][0]),1.0))
      s.st[i]=cplex.getValue(t[0]);
    if (isEqual(cplex.getValue(z[i][E-2]),1.0)) 
      s.ft[i]=cplex.getValue(t[E-1]);

    for (int e=1;e<E-1;++e) {
      if (isEqual(cplex.getValue((z[i][e]-z[i][e-1])),1.0)) 
	s.st[i]=cplex.getValue(t[e]);
      if (isEqual(cplex.getValue((z[i][e-1]-z[i][e])),1.0))
	s.ft[i]=cplex.getValue(t[e]);
    }
    for (int e=0;e<E-1;++e) {	  
      if (!isEqual(cplex.getValue(t[e+1]-t[e]),0.0))
	s.b[i][e]=cplex.getValue(b[i][e])/(cplex.getValue(t[e+1]-t[e]));
      else
	s.b[i][e]=0.0;
    }
  }
  for (int e=0;e<E;++e)
    s.event.push_back(cplex.getValue(t[e]));

  return 0;
}


//*************************************************************
//*********************CREATION MODEL**************************
//*************************************************************

int createOOModel_BB(const Problem<double>& P, IloEnv& env, IloModel& m, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  try {
    return createOOVars(P,env,z,b,w) || createOOConstraints(P,env,m,t,z,b,w);
  }
  catch (IloException& e){
    std::cout << "iloexception in createmodel" << e <<std::endl;
    e.end();
    return  1;
  }
}
int createOOModel(const Problem<double>& P, IloEnv& env, IloModel& m, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix& w,const std::vector<int>& config) {
  try {
    return createOOVars(P,env,z,b,w) || createOOConstraints(P,env,m,t,z,b,w,config);
  }
  catch (IloException& e){
    std::cout << "iloexception in createmodel" << e <<std::endl;
    e.end();
    return  1;
  }
}

//************************************************************
//********************CREATION VARIABLE***********************
//************************************************************

int createOOVars(const Problem<double>& P, IloEnv& env, IloNumVarMatrix& z, IloNumVarMatrix& b,IloNumVarMatrix &w){
  try{
    int i;
    const int nbTask=P.nbTask;
    const int E= 2*nbTask;

    for (i=0;i<nbTask;i++) {
      assert((P.d(i)-P.r(i))*P.A[i].Fi(P.bmax(i)) - P.W(i)>= NEGATIVE_ZERO );
      z[i]=IloNumVarArray(env, E-1, 0, 1, ILOINT);
      b[i]=IloNumVarArray(env, E-1, 0, (P.d(i)-P.r(i))*P.bmax(i), ILOFLOAT);
      w[i]=IloNumVarArray(env, E-1, 0, (P.d(i)-P.r(i))*P.A[i].Fi(P.bmax(i)), ILOFLOAT);}
   
 
    char namevar[24];
    for (int i=0;i<P.nbTask;i++)
      for (int t=0;t<E-1;t++) {
	sprintf(namevar,"z_%d_%d",i,t);
	z[i][t].setName(namevar);
	sprintf(namevar,"b_%d_%d",i,t);
	b[i][t].setName(namevar);
      }
		
    return 0;
  }
  catch (IloException &e) {
    std::cout << "iloexception in create vars" << e <<std::endl;
    e.end();
    return 1;
  }
}



//****************************************************
//****************MODEL CONSTRAINTS*******************
//****************************************************

int createOOConstraints(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix &w) {
  try {
    createObj(P,env,model,b);//objective min b_ie
    createConstraintOrd(P,model,t);//contrainte te < te+1
    createConstraintOneStart(P,env,model,z);//contrainte sum zie>=1
    createConstraintTimeW(P,model,t,z);// te > ri et tf < di
    createConstraintNonPreemp(P,env,model,z);//non preemption
    createConstraintCapacity(P,env,model,t,b);//contrainte capacity
    createConstraintMinDur(P,model,t,z);//durée minimale
    createConstraintEnergy(P,env,model,t,b,w);//contrainte Wi
    createConstraintBmin(P,model,t,z,b,0); //contrainte bmin
    createConstraintBmax(P,model,t,b);//contrainte bmax
    createConstraintNonConsump(P,model,z,b,0);//bie=0 si zie=0
    return 0;
  }
  catch (IloException& e){
    std::cout << "iloexcpetion in createConstraint" << e <<std::endl;
    e.end();
    return 1;
  }
}

int createOOConstraints(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, IloNumVarMatrix &w,const std::vector<int>& config) {
  try {
    assert(config[2]+config[3]<=1);
    std::vector<std::vector<double>> bound(2*P.nbTask-1);
    std::vector<double> bd(2*P.nbTask,P.D);
    createObj(P,env,model,b);//objective min b_ie
    createConstraintOrd(P,model,t);//contrainte te < te+1
    createConstraintOneStart(P,env,model,z);//contrai sum zie>=1
    createConstraintNonPreemp(P,env,model,z);//non preemption
    createConstraintCapacity(P,env,model,t,b);//contrainte capacity
    createConstraintMinDur(P,model,t,z);//durée minimale
    createConstraintEnergy(P,env,model,t,b,w);//contrainte Wi
    createConstraintBmax(P,model,t,b);//contrainte bmax
    createConstraintNonConsump(P,model,z,b,config[4]);//bie=0 si zie=0
    
    //additional constraints
    
    if (config[1]) {
      boundSepEvts(P,bound,config[1]);
      createConstraintSepEvt(P,model,t,bound);
      if (config[2]) 
	createConstraintBmin(P,model,t,z,b,bound); //contrainte bmin
    }
    else if (config[2]){
      boundSepEvts(P,bound,config[2]);
      createConstraintBmin(P,model,t,z,b,bound); //contrainte bmin
    }
    else 
      createConstraintBmin(P,model,t,z,b,config[3]); //contrainte bmin
    
    if (config[6])
      boundEvts(P,bd);
    createConstraintTimeW(P,model,t,z,bd);// te > ri et tf < di 
    if (config[5]) {
      if (!config[6]) 
	boundEvts(P,bd);
      createConstraintBoundEvt(P,model,t,bd);
    }
    
   if (config[7])
     createConstraintKnapsack(P,env,model,z);//knapsack
   else if (config[8])  
     createConstraintKnapsackExt(P,env,model,t,z,b);//knapsack extended
 
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

int createConstraintOrd(const Problem<double>& P,IloModel& model, 
			IloNumVarArray& t){
  for (int e=0;e<2*P.nbTask-1;e++)
    model.add(t[e]<=t[e+1]); 
  return 0;
}

int createConstraintOneStart(const Problem<double>& P, IloEnv& env, 
			     IloModel &model, IloNumVarMatrix& z) {
  for (int i=0;i<P.nbTask;i++) {
    IloExpr expr(env);
    for (int e=0;e<2*P.nbTask-1;e++)
      expr+=z[i][e];
    model.add(expr >= 1);
    expr.end();
  }
  return 0;
}

int createConstraintTimeW(const Problem<double>& P, IloModel& model,
			  IloNumVarArray& t, IloNumVarMatrix& z, const std::vector<double>& M_te){
  const int E=2*P.nbTask;  
  for (int i=0;i<P.nbTask;i++) {
    model.add(z[i][0]*P.r(i) <= t[0]);
    for (int e=1;e<E-1;++e){
      model.add(z[i][e]*P.r(i) <= t[e]);
      model.add(t[e] <= (z[i][e]-z[i][e-1])*P.smax(i) + 
		(1-(z[i][e]-z[i][e-1]))*M_te[e]);
      model.add(P.d(i)*(z[i][e-1]-z[i][e]) + 
		(1-(z[i][e-1]-z[i][e]))*M_te[e]>= t[e]);
      model.add(t[e] >= (z[i][e-1]-z[i][e])*P.emin(i));
    }
    model.add(P.d(i)*z[i][E-2] + 
	      (1-z[i][E-2])*P.D >= t[E-1]);
    model.add(t[E-1] >= z[i][E-2]*P.emin(i));
  }
  return 0;
}

int createConstraintTimeW(const Problem<double>& P, IloModel& model,
			  IloNumVarArray& t, IloNumVarMatrix& z){
  const int E=2*P.nbTask;  
  for (int i=0;i<P.nbTask;i++) {
    model.add(z[i][0]*P.r(i) <= t[0]);
    for (int e=1;e<E-1;++e){
      model.add(z[i][e]*P.r(i) <= t[e]);
      model.add(t[e] <= (z[i][e]-z[i][e-1])*P.smax(i) + 
		(1-(z[i][e]-z[i][e-1]))*P.D);
      model.add(P.d(i)*(z[i][e-1]-z[i][e]) + 
		(1-(z[i][e-1]-z[i][e]))*P.D>= t[e]);
      model.add(t[e] >= (z[i][e-1]-z[i][e])*P.emin(i));
    }
    model.add(P.d(i)*z[i][E-2] + 
	      (1-z[i][E-2])*P.D >= t[E-1]);
    model.add(t[E-1] >= z[i][E-2]*P.emin(i));
  }
  return 0;
}

int createConstraintNonPreemp(const Problem<double>& P, IloEnv& env, 
			      IloModel& model, IloNumVarMatrix& z){
  const int E=2*P.nbTask;
  for (int i=0;i<P.nbTask;i++) {
    for (int e=1;e<E-1;e++) {
      IloExpr expr(env);
      IloExpr expr2(env);
      for (int f=0;f<e;f++)
	expr+=z[i][f];
      model.add(expr <= e*(1-(z[i][e]-z[i][e-1])));
      expr.end();
      for (int f=e;f<E-1;f++)
	expr2+=z[i][f];
      model.add(expr2 <= (E-e)*(1+(z[i][e]-z[i][e-1])));
      expr2.end();
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


int createConstraintMinDur(const Problem<double>& P, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z){
  const int E=2*P.nbTask;
  for (int i=0;i<P.nbTask;i++){
    for (int e=1;e<E-1;e++) {
      for (int f=e+1;f<E-1;f++)
	model.add(t[f] >= t[e] + 
		  ((z[i][e]-z[i][e-1])-(z[i][f]-z[i][f-1]) -1 ) 
		  * (P.W(i)/P.A[i].Fi(P.bmax(i))));
    }
  }
  return 0;    
}

int createConstraintEnergy(const Problem<double>& P, IloEnv& env, IloModel& model, 
			   IloNumVarArray& t, 
			   IloNumVarMatrix& b, IloNumVarMatrix& w){
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

int createConstraintBmin(const Problem<double>&P, IloModel& model, 
			 IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, 
			 const std::vector<std::vector<double>> &M_evt){
  for (int i=0;i<P.nbTask;i++){
    for (int e=0;e<2*P.nbTask-1;e++)
      model.add((t[e+1]-t[e])*P.bmin(i)-P.bmin(i)*M_evt[e][0]*(1-z[i][e]) <= b[i][e]);
  }
  return 0;
}

int createConstraintBmin(const Problem<double>&P, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, const int& bigM){
  for (int i=0;i<P.nbTask;i++){
    for (int e=0;e<2*P.nbTask-1;e++)
      if (bigM)
	model.add((t[e+1]-t[e])*P.bmin(i)-M*(1-z[i][e]) <= b[i][e]);
    else
      model.add((t[e+1]-t[e])*P.bmin(i)-(P.bmin(i)*(P.d(i)-P.r(i))*(1-z[i][e])) <= b[i][e]);
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


int createConstraintNonConsump(const Problem<double>& P, IloModel& model, 
			       IloNumVarMatrix& z , IloNumVarMatrix& b, int bigM){
  if (!bigM){
    for (int i=0;i<P.nbTask;i++){
      for (int e=0;e<2*P.nbTask-1;e++)
	model.add(z[i][e]*(P.bmax(i)*(P.d(i)-P.r(i))) >= b[i][e]);
    }
  }
  else{
    for (int i=0;i<P.nbTask;i++){
      for (int e=0;e<2*P.nbTask-1;e++)
	model.add(z[i][e]*M>= b[i][e]);
    }
  }
  return 0;
}


//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************

int createConstraintKnapsack(const Problem<double>& P, IloEnv& env, 
			     IloModel& model, IloNumVarMatrix& z){
  for (int e=0;e<2*P.nbTask-1;++e){
    IloExpr expr(env);
    for (int i=0;i<P.nbTask;++i){
      if (P.bmin(i)!=0)
	expr+=z[i][e]*P.bmin(i);
    }
    model.add(expr<=P.B);
    expr.end();
  }
  return 0;
}


int createConstraintKnapsackExt(const Problem<double>& P, IloEnv& env, 
				IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, 
IloNumVarMatrix& b){
  for (int e=0;e<2*P.nbTask-1;++e){
    for (int i=0;i<P.nbTask;++i){
      IloExpr expr(env);
      for (int j=0; j<P.nbTask;++j){
	if (j!=i)
	  expr+=z[j][e]*P.bmin(j);
      }
      model.add(b[i][e]+expr<=P.B * (t[e+1]-t[e]));
      expr.end();
    }
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
/*
int createConstraintOneStartEnd(const Problem<double>& P, IloEnv& env,
				IloModel& model, IloNumVarMatrix& z){
  const int n=P.nbTask;
  IloExpr expr(env);
  IloExpr expr2(env);
  for (int i=0;i<n;++i){
    expr+=z[i][0];
    expr2+=z[i][2*n-2];
  }
  model.add(expr==1);
  expr.end();
  model.add(expr2==1);
  expr2.end();
  for (int e=1;e<2*n-1;++e){
    IloExpr expr(env);
    for (int i=0;i<n;++i)
      expr+=(z[i][e]-z[i][e-1]);
    model.add(expr<= 1);
    model.add(-1<= expr);
    expr.end();
  }
  return 0;
}
*/
/*
int createConstraintInterStart(const Problem<double>& P,IloEnv& env, IloModel& model, IloNumVarMatrix& z){
  for (int i=0;i<P.nbTask;++i){
    for (int j=i+1;j<P.nbTask;++j){
      if (P.smax(j) < P.r(i)){
	for (int e=1;e<2*P.nbTask-1;++e){
	  IloExpr expr(env);
	  for (int f=0;f<=e;++f)
	    expr+= z[i][f];
	  model.add(expr <= e*(1-(z[j][e]-z[j][e-1])));
	}
      }
      if (P.smax(i)< P.r(j)){
	for (int e=1;e<2*P.nbTask-1;++e){
	  IloExpr expr(env);
	  for (int f=0;f<=e;++f)
	    expr+= z[j][f];
	  model.add(expr <= e*(1-(z[i][e]-z[i][e-1])));
	}
      }
    }
  }
  return 0;
  }


int createConstraintInterEnd(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarMatrix& z){
  const int E=2*P.nbTask;
  for (int i=0;i<P.nbTask;++i){
    for (int j=i+1;j<P.nbTask;++j){
      if (P.emin(i) > P.d(j)){
	for (int e=1;e<E-1;++e){
	  IloExpr expr(env);
	  for (int f=e;f<E-1;++f)
	    expr+= z[j][f];
	  model.add(expr <= (E-e)*(1-(z[i][e-1]-z[i][e])));
	}
      }
      if (P.emin(j)> P.d(i)){
	for (int e=1;e<E-1;++e){
	  IloExpr expr(env);
	  for (int f=e;f<E-1;++f)
	    expr+= z[i][f];
	  model.add(expr <= (E-e)*(1-(z[j][e-1]-z[j][e])));
	}
      }
    }
  }
  return 0;
}*/

//************************************************************
//************************CUTS********************************
//************************************************************


/*

ILOUSERCUTCALLBACK5(SymmetryBreak, const Problem<double>&, P, IloNumVarMatrix&, z, 
		    IloNum, eps, IloInt&, cpt  , IloInt&, nodeDepth){
  if ( !isAfterCutLoop() )
    return;
  if ( nodeDepth <= 10 && cpt <=USERCUTLIMIT ) {
    IloInt n=z.getSize();
    IloInt m;
    for (IloInt i=0; i< n ;++i){
      m=z[i].getSize();
      for (IloInt e=1;e<m;++e){
	double val=0.0;
	IloExpr expr(getEnv());
	for (IloInt j=0;j<n;++j){
	  val=(j!=i)? val+getValue(z[j][e])-getValue(z[j][e-1]) : val;
	  expr=(j!=i)? expr+z[j][e]-z[j][e-1] : expr;
	}
	if(getValue(z[i][e])-getValue(z[i][e-1])+ val > 1.0+eps){
	  IloRange cut;
	  try {
	    cut= (z[i][e]-z[i][e-1]+expr <= 1.0);
	    cpt++;
	    add(cut).end();
	  }
	  catch (...){
	    cut.end();
	    throw;
	  }
	}
	if(getValue(z[i][e])-getValue(z[i][e-1])- val > 1.0+eps){
	  IloRange cut;
	  try {
	    cut= (z[i][e]-z[i][e-1]-expr <= 1.0);
	    cpt++;
	    add(cut).end();
	  }
	  catch (...){
	    cut.end();
	    throw;
	  }
	}
	if(getValue(z[i][e-1])-getValue(z[i][e])+val > 1.0+eps){
	  IloRange cut;
	  try {
	    cut= (z[i][e-1]-z[i][e]+expr <= 1.0);
	    cpt++;
	    add(cut).end();
	  }
	  catch (...){
	    cut.end();
	    throw;
	  }
	}
	if(getValue(z[i][e-1])-getValue(z[i][e])-val > 1.0+eps){
	  IloRange cut;
	  try {
	    cut= (z[i][e-1]-z[i][e]-expr <= 1.0);
	    cpt++;
	    add(cut).end();
	  }
	  catch (...){
	    cut.end();
	    throw;
	  }
	}
	expr.end();
      }
    }
  }
}

ILOUSERCUTCALLBACK5(noPreemption, const Problem<double>&, P, IloNumVarMatrix&, z, 
		    IloNum, eps, IloInt&, nbCut, IloInt&, nodeDepth) {
  if ( !isAfterCutLoop() )
    return;

  if ( nodeDepth <= 10 && nbCut < USERCUTLIMIT ) {
    IloInt n=z.getSize();
    IloInt m;
    for (IloInt i=0; i< n ;++i){
      m=z[i].getSize();
      for (IloInt e=0;e<m;++e){
	for (IloInt f=e+1;f<m;++f){
	  for (IloInt g=f+1;g<m;++g){
	    if (getValue(z[i][e])-getValue(z[i][f])+getValue(z[i][g])>1.0+eps){
	      IloRange cut;
	      try{
		cut= (z[i][e]-z[i][f]+z[i][g] <= 1.0);
		++nbCut;
		add(cut).end();
	      }
	      catch (...){
		cut.end();
		throw;
	      }
	    }
	  }
	}
      }
    }
  }
}

*/
