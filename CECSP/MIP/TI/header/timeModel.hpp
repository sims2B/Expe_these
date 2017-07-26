#ifndef TIMEMODEL_HPP
#define TIMEMODEL_HPP

#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>
#include <iostream>
#include "Solution.hpp"


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// DEFINITION /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TIME_LIMIT
#define time_limit 1000
#endif

typedef IloArray<IloNumVarArray> IloNumVarMatrix;

//********************* SOLVE FUNCTION ************************
//§§§ No obj
template<typename type, typename type2=type>
int SolveConcave(const Problem<type,type2>& P,Solution<type,type2> &s);
//§§§ No obj
template<typename type, typename type2=type>
int SolveConvex(const Problem<type,type2>& P,Solution<type,type2> &s);

template<typename type, typename type2=type>
int LPsolveConcave(const Problem<type,type2>& P);
template<typename type, typename type2=type>
int LPsolveConvex(const Problem<type,type2>& P);

//**************** CPLEX SOL -> STRUCT SOL ******************

template<typename type, typename type2=type>
int modelToSol(const Problem<type,type2>&,Solution<type,type2>&,IloCplex&,
	       IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&);

int modelToSol(const Problem<int,int> &, Solution<int,int> &,IloCplex&,
			IloNumVarMatrix&,IloNumVarMatrix&, IloNumVarMatrix&);
int modelToSol(const Problem<int,double> &, Solution<int,double> &,IloCplex&,
			IloNumVarMatrix&,IloNumVarMatrix&, IloNumVarMatrix&);

//********************* MODEL CREATION ************************
template<typename type,typename type2=type>
int createConcaveModel(const Problem<type,type2>&,IloEnv&,IloModel&,
		       IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
		       IloNumVarMatrix&);

template<typename type,typename type2=type>
int createConvexModel(const Problem<type,type2>&,IloEnv&,IloModel&,
		      IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
		      IloNumVarMatrix&);

template<typename type,typename type2=type>
int createVars(const Problem<type,type2>&,IloEnv&,
	       IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
	       IloNumVarMatrix&);

int createVars(const Problem<int,int>&,IloEnv&,
	       IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
	       IloNumVarMatrix&);

template<typename type, typename type2=type>
int createConcaveConstraints(const Problem<type,type2 >&,IloEnv&,IloModel&,
			     IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
			     IloNumVarMatrix&);

template<typename type, typename type2=type>
int createConvexConstraints(const Problem<type,type2 >&,IloEnv&,IloModel&,
			    IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
			    IloNumVarMatrix&);

//TODO _b ou véritable expression (cf. manuscrit)
template<typename type, typename type2=type>
int addERinequalities(const Problem<type>&, IloEnv&, IloModel&, 
		      IloNumVarMatrix&,IloNumVarMatrix&);

//************************** CALLBACK ****************************
IloCplex::Callback getFirstSolInfo(IloEnv , IloInt& , IloNum );
/*IloCplex::Callback depth(IloEnv, IloInt& );
  IloCplex::Callback energyCuts(IloEnv , const Problem<double>& ,
  IloNumVarMatrix& , IloNumVarMatrix& ,IloNum , 
  IloInt& ,  IloInt& , IloInt& );
*/
			      

////////////////////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

