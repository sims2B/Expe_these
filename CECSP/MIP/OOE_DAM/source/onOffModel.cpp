#include "onOffModel.h"
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>


//*******************************************************
//********************COMPUTE BOUND**********************
//*******************************************************
bool intervalComp(const Interval<double> &i,const Interval<double> &j) { 
  return (i.t1<j.t1 || (i.t1==j.t1 && i.t2<j.t2)); 
}


int boundEvts(const Problem<double,double>& P, std::vector<double>& bd){
  const int n=P.nbTask;
  std::vector<double> I(2*n);
  for (int i=0;i<n;++i){
    I[2*i]=P.smax(i);
    I[2*i+1]=P.d(i);
  }
  std::sort(I.begin(),I.end());

  bd.resize(2*n);
  for (uint i = 0 ; i < bd.size() - 1 ; ++i) 
    bd[i] = I[i+1];
  bd[2*n-1] = P.D;
  return 0;
}

int boundSepEvts(const Problem<double,double>& P,std::vector<double>& bound){
  std::vector<Interval<double>> I(2*P.nbTask,Interval<double>(0,0));
  for (int i=0;i<P.nbTask;++i){
    I[2*i]=Interval<double>(P.r(i),P.smax(i));
    I[2*i+1]=Interval<double>(P.emin(i),P.d(i));
  }
  std::sort(I.begin(),I.end(),intervalComp);

  bound.resize(2*P.nbTask-1);
  for (uint e = 0 ; e < bound.size() ; ++e){
    bound[e] = std::max(I[e+1].t2,I[e].t2) - I[e].t1;
  }
  return 0;
}


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
  IloInt64 nextNode = 0;
  nodeDepth=getDepth(nextNode);
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
    /*std::cout << "Sol n°" << cpt+1 << " obj: \t" << getObjValue() << std::endl;
    std::cout << "Sol n°" << cpt+1 << " time: \t" << getCplexTime()-startTime << std::endl;
    std::cout << "Sol n°" << cpt+1 << " gap: \t" << getMIPRelativeGap() << std::endl;*/
    std::cout << getObjValue() << ";" << getCplexTime()-startTime << ";"
	      << getMIPRelativeGap() << ";";
    cpt=cpt+1;
  }
    
}

ILOLAZYCONSTRAINTCALLBACK4(EminSmaxConstraint, const Problem<double>&, P, IloNumVarMatrix&, z,
			  IloNumVarArray&, t, const std::vector<double>&, M_te){
  const int E = 2*P.nbTask;
  for (int i = 0 ; i < P.nbTask ; i++) {
    IloConstraint cut;
    for (int e = 1 ; e < E-1 ; ++e){
      int bound_9 =  std::max( std::min (P.D , M_te[e]),
			       std::min ( (M_te[e] + P.smax(i))/2 ,
					  (P.D + P.smax(i))/2 ));
     
      cut = (t[e] <= (z[i][e]-z[i][e-1])*P.smax(i) + 
	     (1-(z[i][e]-z[i][e-1]))* bound_9);
      add(cut, IloCplex::UseCutPurge).end();

      cut = (t[e] >= (z[i][e-1]-z[i][e])*P.emin(i));
      add(cut, IloCplex::UseCutPurge).end();
    }   
      
    cut = (t[E-1] >= z[i][E-2]*P.emin(i));
    add(cut, IloCplex::UseCutPurge).end();
  }
}

//********************************************************
//************************ SOLVE *************************
//********************************************************
int setCplexParam(IloCplex& cplex, IloEnv& env){
  //Set cplex parameter
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads, 1);
  cplex.setOut(env.getNullStream());
  cplex.setParam(IloCplex::PreLinear, 0);
  cplex.setParam(IloCplex::CutsFactor, 1);
  cplex.setParam(IloCplex::MIPDisplay, 2);

  return 0;
}


