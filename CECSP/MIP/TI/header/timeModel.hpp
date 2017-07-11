#ifndef TIMEMODEL_H
#define TIMEMODEL_H

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

template<typename type, typename type2>
int Solve(const Problem<type>& P,Solution<type,type2> &s);

template<>
int Solve<int,int>(const Problem<int>& P,Solution<int,int> &s);

//**************** CPLEX SOL -> STRUCT SOL ******************

template<typename type, typename type2>
int modelToSol(const Problem<type>&,Solution<type,type2>&,IloCplex&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&);

template<>
int modelToSol<int,int>(const Problem<int> &, Solution<int,int> &,IloCplex&,
	       IloNumVarMatrix&,IloNumVarMatrix&, IloNumVarMatrix&);

//********************* MODEL CREATION ************************
template<typename type>
int createModel(const Problem<type>&,IloEnv&,IloModel&,
IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
		IloNumVarMatrix&);


template<typename type>
int createVars(const Problem<type>&,IloEnv&,
	       IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
	       IloNumVarMatrix&);

template<typename type>
int createConstraints(const Problem<type>&,IloEnv&,IloModel&,
		      IloNumVarMatrix&,IloNumVarMatrix&,IloNumVarMatrix&,
		      IloNumVarMatrix&);


//************************** CALLBACK ****************************
/*IloCplex::Callback getFirstSolInfo(IloEnv , IloInt& , IloNum );
IloCplex::Callback depth(IloEnv, IloInt& );
IloCplex::Callback energyCuts(IloEnv , const Problem<double>& ,
			      IloNumVarMatrix& , IloNumVarMatrix& ,IloNum , 
			      IloInt& ,  IloInt& , IloInt& );*/
			      
			      

///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type, typename type2>
int Solve(const Problem<type>& P,Solution<type,type2> &s) {
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
  
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads,2);
  cplex.setOut(env.getNullStream());
  start= cplex.getCplexTime();
  //IloInt cpt=0;
  //cplex.use(getFirstSolInfo(env,cpt,start));
  
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