template<typename type,typename type2>
int LPsolveConcave(const Problem<type,type2>& P){
  try  {
    IloNum start,time_exec;
    const int n = P.nbTask;
    
    // create cplex model  
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix x(env,n);
    IloNumVarMatrix y(env,n);
    IloNumVarMatrix b(env,n);
    IloNumVarMatrix w(env,n);
    
    createConcaveModel(P,env,model,x,y,b,w);
    IloCplex cplex(model);
    for (int i=0;i<n;++i)
      for (int t=0;t<P.D;++t){
	model.add(IloConversion(env,x[i][t],ILOFLOAT));
	model.add(IloConversion(env,y[i][t],ILOFLOAT));
      }

    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setParam(IloCplex::Threads,2);
    cplex.setOut(env.getNullStream());
    start= cplex.getCplexTime();
    
    // solve !
    if (cplex.solve()) {	 
      time_exec=cplex.getCplexTime()-start;
      std::cout << "Root objective: \t"<< cplex.getObjValue() << std::endl;   
      env.end();
      return 0;
    }
    else if (cplex.getStatus()==IloAlgorithm::Infeasible){
      time_exec=cplex.getCplexTime()-start;
      std::cout << "status: "<< cplex.getStatus() << " at root node en " << time_exec << std::endl;
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


template<typename type,typename type2>
int LPsolveConvex(const Problem<type,type2>& P){
  try  {
    IloNum start,time_exec;
    const int n = P.nbTask;
    
    // create cplex model  
    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix x(env,n);
    IloNumVarMatrix y(env,n);
    IloNumVarMatrix b(env,n);
    IloNumVarMatrix w(env,n);
    
    createConvexModel(P,env,model,x,y,b,w);
    IloCplex cplex(model);
    for (int i=0;i<n;++i)
      for (int t=0;t<P.D;++t){
	model.add(IloConversion(env,x[i][t],ILOFLOAT));
	model.add(IloConversion(env,y[i][t],ILOFLOAT));
      }

    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setParam(IloCplex::Threads,2);
    cplex.setOut(env.getNullStream());
    start= cplex.getCplexTime();
    
    // solve !
    if (cplex.solve()) {	 
      time_exec=cplex.getCplexTime()-start;
      std::cout << "Root objective: \t"<< cplex.getObjValue() << std::endl;   
      env.end();
      return 0;
    }
    else if (cplex.getStatus()==IloAlgorithm::Infeasible){
      time_exec=cplex.getCplexTime()-start;
      std::cout << "status: "<< cplex.getStatus() << " at root node en " << time_exec << std::endl;
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


template<typename type, typename type2=type>
int SolveConcave(const Problem<type,type2>& P,Solution<type,type2> &s) {
  IloNum start,time_exec;
  const int n= P.nbTask;
  
  IloEnv env;
  IloModel model(env);
  		
  IloNumVarMatrix x(env,n);
  IloNumVarMatrix y(env,n);
  IloNumVarMatrix b(env,n);
  IloNumVarMatrix w(env,n);

  createConcaveModel(P,env,model,x,y,b,w);
  IloCplex cplex(model);
  
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

    modelToSol(P,s,cplex,x,y,b);
    env.end();
    return 0;
  }
  else if (cplex.getStatus()==IloAlgorithm::Infeasible){
    time_exec=cplex.getCplexTime()-start;
    std::cout << "Final status: \t"<< cplex.getStatus() << " en " 
	      << time_exec << std::endl;
    env.end();
    return 1;
  }
  env.end();
  return 1;
}


template<typename type, typename type2=type>
int SolveConvex(const Problem<type,type2>& P,Solution<type,type2> &s) {
  IloNum start,time_exec;
  const int n= P.nbTask;
  
  IloEnv env;
  IloModel model(env);
  		
  IloNumVarMatrix x(env,n);
  IloNumVarMatrix y(env,n);
  IloNumVarMatrix b(env,n);
  IloNumVarMatrix w(env,n);

  createConvexModel(P,env,model,x,y,b,w);
  IloCplex cplex(model);
  
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
    modelToSol(P,s,cplex,x,y,b);
    env.end();
    return 0;
  }
  else if (cplex.getStatus()==IloAlgorithm::Infeasible){
    time_exec=cplex.getCplexTime()-start;
    std::cout << "Final status: \t"<< cplex.getStatus() << " en " 
	      << time_exec << std::endl;
    env.end();
    return 1;
  }
  env.end();
  return 1;
}


template<typename type,typename type2>
int modelToSol(const Problem<type,type2> &P, Solution<type,type2> &s,
	       IloCplex& cplex, IloNumVarMatrix& x,IloNumVarMatrix& y, 
	       IloNumVarMatrix& b){
  int i, t; 
  for (i=0;i<P.nbTask;++i){
    for (t=P.r(i);t<=P.smax(i);++t){
      if (isEqual(cplex.getValue(x[i][t]),1.0))
	s.st[i]=t;
    } 
    for (t=P.emin(i)-1;t<P.d(i);++t){
      if (isEqual(cplex.getValue(y[i][t]),1.0)) 
	s.ft[i]=t+1;
    }
    for (t=P.r(i);t<P.d(i);++t)
      s.b[i][t]=cplex.getValue(b[i][t]);    
  }
  for (i=0;i<P.D+1;++i)
    s.event.push_back(i);
  return 0;
}


template<typename type,typename type2>
int createConcaveModel(const Problem<type,type2>& P, IloEnv& env, IloModel& m, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  return createVars(P,env,x,y,b,w) || createConcaveConstraints(P,env,m,x,y,b,w);
}

template<typename type,typename type2>
int createConvexModel(const Problem<type,type2>& P, IloEnv& env, IloModel& m, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  return createVars(P,env,x,y,b,w) || createConvexConstraints(P,env,m,x,y,b,w);
}

template<typename type,typename type2>
int createVars(const Problem<type,type2>& P, IloEnv& env, IloNumVarMatrix& x, 
	       IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  IloInt i,j;
  const int n=P.nbTask;
  
  for (i=0;i<n;i++){
    IloNumArray ub_x(env,P.D);
    IloNumArray ub_y(env,P.D);
    IloNumArray ub_b(env,P.D); 
    IloNumArray ub_w(env,P.D); 
    for (j=0;j<P.D;++j)
      if (j==P.r(i) || j<=P.smax(i)) ub_x[j]=1;
      else ub_x[j]=0;
    x[i]=IloNumVarArray(env,0, ub_x,ILOINT);
    for (j=0;j<P.D;++j)
      if (j==P.emin(i)-1 || j<P.d(i)) ub_y[j]=1;
      else  ub_y[j]=0;
    y[i]=IloNumVarArray(env,0, ub_y,ILOINT);
    for (j=0;j<P.D;++j)
      if (j==P.r(i) || j<P.d(i)) ub_b[j]=P.bmax(i);
      else ub_b[j]=0;
    b[i]=IloNumVarArray(env,0, ub_b, ILOFLOAT);
    for (j=0;j<P.D;++j)
      if ( j==P.r(i) || j<P.d(i)) ub_w[j]=P.A[i].Fi(P.bmax(i));
      else ub_w[j]=0;
    w[i]=IloNumVarArray(env,0,ub_w, ILOFLOAT);
  }
  return 0;
}


template<typename type,typename type2>
int createConcaveConstraints(const Problem<type,type2>& P, IloEnv& env, 
			     IloModel& model,  IloNumVarMatrix& x, IloNumVarMatrix& y, 
			     IloNumVarMatrix& b, IloNumVarMatrix& w) {	
  int i,t,tau;
  const int nbTask=P.nbTask;	
  
  //objective
   IloExpr expr(env);
  for (i=0;i<nbTask;++i){
    for (t=0;t<P.D;++t)
      expr += b[i][t];
  }
  model.add(IloMinimize(env,expr));
  expr.end();
  
  // (sum x_it == 1) et (sum yit==1)
  for (i=0; i<nbTask; i++) {
    IloExpr exprx(env);
    for (t=P.A[i].ri; t<=P.A[i].smax;t++)
      exprx+=x[i][t];
    model.add(exprx == 1);
    exprx.end();
      
    IloExpr expry(env);
    for (t=P.A[i].emin-1; t<P.A[i].di;t++)
      expry+=y[i][t];
    model.add(expry == 1);
    expry.end();
  }

  //contrainte bmin
  for (i=0;i<nbTask;i++) {
    for (t=P.A[i].ri;t<P.A[i].di;t++){
      IloExpr expr(env);
      for (tau=P.A[i].ri;tau<=t;tau++)
	expr+=x[i][tau];
      for (tau=P.A[i].ri+1;tau<t;tau++)
	expr+=-y[i][tau];
      model.add(expr*P.A[i].bmin <= b[i][t]);
      expr.end();
    }
  }

  //contrainte bmax
  for (i=0;i<nbTask;i++)  {
    for (t=P.r(i);t<P.d(i);t++){
      IloExpr expr(env);
      for (tau=P.A[i].ri+1;tau<=t;tau++)
	expr+=x[i][tau];
      for (tau=P.A[i].ri;tau<t;tau++)
	expr+=-y[i][tau];
      model.add(expr*P.bmax(i) >= b[i][t]);
      expr.end();
    }
  }
 
  //contrainte ressource
  for (t=0;t<P.D;t++) {
    IloExpr expr(env);
    for (i=0;i<nbTask;i++)
      expr+=b[i][t];
    model.add(expr <= P.B);
    expr.end();
  }
   
  //Contraintes Wi
  for (i=0;i< nbTask; i++){
    IloExpr expr(env);
    for (t=P.A[i].ri;t<P.A[i].di;t++)
      expr+= w[i][t];
    model.add(expr >= P.A[i].Wi);
    expr.end();
  }
  
  // wit = 0 if task not in process
  for (i=0;i< nbTask; i++){
    for (t=0;t<P.D;t++){
      IloExpr expr(env);
      for (tau=P.A[i].ri;tau<=t;tau++)
	expr+=x[i][tau];
      for (tau=P.A[i].ri;tau<t;tau++)
	expr+=-y[i][tau];
      model.add(expr*P.A[i].Wi >= w[i][t]);
      expr.end();
    }
  }

  for (i=0;i<nbTask;++i) {
    for (t=P.r(i);t<P.d(i);t++){
      IloExpr expr(env);
      for (tau=P.A[i].ri;tau<=t;tau++)
	expr+=x[i][tau];
      for (tau=P.A[i].ri;tau<t;tau++)
	expr+=-y[i][tau];
      for (int q=0;q<P.A[i].Fi.nbPiece;++q)
	model.add(P.A[i].Fi.F[q].f.a*b[i][t]+P.A[i].Fi.F[q].f.c*(expr)>=w[i][t]);
      expr.end();
    }
  }   for (i=0; i<nbTask; i++) {
      IloExpr exprx(env);
      for (t=0; t<P.A[i].ri;t++)
	exprx+=x[i][t];
      for (t=P.smax(i)+1; t<P.D;t++)
	exprx+=x[i][t];
      model.add(exprx == 0);
      exprx.end();
      
      IloExpr expry(env);
      for (t=0; t<P.A[i].emin-1;t++)
	expry+=y[i][t];
      for (t=P.d(i); t<P.D;t++)
	expry+=y[i][t];
      model.add(expry == 0);
      expry.end();
    }
  return 0;
}

template<typename type,typename type2>
int createConvexConstraints(const Problem<type,type2>& P, IloEnv& env, 
			    IloModel& model,  IloNumVarMatrix& x, IloNumVarMatrix& y, 
			    IloNumVarMatrix& b, IloNumVarMatrix& w) {	
  try{
    int i,t,tau,q;
    const int nbTask=P.nbTask;	
    
    //objective
    /*    IloExpr expr(env);
    for (i=0;i<nbTask;++i){
      for (t=0;t<P.D;++t)
	expr += b[i][t];
    }
    model.add(IloMinimize(env,expr));
    expr.end();
    */
    // (sum x_it == 1) et (sum yit==1)
    for (i=0; i<nbTask; i++) {
      IloExpr exprx(env);
      for (t=P.A[i].ri; t<=P.A[i].smax;t++)
	exprx+=x[i][t];
      model.add(exprx == 1);
      exprx.end();
      
      IloExpr expry(env);
      for (t=P.A[i].emin-1; t<P.A[i].di;t++)
	expry+=y[i][t];
      model.add(expry == 1);
      expry.end();
    }

    //contrainte bmin
    for (i=0;i<nbTask;i++) {
      for (t=P.A[i].ri;t<P.A[i].di;t++){
	IloExpr expr(env);
	for (tau=P.A[i].ri;tau<=t;tau++)
	  expr+=x[i][tau];
	for (tau=P.A[i].ri+1;tau<t;tau++)
	  expr+=-y[i][tau];
	model.add(expr*P.A[i].bmin<=b[i][t]);
	expr.end();
      }
    }

    //contrainte bmax
    for (i=0;i<nbTask;i++)  {
      for (t=P.r(i);t<P.d(i);t++){
	IloExpr expr(env);
	for (tau=P.A[i].ri;tau<=t;tau++)
	  expr+=x[i][tau];
	for (tau=P.A[i].ri;tau<t;tau++)
	  expr+=-y[i][tau];
	model.add(expr*P.bmax(i) >= b[i][t]);
	expr.end();
      }
    }


    //contrainte ressource
    for (t=0;t<P.D;t++) {
      IloExpr expr(env);
      for (i=0;i<nbTask;i++)
	expr+=b[i][t];
      model.add(expr <= P.B);
      expr.end();
    }
   
    //Contraintes Wi
    for (i=0;i< nbTask; i++){
      IloExpr expr(env);
      for (t=P.A[i].ri;t<P.A[i].di;t++)
	expr+=w[i][t];
      model.add(expr >= P.A[i].Wi);
      expr.end();
    }
  
  
  
    for (i=0;i<nbTask;++i) {
      if (P.A[i].Fi.nbPiece > 1){
	IloNumArray bp(env,P.A[i].Fi.nbPiece-1); 
	IloNumArray slope(env,P.A[i].Fi.nbPiece);
	for (q=0;q<P.A[i].Fi.nbPiece;++q)
	  slope[q]=P.A[i].Fi.F[q].f.a;
	for (q=0;q<P.A[i].Fi.nbPiece-1;++q)
	  bp[q]=P.A[i].Fi.F[q+1].I.t1;

	for (t=P.r(i);t<P.d(i);t++)
	  model.add(w[i][t]<=
		    IloPiecewiseLinear(b[i][t],bp,slope,P.bmin(i),P.A[i].Fi(P.bmin(i))));
    
	// wit = 0 if task not in process
	for (t=P.r(i);t<P.d(i);t++){
	  IloExpr expr(env);
	  for (tau=P.A[i].ri;tau<=t;tau++)
	    expr+=x[i][tau];
	  for (tau=P.A[i].ri;tau<t;tau++)
	    expr+=-y[i][tau];
	  model.add(expr*P.A[i].Wi >= w[i][t]);
	  expr.end();
	}
      }
      else{
	for (t=P.r(i);t<P.d(i);t++){
	  IloExpr expr(env);
	  for (tau=P.A[i].ri;tau<=t;tau++)
	    expr+=x[i][tau];
	  for (tau=P.A[i].ri;tau<t;tau++)
	    expr+=-y[i][tau];
	  for (int q=0;q<P.A[i].Fi.nbPiece;++q)
	    model.add(P.A[i].Fi.F[q].f.a*b[i][t]+P.A[i].Fi.F[q].f.c*(expr)==w[i][t]);
	  expr.end();
	}
      }
    }

    for (i=0; i<nbTask; i++) {
      IloExpr exprx(env);
      for (t=0; t<P.A[i].ri;t++)
	exprx+=x[i][t];
      for (t=P.smax(i)+1; t<P.D;t++)
	exprx+=x[i][t];
      model.add(exprx == 0);
      exprx.end();
      
      IloExpr expry(env);
      for (t=0; t<P.A[i].emin-1;t++)
	expry+=y[i][t];
      for (t=P.d(i); t<P.D;t++)
	expry+=y[i][t];
      model.add(expry == 0);
      expry.end();
    }
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



  template<typename type,typename type2>
    int addERinequalities(const Problem<type,type2>& P, IloEnv& env,
			  IloModel &model,IloNumVarMatrix& x, IloNumVarMatrix&  y) {
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




#endif