int LPsolveOO(const Problem<double>& P,const std::vector<int>& config){
    try  {
      const int n = P.nbTask;
      IloEnv env;
      IloModel model(env);
      IloNumVarMatrix z(env,n),b(env,n), w(env,n);
      IloNumVarArray t(env, 2*n, 0, P.D, ILOFLOAT);
    
      createOOModel(P,env,model,t,z,b,w,config);
      IloCplex cplex(model);
      for (int i=0;i<n;++i){
	for (int e=0;e<2*n-1;++e)
	  model.add(IloConversion(env,z[i][e],ILOFLOAT));
      }
      setCplexParam(cplex,env);
      
      if (config[0]){
	IloInt cptCut=0;
	IloInt nodeDepth=0;
	cplex.use(noPreemption_tk(env,P,z, 0.01, cptCut, nodeDepth, 10));
      }
    
      std::vector<double> bd(2*P.nbTask,P.D);
      if (config[4]) boundEvts(P,bd);
      cplex.use(EminSmaxConstraint(env,P,z,t,bd));
    
    // solve !
    if (cplex.solve()) {	 
      std::cout << cplex.getObjValue() << ";";   
      env.end();
      return 0;
    }
    else if (cplex.getStatus()==IloAlgorithm::Infeasible)
      std::cout << " ; ; ; ; ; ; ; ; ; ; ; ; \n";
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


int solveOO(const Problem<double>& P,Solution<double,double> &s,const std::vector<int>& config){
  try  {
    IloNum start,time_exec;
    const int n = P.nbTask;
    IloInt cptCut = 0;
    IloInt cpt=0;

    // create cplex model
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix z(env,n),b(env,n), w(env,n);
    IloNumVarArray t(env, 2*n, 0, P.D, ILOFLOAT);
    createOOModel(P,env,model,t,z,b,w,config);
    IloCplex cplex(model);


    if (config[0]){
      //creation callback
      IloInt nodeDepth = 0;
      cplex.use(depth(env,nodeDepth));
      cplex.use(noPreemption_tk(env,P,z, 0.01, cptCut,nodeDepth,10));
    }
    
    setCplexParam(cplex,env);
    start = cplex.getCplexTime();
    cplex.use(getFirstSolInfo(env,cpt,start));

    std::vector<double> bd(2*P.nbTask,P.D);
    if (config[4]) boundEvts(P,bd);
    cplex.use(EminSmaxConstraint(env,P,z,t,bd));

    // solve !
    if (cplex.solve() || cplex.getStatus()==IloAlgorithm::Infeasible) {	 
      time_exec=cplex.getCplexTime()-start;
      displayCVS(cplex,config,time_exec,cptCut);
      if (cplex.getStatus()==IloAlgorithm::Infeasible){
	env.end();
	return 1;
      }
      else{
	modelToSol(P,s,cplex,t,z,b);
	env.end();
	return 0;
      }
    }
    else{
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

void displayCVS(const IloCplex& cplex, const std::vector<int>& config, const IloNum&  time_exec, const int& cptCut){
  std::cout << time_exec << ";";
  if (cplex.getStatus()==IloAlgorithm::Infeasible)
    std::cout << " ; ; ; ; ; ; ; ; \n";
  else{
    std::cout << "1;1;";
    if (cplex.getStatus()==IloAlgorithm::Optimal)
      std::cout << "1;";
    else std::cout << "0;";
    std::cout << cplex.getObjValue() << ";"
	      << cplex.getBestObjValue() << ";" << 100*cplex.getMIPRelativeGap()
	      << ";" <<cplex.getNnodes()<< ";" << cptCut << ";" ; 
  }
}

void displayCplexResults(const IloCplex& cplex, const std::vector<int>& config, const IloNum&  time_exec, const int& cptCut){
  
  std::cout << "Final status: \t"<< cplex.getStatus() << " en " 
	    << time_exec << std::endl;
  if (cplex.getStatus()!=IloAlgorithm::Infeasible){
    std:: cout << "Final objective: " << cplex.getObjValue() 
	       <<"\nFinal gap: \t" << cplex.getMIPRelativeGap()
	       << std::endl;  
    std:: cout << "Number of nodes: " << cplex.getNnodes()  
	       << std::endl;   }
  if (config[0]) std::cout << "number of preemp cuts: "
			   << cptCut << "\n";
      
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


int createOOConstraints(const Problem<double>& P, IloEnv& env, IloModel& model,
			IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b,
			IloNumVarMatrix &w,const std::vector<int>& config) {
  try {
    std::vector<double> bound(2*P.nbTask-1);
    std::vector<double> bd(2*P.nbTask,P.D);
    
    createObj(P,env,model,b);//objective min b_ie
    createConstraintOrd(P,model,t);//contrainte te < te+1
    createConstraintOneStart(P,env,model,z);//contrai sum zie>=1
    createConstraintNonPreemp(P,env,model,z);//non preemption
    createConstraintCapacity(P,env,model,t,b);//contrainte capacity
    // createConstraintMinDur(P,model,t,z);//durée minimale
    createEnergyConstraints(P,env,model,t,z,b,w);//contrainte Wi
    createConstraintBmax(P,model,t,b);//contrainte bmax
    createConstraintNonConsump(P,model,z,b);//bie=0 si zie=0
    
    if (config[1] || config[2] ) 
      boundSepEvts(P,bound);
    //te+1 - te <= ... constraints
    if (config[1])
      createConstraintSepEvt(P,model,t,bound);
    //bmin constraints
    if (config[2])
      createConstraintBmin(P,model,t,z,b,bound); //contrainte bmin with bound on te+1-te
    else
      createConstraintBmin(P,model,t,z,b); //contrainte bmin
    
      
    if (!config[3] && config[4])
      boundEvts(P,bd);
    if (config[3])
      createConstraintBoundEvt(P,model,t,bd);
    createConstraintTimeW(P,model,t,z,bd);// te > ri et tf < di 
    
    if (config[5])
      createConstraintKnapsack(P,env,model,z);//knapsack
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
  for (int i = 0 ; i < P.nbTask ; ++i) {
    for (int e = 0 ;e < 2*P.nbTask-1 ; ++e)
      expr += b[i][e];
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
  for (int e = 0 ; e < 2*P.nbTask-1 ; e++)
    model.add(t[e] <= t[e+1]); 
  return 0;
}

int createConstraintOneStart(const Problem<double>& P, IloEnv& env, 
			     IloModel &model, IloNumVarMatrix& z) {
  for (int i = 0 ; i < P.nbTask ; i++) {
    IloExpr expr(env);
    for (int e = 0 ; e < 2*P.nbTask-1 ; e++)
      expr += z[i][e];
    model.add(expr >= 1);
    expr.end();
  }
  return 0;
}

int createConstraintNonPreemp(const Problem<double>& P, IloEnv& env, 
			      IloModel& model, IloNumVarMatrix& z){
  const int E = 2*P.nbTask;
  
  for (int i = 0 ; i < P.nbTask ; i++) {
    for (int e = 1 ; e < E-1 ; e++) {

      IloExpr expr(env);
      IloExpr expr2(env);
      
      for (int f = 1 ; f < e ; f++)
	expr += z[i][f];
      model.add(expr <= (e-1) * (1 - (z[i][e]-z[i][e-1]) ));
      expr.end();
      
      for (int  f = e ; f < E - 1 ; f++)
	expr2 += z[i][f];
      model.add(expr2 <= (E - e) * (1 + (z[i][e]-z[i][e-1]) ));
      expr2.end();
    }
  }
  return 0;
}

int createConstraintCapacity(const Problem<double>& P, IloEnv& env, 
IloModel& model, IloNumVarArray& t, IloNumVarMatrix& b){
  for (int e=0 ; e < 2*P.nbTask-1 ; e++){
    IloExpr expr(env);
    for (int i = 0 ; i < P.nbTask ; i++)
      expr += b[i][e];
    model.add(expr <= P.B*(t[e+1]-t[e]));
    expr.end();
  }
  return 0;
}


int createConstraintMinDur(const Problem<double>& P, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z){
  const int E = 2*P.nbTask;
  for (int i = 0 ; i < P.nbTask ; i++){
    for (int f = 1 ; f < E-1 ; f++)
      model.add(t[f] >= t[0] + (z[i][0]- (z[i][f]-z[i][f-1]) - 1 ) 
		* (P.W(i)/P.A[i].Fi(P.bmax(i))));
  
    for (int e = 1 ; e < E-1 ; e++) {
      for (int f = e + 1 ; f < E-1 ; f++)
	model.add(t[f] >= t[e] + 
		  ((z[i][e]-z[i][e-1]) - (z[i][f]-z[i][f-1]) - 1 ) 
		  * (P.W(i)/P.A[i].Fi(P.bmax(i))));
    }
  }
  return 0;    
}


/*int createConstraintEnergyNul(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarMatrix& w, IloNumVarMatrix& z,const std::vector<double> &M_evt){
  for (int i = 0; i < P.nbTask; ++i)
    for (int e = 0; e < 2 * P.nbTask - 1; ++e)
      model.add( w[i][e] <= M_evt[e] * z[i][e] * P.A[i].Fi.F[0].f.c );
  return 0;
  }*/

int createConstraintEnergyNul(const Problem<double>& P, IloEnv& env, IloModel& model, IloNumVarMatrix& w, IloNumVarMatrix& z){
  for (int i = 0; i < P.nbTask; ++i)
    for (int e = 0; e < 2 * P.nbTask - 1; ++e)
      model.add( w[i][e] <=  P.W(i) * z[i][e] );
  return 0;
}

int createEnergyConstraints(const Problem<double>& P, IloEnv& env, IloModel& model, 
			   IloNumVarArray& t, IloNumVarMatrix& z,
			   IloNumVarMatrix& b, IloNumVarMatrix& w){
  for (int i = 0 ; i < P.nbTask ; ++i){
    IloExpr expr(env);
    for (int e = 0 ; e < 2*P.nbTask-1 ; ++e){
      model.add(P.A[i].Fi.F[0].f.a * b[i][e] +
		P.A[i].Fi.F[0].f.c * (t[e+1] - t[e]) >= w[i][e]);
      expr += w[i][e];
    }
    model.add(expr == P.W(i));
    expr.end();
  }
  createConstraintEnergyNul( P,  env,  model,   w,  z );
  return 0;
}

int createConstraintBmin(const Problem<double>&P, IloModel& model, 
			 IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b, 
			 const std::vector<double> &M_evt){
  for (int i = 0 ; i < P.nbTask ; i++){
    for (int e = 0 ; e < 2*P.nbTask-1 ; e++)
      model.add((t[e+1]-t[e])*P.bmin(i) - P.bmin(i)*M_evt[e]*(1-z[i][e]) <= b[i][e]);
  }
  return 0;
}

int createConstraintBmin(const Problem<double>&P, IloModel& model, IloNumVarArray& t, IloNumVarMatrix& z, IloNumVarMatrix& b){
  for (int i = 0 ; i < P.nbTask ; i++)
    for (int e = 0 ; e < 2*P.nbTask-1 ; e++)
      model.add((t[e+1]-t[e])*P.bmin(i) - (P.bmin(i)*(P.d(i)-P.r(i))*(1-z[i][e]))
		<= b[i][e]);
  
  return 0;
}


int createConstraintBmax(const Problem<double>&P, IloModel& model, 
			 IloNumVarArray& t, IloNumVarMatrix& b){
  for (int i = 0 ; i < P.nbTask ; i++){
    for (int e = 0 ; e < 2*P.nbTask-1 ; e++)
      model.add( (t[e+1]-t[e])*P.bmax(i) >= b[i][e]);
  }
  return 0;
}


int createConstraintNonConsump(const Problem<double>& P, IloModel& model, 
			       IloNumVarMatrix& z , IloNumVarMatrix& b){
  for (int i = 0 ; i < P.nbTask ; i++)
    for (int e = 0 ; e < 2*P.nbTask-1 ; e++)
      model.add(z[i][e]*(P.bmax(i)*(P.d(i)-P.r(i))) >= b[i][e]);
  return 0;
}

int createConstraintTimeW(const Problem<double>& P, IloModel& model,
			  IloNumVarArray& t, IloNumVarMatrix& z, const std::vector<double>& M_te){
  const int E = 2*P.nbTask;
  for (int i = 0 ; i < P.nbTask ; i++) {    
    model.add(z[i][0] * P.r(i) <= t[0]);
    for (int e = 1 ; e < E-1 ; ++e){
      model.add(z[i][e] * P.r(i) <= t[e]);
      int bound_10 =  std::max( std::min (P.D , M_te[e]),
				std::min ( (M_te[e] + P.d(i))/2 ,
					   (P.D + P.d(i))/2 ));
      model.add(P.d(i)*(z[i][e-1]-z[i][e]) + 
		(1-(z[i][e-1]-z[i][e]))* bound_10 >= t[e]);
    }
    model.add(P.d(i)*z[i][E-2] + (1-z[i][E-2])*P.D >= t[E-1]);
  }
  return 0;
}

//***********************************************
//*************ADDITIONAL CONSTRAINTS************
//***********************************************

int createConstraintKnapsack(const Problem<double>& P, IloEnv& env, 
			     IloModel& model, IloNumVarMatrix& z){
  for (int e = 0 ; e < 2*P.nbTask-1 ; ++e){
    IloExpr expr(env);
    for (int i = 0 ; i < P.nbTask ; ++i){
      if (P.bmin(i)!=0)
	expr += z[i][e]*P.bmin(i);
    }
    model.add(expr <= P.B);
    expr.end();
  }
  return 0;
}

int createConstraintBoundEvt(const Problem<double>& P,IloModel& model, 
			     IloNumVarArray& t,std::vector<double>& bd){
  boundEvts(P,bd);
  for (int e = 0 ; e < 2*P.nbTask-1 ; ++e)
    model.add(t[e] <= bd[e]);
  return 0;
}

int createConstraintSepEvt(const Problem<double>& P,IloModel& model, 
			   IloNumVarArray& t,const std::vector<double>& bound){
  for (int e = 0 ; e < 2*P.nbTask-1 ; ++e)
    model.add(t[e+1]-t[e] <= bound[e]);
  return 0;
}
