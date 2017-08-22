#include "model_RCPSP_OOE.h"
#include "pre_proc_MIP.h"
#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#define USERCUTLIMIT 500


//Model OO: Le nombre de taches est n-1!! On oublie la tâche fictive de fin!!! On aurait pu aussi oubier celle de debut et avoir une tache de moins mais c'est chiant
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



ILOUSERCUTCALLBACK5(noPreemption_tk, const problem&, P, IloNumVarMatrix&, z, IloNum, eps, IloInt&, nbCut, IloInt&, nodeDepth) {
  if ( !isAfterCutLoop() )
    return;

  if ( nodeDepth <= 10 && nbCut < USERCUTLIMIT ) {
    const int n = P.nbTask-1;
    const int E = n;
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





ILOUSERCUTCALLBACK5(SymmetryBreak, const problem&, P, IloNumVarMatrix&, z, 
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

ILOUSERCUTCALLBACK5(noPreemption, const problem&, P, IloNumVarMatrix&, z, 
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


//********************************************************
//************************ SOLVE *************************
//********************************************************
int setCplexParam(IloCplex& cplex, IloEnv& env,int nodeLimit, int nbSol){
  //Set cplex parameter
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads, 1);
  cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::PreLinear, 0);

  if (nodeLimit != -1) 
    cplex.setParam(IloCplex::NodeLim, nodeLimit);
  if (nbSol != -1) 
    cplex.setParam(IloCplex::IntSolLim, nbSol);

  return 0;
}

int solve_OOE_LP(const problem& P, type& Obj){
  try  {
    const int n= P.nbTask-1;
    const int E=n;
    Obj= std::numeric_limits<type>::max();      
    IloEnv env;
    IloModel model(env);
    
    IloNumVarMatrix z(env,n);
    IloNumVarArray t(env, E, 0, P.A[P.nbTask-1].tw.t2, ILOFLOAT);
    IloNumVar Cmax(env,0,P.A[P.nbTask-1].tw.t2,ILOFLOAT);
    // create cplex model
    createModel(P,env,model,t,z,Cmax);

    IloCplex cplex(model);
    for (int i=0;i<n;++i){
      for (int e=0;e<E;++e)
	model.add(IloConversion(env,z[i][e],ILOFLOAT));
    }
    
    setCplexParam(cplex,env,-1,-1);
    cplex.solve();
    Obj= cplex.getObjValue();
    std::cout << "root objective value:  " << Obj << "\n";
    env.end();
    return 0;
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

int solve_OOE_MIP(const problem& P,solution &s, type& Obj, int nodeLimit, 
		  int nbSol, bool preempCutOn, bool SymCutOn) {
  try  {
    IloNum start,time_exec;
    const int nbTask = P.nbTask-1;
    const int E=nbTask;
    IloInt cptSymCut=0;
    IloInt cptCut=0;
    Obj= std::numeric_limits<type>::max();

    IloEnv env;
    IloModel modelMIP(env);
    
    IloNumVarMatrix z(env,nbTask);
    IloNumVarArray t(env, E, 0, P.A[P.nbTask-1].tw.t2, ILOFLOAT);
    IloNumVar Cmax(env,0,P.A[P.nbTask-1].tw.t2,ILOFLOAT);
    // create cplex model
    createModel(P,env,modelMIP,t,z,Cmax);
    
    IloCplex cplexMIP(modelMIP);


    if (preempCutOn){
      //creation callback
      IloInt nodeDepth=0;
      cplexMIP.use(depth(env,nodeDepth));
      cplexMIP.use(noPreemption_tk(env,P,z, 0.01, cptCut,nodeDepth));
    }
    if ( SymCutOn){
      IloInt nodeDepth=0;
      cplexMIP.use(SymmetryBreak(env,P,z, 0.01, cptSymCut,nodeDepth));
    }
			
    /*    {
      IloNum lastObjVal = -IloInfinity;
      cplexMIP.use(loggingCallback(env, -100000, lastObjVal,
				   cplexMIP.getCplexTime(), cplexMIP.getDetTime()));
				   }*/
    setCplexParam(cplexMIP,env, nodeLimit,nbSol);
    cplexMIP.setParam(IloCplex::MIPDisplay, 2);
    start = cplexMIP.getCplexTime();
    if (cplexMIP.solve()){
      time_exec=cplexMIP.getCplexTime()-start;
      //printSolCplex(env,cplexMIP,z,t);
      fillSolution(P,s,cplexMIP,t,z,Cmax);
      Obj=cplexMIP.getObjValue();
      std:: cout <<  "objective value: " << Obj <<"\nGap: " 
		 << cplexMIP.getMIPRelativeGap()<< "\n";
    }
    else {
      Obj= cplexMIP.getBestObjValue();
      std:: cout <<  "best objective value: " << Obj << "\n";
    }
    time_exec=cplexMIP.getCplexTime()-start;
    if (preempCutOn) std::cout << "number of preemp cuts: "<< cptCut << "\n";
    if (SymCutOn) std::cout << "number of Sym cuts: "<< cptSymCut << "\n";
    std:: cout << cplexMIP.getStatus() << " en : " << time_exec <<"\n";
    env.end();
    return 0;
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

int printSolCplex(const IloEnv& env,const IloCplex& cplex,const IloNumVarMatrix& z, const IloNumVarArray& t){
  IloNumArray v(env);
  for(int i=0;i<z.getSize();++i){
    cplex.getValues(v,z[i]);
    std::cout << "z["<<i<<"]="<<v<<std::endl;
  }
    cplex.getValues(v,t);
    std::cout << "t[e]="<<v<<std::endl;
  
    return 0;
}


int solveOOE(const problem& P,solution &s) {
  try{
    IloNum start,time_exec;
    const int nbTask = P.nbTask-1;
    const int E=nbTask;
      
    IloEnv env;
    IloModel model(env);
    
    IloNumVarMatrix z(env,nbTask);
    IloNumVarArray t(env, E, 0, P.A[P.nbTask-1].tw.t2, ILOFLOAT);
    IloNumVar Cmax(env,0,P.A[P.nbTask-1].tw.t2,ILOFLOAT);
    // create cplex model
    createModel(P,env,model,t,z,Cmax);

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
      fillSolution(P,s,cplex,t,z,Cmax);
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


int createModel(const problem& P, IloEnv& env, IloModel& m, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVar& Cmax) {
  try {
    return createVars(P,env,z) || createConstraints(P,env,m, t, z, Cmax);
  }
  catch (IloException& e){
    std::cout << "iloexception in createmodel" << e <<std::endl;
    e.end();
    return  1;
  }
}

int createVars(const problem& P, IloEnv& env, IloNumVarMatrix& z){
  try{
    int i;
    const int nbTask=P.nbTask-1;
    for (i=0;i<nbTask;i++)
      z[i]=IloNumVarArray(env, nbTask, 0, 1, ILOINT);     
    return 0;
  }
  catch (IloException &e){
    std::cout << "iloexception in create vars" <<e <<std::endl;
    e.end();
    return  1;
  }
}




int createConstraints(const problem& P, IloEnv& env, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVar& Cmax){
  try{
    const int nbTask=P.nbTask-1;

    std::vector<type> bound(nbTask,P.A[nbTask].tw.t2);
    std::vector<type> bd(nbTask+1,P.A[nbTask].tw.t2);
    //boundSepEvts(P,bound);
    //boundEvts(P,bd);
    
    //objectif (min makespan)
    MinMakespan(env,model,Cmax); 
    createConstraintOrd(P,model,t);
    createConstraintOneStart(P,env,model,z) ;
    createConstraintTimeW(P,model,t,z,Cmax,bd);
    createConstraintNonPreemp(P,env,model,z);
    createConstraintCapacity(P,env,model,z); 
    createConstraintDur(P,model,t,z,Cmax) ;
    createConstraintPrec(P,env,model,z) ;

    //contrainte additionelle
    //createConstraintSepEvt(P,model,t,bound);
    //createConstraintOneStartEnd(P,env,model,z);
    return 0;      
  }
  catch (IloException& e){
    std::cout << "iloexcpetion in createConstraint" << e <<std::endl;
    e.end();
    return  1;
  }
}

//*******************************************************
//********************OBJECTIVE**************************
//*******************************************************

int MinMakespan(IloEnv& env, IloModel& model,IloNumVar& Cmax){
  model.add(IloMinimize(env,Cmax)); 
  return 0;  
}

//******************************************************
//*******************CONSTRAINTS************************
//******************************************************

int createConstraintOrd(const problem& P,IloModel& model, 
			IloNumVarArray& t){
  model.add(t[0]==0);
  for (int e=0;e<P.nbTask-2;e++)
    model.add(t[e]<=t[e+1]); 
  return 0;
}


int createConstraintOneStart(const problem& P, IloEnv& env, 
			     IloModel &model, IloNumVarMatrix& z) {
  for (int i=0;i<P.nbTask-1;++i){
    IloExpr expr(env);
    for (int e=0;e<P.nbTask-1;++e)
      expr+=z[i][e];
    model.add(expr>=1);
    expr.end();
  }
  return 0;
}


int createConstraintTimeW(const problem& P, IloModel& model, IloNumVarArray& t, 
			  IloNumVarMatrix& z, IloNumVar& Cmax,const std::vector<type>& M_te){
  const int n=P.nbTask-1;
  const int E=n;
  for (int i=0;i<n;i++){
    //model.add(t[0]>=P.A[i].tw.t1*z[i][0]);
    model.add(t[0]<= P.A[i].tw.t2*z[i][0] + 
	      M_te[0]*(1-z[i][0]));
    for (int e=1;e<E;e++){
      model.add(t[e]>=P.A[i].tw.t1*z[i][e]);
      model.add(t[e]<= P.A[i].tw.t2*(z[i][e]-z[i][e-1])+ 
		M_te[e]*(1-(z[i][e]-z[i][e-1])));
    }
  }
  model.add(P.A[n].tw.t1<=Cmax);
  model.add(Cmax<=M_te[n]);
  return 0;
}


int createConstraintNonPreemp(const problem& P, IloEnv& env, 
			      IloModel& model, IloNumVarMatrix& z){
  for (int i=0;i<P.nbTask-1;i++) {
    for (int e=1;e<P.nbTask-1;e++) {
      IloExpr expr(env);
      IloExpr expr2(env);
      for (int f=0;f<e;f++)
	expr+=z[i][f];
      model.add(expr <= e*(1-(z[i][e]-z[i][e-1])));
      expr.end();
      for (int f=e+1;f<P.nbTask-1;f++)
	expr2+=z[i][f];
      model.add(expr2 <= (P.nbTask-1-e)*(1+(z[i][e]-z[i][e-1])));
      expr2.end();
    }
  }    
  return 0;
}


int createConstraintCapacity(const problem& P, IloEnv& env, IloModel& model, 
			     IloNumVarMatrix& z){
  for (uint k=0;k<P.R.size();++k){
    for (int e=0;e<P.nbTask-1;++e){
      IloExpr expr(env);
      for (int i=0;i<P.nbTask-1;i++)
	expr+=P.A[i].b[k]*z[i][e];
      model.add(expr<=P.R[k].B);
      expr.end();
    }
  }
  return 0;
}


int createConstraintDur(const problem& P, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVar& Cmax){
  const int E=P.nbTask-1;
  for (int i=0;i<P.nbTask-1;i++){
    for(int f=1;f<E;f++) 
      model.add(t[f] >= t[0] +((z[i][0])-(z[i][f]-z[i][f-1])-1)* P.A[i].p);
    for (int e=1;e<E;e++) {
    model.add(Cmax >= t[e]+(z[i][e]-z[i][e-1])*P.A[i].p);
      for (int f=e+1;f<E;f++)
	model.add(t[f] >= t[e] + 
		  P.A[i].p*((z[i][e]-z[i][e-1])-(z[i][f]-z[i][f-1])-1));
    }
  }
  return 0;    
}

int createConstraintPrec(const problem& P, IloEnv& env,IloModel& model, IloNumVarMatrix& z){ 
  uint j;
  for (int i=0;i<P.nbTask-1;++i){
    for (j=0;j<P.A[i].succ.size();++j){
      for (int e=0;e<P.nbTask-1;++e){
	IloExpr expr(env);
	for (int f=0;f<=e;++f)
	  expr= (P.A[i].succ[j]<P.nbTask-1)? expr+z[P.A[i].succ[j]][f] : expr;
	model.add(z[i][e]+expr<=1+(1-z[i][e])*e);
	expr.end();
      }
    }
  }
  return 0;
}


//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************


int createConstraintOneStartEnd(const problem& P, IloEnv& env, IloModel& model, IloNumVarMatrix& z){
  const int n=P.nbTask-1;
  IloExpr expr(env);
  IloExpr expr2(env);
  for (int i=0;i<n;++i){
    expr+=z[i][0];
    expr2+=z[i][n-1];
  }
  model.add(expr==1);
  expr.end();
  model.add(expr2==1);
  expr2.end();
  for (int e=1;e<n;++e){
    IloExpr expr(env);
    for (int i=0;i<n;++i)
      expr+=(z[i][e]-z[i][e-1]);
    model.add(expr<= 1);
    model.add(-1<= expr);
    expr.end();
  }
  return 0;
}


int createConstraintSepEvt(const problem& P,IloModel& model, IloNumVarArray& t,const std::vector<type>& bd){
  for (int e=0;e<P.nbTask-2;++e)
    model.add(t[e+1]-t[e]<=bd[e]);
  return 0;
}


//******************************************************
//*******************FILL SOLUTION************************
//******************************************************


int fillSolution(const problem &P,solution &s,IloCplex& cplex,IloNumVarArray& t,IloNumVarMatrix& z,IloNumVar& Cmax){
  const int E=P.nbTask-1;
  for (int i=0;i<P.nbTask-1;++i){
    if (cplex.getValue(z[i][0])==1) 
      s.st[i]=cplex.getValue(t[0]);
    for (int e=1;e<E;++e){
      if (cplex.getValue(z[i][e]-z[i][e-1])==1)
	s.st[i]=ceil(cplex.getValue(t[e]));
    }
  }
  s.st[P.nbTask-1]=cplex.getValue(Cmax);
  return 0;
}
