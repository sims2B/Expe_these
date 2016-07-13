#include "timeModel.h"
#include "satisfiability.h"
#include <iostream>
#include <vector>
#include <algorithm>

//********************************************************
//********************** CALLBACK ************************
//********************************************************

IloCplex::Callback getFirstSolInfo(IloEnv env, IloInt& cpt, IloNum startTime);
IloCplex::Callback depth(IloEnv env, IloInt& nodeDepth);
IloCplex::Callback energyCuts(IloEnv env, const Problem<double>& P, IloModel& model,
			      IloNumVarMatrix& x, IloNumVarMatrix& y,IloNum eps,  IloInt& nodeDepth, 
			      IloInt& maxDepth);


//*********************************************************
//************************SOLVE**************************
//*********************************************************

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
      addERinequalities(P,env,model,x,y,b);
    }
  
    /*if (ERIneq==2) {
      std::cout << " Starting resolution with ER inequalities in tree <10\n";
      IloInt nodeDepth=0;
      IloInt maxDepth=10;
      cplex.use(depth(env,nodeDepth));
      cplex.use(energyCuts(env,P,x,y, 0.01, cptCut,nodeDepth,maxDepth));
    }*/
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


template<typename type,typename type2>
int timeModel<type,type2>::Solve(const Problem<type>& P,Solution<type,type2> &s,double time_limit,double& tps) {
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
  // solve !
  if (cplex.solve()) {	 
    time_exec=cplex.getCplexTime()-start;
    tps+=time_exec;
    modelToSol(P,s,cplex,x,y,b);
    env.end();
    return 1;
  }
  else if (cplex.getStatus()==IloAlgorithm::Infeasible){
    time_exec=cplex.getCplexTime()-start;
    tps+=time_exec;
  }
  env.end();
  return 0;
}


template<typename type,typename type2>
int timeModel<type,type2>::addERinequalities(const Problem<type>& P, IloEnv& env, 
					     IloModel &model,IloNumVarMatrix& x,
					     IloNumVarMatrix&  y, IloNumVarMatrix&  b) {
  int t, i;
  IntervalList<type> list;
  computeCheckInterval(list,P);
  Interval<type> current;
  type2 _b;
  for (uint I=0;I<list.size();++I){
    current=list[I];
    type _B=P.totalResourceConsumption(current) ;
    if (current.t1 < current.t2){
      for (i=0;i<P.nbTask;++i){
	_B= _B - P.A[i].resourceConsumption(current);
	_b=P.A[i].resourceConversion(P.W(i),current);
	IloExpr expr(env),expr2(env),expr3(env),expr4(env),expr5(env);
	for (t=0;t<current.t1;++t)
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
	/*	  for (t=current.t1; t <std::min(P.D,current.t2+1);++t)
		  expr2+=b[i][t];
	*/for (t=current.t2;t<=P.D;++t)
	  expr5+=y[i][t];
	model.add( (expr5-1)*_b +_B <= P.B*(current.t2 - current.t1));
	expr5.end();
	expr2.end();
	_B = _B + P.A[i].resourceConsumption(current);
      }
    }
  } 
  return 0;
}

template<typename type,typename type2>
int timeModel<type,type2>::createB(const Problem<type>& P,IloEnv& env,
				   IloNumVarMatrix& b){
  for (int i=0;i<P.nbTask;i++){
    IloNumArray ub_b(env,P.D); 
    for (int j=P.r(i);j<P.d(i);++j)
      ub_b[j]=P.bmax(i);
    b[i]=IloNumVarArray(env,0, ub_b, ILOFLOAT);
  }
  return 0;
}

template<>
int timeModel<int,int>::createB(const Problem<int>& P,IloEnv& env,
				IloNumVarMatrix& b);


template<typename type,typename type2>
int timeModel<type,type2>::modelToSol(const Problem<type> &P, 
				      Solution<type,type2> &s,IloCplex& cplex,
				      IloNumVarMatrix& x,IloNumVarMatrix& y,
				      IloNumVarMatrix& b){
  for (int i=0;i<P.nbTask;++i){
    for (int t=P.r(i);t<=P.smax(i);++t){
      if (isEqual(cplex.getValue(x[i][t]),1.0))
	s.st[i]=t;
    }
    for (int t=P.emin(i);t<P.d(i);++t){
      if (isEqual(cplex.getValue(y[i][t]),1.0)) 
	s.ft[i]=t;
    }
    if (isEqual(cplex.getValue(y[i][P.d(i)]),1.0))  
      s.ft[i]=P.d(i);
    for (int t=P.r(i);t<P.d(i);++t)
      s.b[i][t]=cplex.getValue(b[i][t]);
    
  }

  for (int i=0;i<=P.D;++i)
    s.event.push_back(i);
  return 0;
}