template<typename type,typename type2>
int modelToSol(const Problem<type> &P, Solution<type,type2> &s,
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


template<typename type>
int createModel(const Problem<type>& P, IloEnv& env, IloModel& m, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  return createVars(P,env,x,y,b,w) || createConstraints(P,env,m,x,y,b,w);
}

template<typename type>
int createVars(const Problem<type>& P, IloEnv& env, IloNumVarMatrix& x, 
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


template<typename type>
int createConstraints(const Problem<type>& P, IloEnv& env, IloModel& model, 
		      IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, 
		      IloNumVarMatrix& w) {	
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
      for (tau=P.A[i].ri;tau<t;tau++)
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
  }
  return 0;
}

/*

ILOINCUMBENTCALLBACK2(getFirstSolInfo, IloInt&, cpt, IloNum, startTime){
  if (cpt <1){
    std::cout << "Sol n°" << cpt+1 << " obj: \t" << getObjValue() << std::endl;
    std::cout << "Sol n°" << cpt+1 << " time: \t" << getCplexTime()-startTime << std::endl;
    std::cout << "Sol n°" << cpt+1 << " gap: \t" << getMIPRelativeGap() << std::endl;
    cpt=cpt+1;
  }
    
}

ILONODECALLBACK1(depth, IloInt& , nodeDepth) {
  nodeDepth=getDepth(0);
}




#define USERCUTLIMIT 500


//template???? A refaire dans tous les cas.
ILOUSERCUTCALLBACK6(energyCuts, const Problem<double>&, P,  
		    IloNumVarMatrix&, x, IloNumVarMatrix&,  y, IloNum, eps, 
		    IloInt&, nodeDepth, IloInt& , depthMax) {
  try{
    if ( !isAfterCutLoop() )
      return;
  
    if ( nodeDepth >= 1 && nodeDepth <= depthMax ) {    
      int t, i;
      IntervalList<double> L;
      computeCheckInterval(L,P);
      double _b;
      for (uint l=0;l<L.size();++l){
	double _B=P.totalResourceConsumption(L[l]) ;
	if (L[l].t1 < L[l].t2){
	  for (i=0;i<P.nbTask;++i){
	    double val = 0.0;
	    IloExpr expr(getEnv());	  
	    IloRange cut;
	    _B= _B - P.A[i].resourceConsumption(L[l]);
	    _b=P.A[i].resourceConversion(P.W(i),L[l]);
	    for (t=0;t<std::min(L[l].t1,P.D+1);++t)
	      val -= getValue(x[i][t]);
	    for (t=std::max(P.r(i)+1,L[l].t2+1);t<=P.D;++t)
	      val -= getValue(y[i][t]);

	    IloNum rhs = P.B * (L[l].t2 - L[l].t1);
	    if ((1+val * _b + _B <= rhs)){
	      for (t=0;t<std::min(L[l].t1,P.D+1);++t)
		expr -= x[i][t];
	      for (t=std::max(0,(int)L[l].t2+1);t<=P.D;++t)
		expr -= y[i][t];

	      cut= ( (1 + expr) * _b + _B  <= rhs);
	      add(cut);
	      cut.end();	
	      
	      expr.end();
	    }
	  
	    _b=P.A[i].resourceConversion(std::max(0.0,P.W(i)-P.A[i].Fi(P.bmax(i))*(std::max(0.0,L[l].t1-P.r(i)) +std::max(0.0, P.d(i) - L[l].t2))),L[l]);
	    if ((getValue(x[i][P.A[i].ri])+ getValue(y[i][P.A[i].di]-1))*_b+_B <= P.B*(L[l].t2 - L[l].t1))
	      add((x[i][P.A[i].ri]+ y[i][P.A[i].di]-1)*_b+_B <= P.B*(L[l].t2 - L[l].t1));
	       
	    val =0.0;
	    _b=P.A[i].resourceConversion(P.A[i].leftShift(L[l]),L[l]);
	    for (t=std::max(0,(int)L[l].t1+1);t<=std::min(P.D,L[l].t2-1);++t)
	      val +=getValue(y[i][t]);
	    if ((val+getValue(x[i][P.A[i].ri])-1)*_b + _B <= P.B*(L[l].t2 - L[l].t1)){
	      for (t=std::max(0,(int)L[l].t1+1);t<=std::min(P.D,L[l].t2-1);++t) 
		expr+=y[i][t];
	      add((expr+x[i][P.A[i].ri]-1)*_b + _B <= P.B*(L[l].t2 - L[l].t1));
	      expr.end();
	    }
	    
	    val=0.0;
	    _b=P.A[i].resourceConversion(P.A[i].rightShift(L[l]),L[l]);	
	    for (t=std::max(0,(int)L[l].t1);t<std::min(P.D+1,L[l].t2);++t)
	      val += getValue(x[i][t]);
	    if ((val+ getValue(y[i][P.A[i].di-1]))*_b + _B <= P.B*(L[l].t2 - L[l].t1)){
	      for (t=std::max(0,(int)L[l].t1);t<std::min(P.D+1,L[l].t2);++t)
		expr+=x[i][t];
	      add((expr+ y[i][P.A[i].di-1])*_b + _B <= P.B*(L[l].t2 - L[l].t1));
	      expr.end();}
	      
	    _b=P.bmin(i)*(L[l].t2 - L[l].t1);       
	    val =0.0;
	    for (t= 0;t<=std::min(P.D,L[l].t1);++t)
	      val+=getValue(x[i][t]);
	    for (t=std::max((int)L[l].t2,0);t<=P.D;++t)
	      val+=getValue(y[i][t]);
	    if ((val-1)*_b +_B <= P.B*(L[l].t2 - L[l].t1)){
	      for (t=0;t<=std::min(P.D,L[l].t1);++t)
		expr+=x[i][t];
	      for (t=std::max((int)L[l].t2,0);t<=P.D;++t)
		expr+=y[i][t];
	      add( (expr-1)*_b +_B <= P.B*(L[l].t2 - L[l].t1));
	      expr.end();
	    }
	    _B = _B + P.A[i].resourceConsumption(L[l]);
	  }
	}
      }
    }
  }
  catch (IloCplex::InvalidCutException &e) {
    std::cout << "Iloexception in energy cuts : " << e.getCut() << std::endl;
    std:: cout << e.getCut() << std::endl;
    e.end();
    throw;
  }
  catch (IloException &e) {
    std::cout << "Iloexception in energy cuts : " << e << std::endl;
    e.end();
    throw;
  }
}
*/

#endif