template<>
int timeModel<int,int>::modelToSol(const Problem<int> &P, 
				   Solution<int,int> &s,IloCplex& cplex,
				   IloNumVarMatrix& x,IloNumVarMatrix& y,
				   IloNumVarMatrix& b);


template<typename type,typename type2>
int timeModel<type,type2>::createModel(const Problem<type>& P, IloEnv& env, IloModel& m, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  return createVars(P,env,x,y,b,w) || createConstraints(P,env,m,x,y,b,w);
}
	
template<typename type, typename type2>
int timeModel<type,type2>::createVars(const Problem<type>& P, IloEnv& env,  IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, IloNumVarMatrix& w) {
  IloInt j,i;
  const int n=P.nbTask;
  
  for (i=0;i<n;i++){
    IloNumArray ub_b(env,P.D); 
    IloNumArray ub_x(env,P.D+1);
    IloNumArray ub_y(env,P.D+1);
    for (j=P.r(i);j<=P.smax(i);++j)
      ub_x[j]=1;
    x[i]=IloNumVarArray(env,0, ub_x,ILOINT);
    for (j=P.emin(i);j<=P.d(i);++j)
      ub_y[j]=1;
    y[i]=IloNumVarArray(env,0, ub_y,ILOINT);
    for ( j=P.r(i);j<P.d(i);++j)
      ub_b[j]=P.A[i].Fi.F[0].f.a*P.bmax(i)+P.A[i].Fi.F[0].f.c;
    w[i]=IloNumVarArray(env,0,ub_b, ILOFLOAT);
  }
		
  return createB(P,env,b);
}



template<typename type,typename type2>	
int timeModel<type,type2>::createConstraints(const Problem<type>& P, IloEnv& env, 
					     IloModel& model, IloNumVarMatrix& x, IloNumVarMatrix& y, IloNumVarMatrix& b, 
					     IloNumVarMatrix& w) {	
  int i,t,tau;
  const int nbTaskInt=P.nbTask;	

    IloExpr expr(env);
      for (i=0;i<nbTaskInt;++i){
      for (t=0;t<P.D;++t)
      expr += b[i][t];
      }
      model.add(IloMinimize(env,expr));
      expr.end();
  
  // (sum x_it == 1) et (sum yit==1)
  for (i=0; i<nbTaskInt; i++) {
    IloExpr exprx(env);
    for (t=P.A[i].ri; t<=P.A[i].smax;t++)
      exprx+=x[i][t];
    model.add(exprx == 1);
    exprx.end();
      
    IloExpr expry(env);
    for (t=P.A[i].emin; t<=P.A[i].di;t++)
      expry+=y[i][t];
    model.add(expry == 1);
    expry.end();
  }

  //contrainte bmin
  for (i=0;i<nbTaskInt;i++) {
    for (t=P.A[i].ri;t<P.A[i].di;t++){
      IloExpr expr(env);
      for (tau=P.A[i].ri;tau<=t;tau++)
	expr+=x[i][tau]-y[i][tau];
      model.add(expr*P.A[i].bmin<=b[i][t]);
      expr.end();
    }
  }

  //contrainte bit=0 si t < start
  for (i=0;i<nbTaskInt;i++)  {
    for (t=P.r(i);t<P.d(i);t++){
      IloExpr expr(env);
      for (tau=P.r(i);tau<=t;tau++)
	expr+=x[i][tau]-y[i][tau];
      model.add(expr*P.bmax(i) >= b[i][t]);
      expr.end();
    }
  }
 
  //contrainte ressource
  for (t=0;t<P.D;t++) {
    IloExpr expr(env);
    for (i=0;i<nbTaskInt;i++)
      expr+=b[i][t];
    model.add(expr <= P.B);
    expr.end();
  }
   
  //Contrainte Wi
  for (i=0;i< nbTaskInt; i++){
    IloExpr expr(env);
    for (t=P.A[i].ri;t<P.A[i].di;t++)
      expr+=w[i][t];
    model.add(expr >= P.A[i].Wi);
    expr.end();
  }

  
  for (i=0;i<nbTaskInt;++i) {
    for (t=P.r(i);t<P.d(i);t++){
      IloExpr expr(env);
      for (tau=P.r(i);tau<=t;++tau)
	expr+=x[i][tau]-y[i][tau];
      for (int q=0;q<P.A[i].Fi.nbPiece;++q)
	model.add(P.A[i].Fi.F[q].f.a*b[i][t]+P.A[i].Fi.F[q].f.c*(expr)>=w[i][t]);
      expr.end();
    }
  }
  return 0;
}

