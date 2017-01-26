#include "time_model.h"
#include "satisfiability.h"

ILOINCUMBENTCALLBACK2(getFirstSolInfo, IloInt&, cpt, IloNum, startTime){
  if (cpt <1){
    std::cout << "Sol n°" << cpt+1 << " obj: \t" << getObjValue() << std::endl;
    std::cout << "Sol n°" << cpt+1 << " time: \t" << getCplexTime()-startTime << std::endl;
    std::cout << "Sol n°" << cpt+1 << " gap: \t" << getMIPRelativeGap() << std::endl;
    cpt=cpt+1;
  } 
}

#define USERCUTLIMIT 500

ILONODECALLBACK1(depth, IloInt& , nodeDepth) {
  nodeDepth=getDepth(0);
}

ILOUSERCUTCALLBACK6(energyCuts, const Problem<double>&, P,  
		    IloNumVarMatrix&, x,
		    IloNumVarMatrix&,  y, IloNum, eps, IloInt&, nodeDepth, IloInt& , depthMax) {
  try{
    if ( !isAfterCutLoop() )
      return;
  
    if ( nodeDepth >= 1 && nodeDepth <= depthMax ) {    
      int t, i;
      IntervalList<double> L;
      computeCheckInterval(L,P);
      double _b;
      for (uint l=0;l<L.size();++l){
	if (L[l].t1 < L[l].t2){
	  double _B=P.totalResourceConsumption(L[l]) ;
	  for (i=0;i<P.nbTask;++i){
	    double val = 0.0;
	    IloExpr expr1(getEnv());	  
	    IloRange cut;
	    _B= _B - P.A[i].resourceConsumption(L[l]);
	    	    _b=P.A[i].resourceConversion(P.W(i),L[l]);
	    for (t=0;t<=std::min(L[l].t1,P.D+1);++t)
	      val -= getValue(x[i][t]);
	    for (t=std::max(P.r(i)+1,L[l].t2);t<=P.D;++t)
	      val -= getValue(y[i][t]);

	    IloNum rhs = P.B * (L[l].t2 - L[l].t1);
	    if ((1+val * _b + _B <= rhs)){
	      for (t=0;t<=std::min(L[l].t1,P.D+1);++t)
		expr1 -= x[i][t];
	      for (t=std::max(0,(int)L[l].t2);t<=P.D;++t)
		expr1 -= y[i][t];

	      cut= ( (1 + expr1) * _b + _B  <= rhs);
	      add(cut);
	      cut.end();	
	      
	      expr1.end();
	    }
	  
	    _b=P.A[i].resourceConversion(std::max(0.0,P.W(i)-P.A[i].Fi(P.bmax(i))*(std::max(0.0,L[l].t1-P.r(i)) +std::max(0.0, P.d(i) - L[l].t2))),L[l]);
	    if ((getValue(x[i][P.A[i].ri])+ getValue(y[i][P.A[i].di])-1)*_b+_B <= P.B*(L[l].t2 - L[l].t1))
	      add((x[i][P.A[i].ri]+ y[i][P.A[i].di]-1)*_b+_B <= P.B*(L[l].t2 - L[l].t1));
	    

	    val =0.0;
	    _b=P.A[i].resourceConversion(P.A[i].leftShift(L[l]),L[l]);
	    for (t=std::max(0,(int)L[l].t1+1);t<=std::min(P.D,L[l].t2);++t)
	      val +=getValue(y[i][t]);
	    if ((val+getValue(x[i][P.A[i].ri])-1)*_b + _B <= P.B*(L[l].t2 - L[l].t1)){
	      IloExpr expr2(getEnv());
	      for (t=std::max(0,(int)L[l].t1+1);t<=std::min(P.D,L[l].t2);++t) 
		expr2+=y[i][t];
	      add((expr2+x[i][P.A[i].ri]-1)*_b + _B <= P.B*(L[l].t2 - L[l].t1));
	      expr2.end();
	    }
	    
	    val=0.0;
	    _b=P.A[i].resourceConversion(P.A[i].rightShift(L[l]),L[l]);	
	    for (t=std::max(0,(int)L[l].t1);t<std::min(P.D+1,L[l].t2-1);++t)
	      val += getValue(x[i][t]);
	    if ((val+ getValue(y[i][P.A[i].di]) -1)*_b + _B <= P.B*(L[l].t2 - L[l].t1)){
	      IloExpr expr3(getEnv());
	      for (t=std::max(0,(int)L[l].t1);t<std::min(P.D+1,L[l].t2-1);++t)
		expr3+=x[i][t];
	      add((expr3+ y[i][P.A[i].di]-1)*_b + _B <= P.B*(L[l].t2 - L[l].t1));
	      expr3.end();}
	      
	    _b=P.bmin(i)*(L[l].t2 - L[l].t1);       
	    val =0.0;
	    for (t= 0;t<=std::min(P.D,L[l].t1);++t)
	      val+=getValue(x[i][t]);
	    for (t=std::max((int)L[l].t2,0);t<=P.D;++t)
	      val+=getValue(y[i][t]);
	    if ((val-1)*_b +_B <= P.B*(L[l].t2 - L[l].t1)){
	      IloExpr expr4(getEnv());
	      for (t=0;t<=std::min(P.D,L[l].t1);++t)
		expr4+=x[i][t];
	      for (t=std::max((int)L[l].t2,0);t<=P.D;++t)
		expr4+=y[i][t];
	      add( (expr4-1)*_b +_B <= P.B*(L[l].t2 - L[l].t1));
	      expr4.end();
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



int Solve(const Problem<double>& P,Solution<double,double>& s, double time_limit, int ERIneq){
  try{
    IloNum start,time_exec;
    const int n= P.nbTask;
    // IloInt cptCut=0;
  
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
      addERinequalities(P,env,model,x,y);
    }
  if (ERIneq==2) {
      IloInt nodeDepth=0;
      IloInt maxDepth=10;
      std::cout << " Starting resolution with ER inequalities in tree < " << maxDepth << "\n";
      cplex.use(depth(env,nodeDepth));
      cplex.use(energyCuts(env,P,x,y, 0.01, nodeDepth,maxDepth));
    }
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
      modelToSol(P,s,cplex,x,y,b);
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
    std::cout << "Iloexception in solve : " << e << std::endl;
    e.end();
    return 1;
  } 
  catch (...){
    std::cout << "Error unknown\n";
    return 1;
  }
}

//int Solve(const Problem<double>& P,Solution<double,double> &s,double time_limit,double& tps){}

//TODO _b ou véritable expression (cf. manuscrit)
int addERinequalities(const Problem<double>& P, IloEnv& env, IloModel& model, 
IloNumVarMatrix& x,IloNumVarMatrix& y){
int t, i;
  IntervalList<double> list;
  computeCheckInterval(list,P);
  Interval<double> current;
  double _b;
  for (uint I=0;I<list.size();++I){
    current=list[I];
    if (current.t1 < current.t2){
      double _B=P.totalResourceConsumption(current) ;
      for (i=0;i<P.nbTask;++i){
	_B= _B - P.A[i].resourceConsumption(current);
	_b=P.A[i].resourceConversion(P.W(i),current);
       
	IloExpr expr(env);
	for (t=0;t<=current.t1;++t)
	  expr-=x[i][t];
	for (t=current.t2;t<=P.D;++t)
	  expr-=y[i][t];
	model.add((1+expr)*_b + _B <= P.B*(current.t2 - current.t1));
	
	_b=P.A[i].resourceConversion(std::max(0.0,P.W(i)-P.A[i].Fi(P.bmax(i))*(std::max(0.0,current.t1-P.r(i)) +std::max(0.0, P.d(i) - current.t2))),current);
	model.add((x[i][P.A[i].ri]+ y[i][P.A[i].di]-1)*_b+_B <= P.B*(current.t2 - current.t1));
	
	_b=P.A[i].resourceConversion(P.A[i].leftShift(current),current);
	IloExpr expr3(env);
	for (t=current.t1+1;t<=current.t2;++t)
	  expr3+=y[i][t];
	model.add((expr3+x[i][P.A[i].ri]-1)*_b + _B <= P.B*(current.t2 - current.t1));
	expr3.end();
	
	_b=P.A[i].resourceConversion(P.A[i].rightShift(current),current);	IloExpr expr4(env);
	for (t=current.t1;t<current.t2-1;++t)
	  expr4+=x[i][t];
	model.add((expr4+ y[i][P.A[i].di]-1)*_b + _B <= P.B*(current.t2 - current.t1));
	expr4.end();
	
	_b=P.bmin(i)*(current.t2 - current.t1);
	IloExpr expr5(env);
	for (t=0;t<=current.t1;++t)
	  expr5+=x[i][t];
	for (t=current.t2;t<=P.D;++t)
	  expr5+=y[i][t];
	model.add( (expr5-1)*_b +_B <= P.B*(current.t2 - current.t1));
	expr5.end();
	_B = _B + P.A[i].resourceConsumption(current);
      }
    }
    } 
  return 0;
}


int modelToSol(const Problem<double>& P,Solution<double,double>& s,IloCplex& cplex,
IloNumVarMatrix& x ,IloNumVarMatrix& y, IloNumVarMatrix& b){for (int i=0;i<P.nbTask;++i){
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

int createModel(const Problem<double>& P,IloEnv& env ,IloModel& m ,IloNumVarMatrix& x,IloNumVarMatrix& y,IloNumVarMatrix& b,
IloNumVarMatrix& w){
return createVars(P,env,x,y,b,w) || createConstraints(P,env,m,x,y,b,w);
}


//TODO si bi ILOINT => wi ILOINT ou ILOFLOAT?
int createVars(const Problem<double>& P,IloEnv& env,
IloNumVarMatrix& x,IloNumVarMatrix& y,IloNumVarMatrix& b,
IloNumVarMatrix& w){
  IloInt i;
  const int n=P.nbTask;
  
  for (i=0;i<n;i++){
    x[i]=IloNumVarArray(env, P.D+1, 0 , 1,ILOINT);
    y[i]=IloNumVarArray(env,P.D+1, 0 , 1,ILOINT);
    
    w[i]=IloNumVarArray(env,P.D+1, 0,P.A[i].Fi.F[0].f.a*P.bmax(i)+P.A[i].Fi.F[0].f.c, ILOFLOAT);
  }
  
  char namevar[24];
  for (int i=0;i<n;i++){
    for (int t=0;t<=P.D;t++) {
      sprintf(namevar,"x_%d_%d",i,t);
      x[i][t].setName(namevar);
      sprintf(namevar,"y_%d_%d",i,t);
      y[i][t].setName(namevar);
    }	
  }
  return createB(P,env,b);
}

int createB(const Problem<double>& P,IloEnv& env,
IloNumVarMatrix& b){
  for (int i=0;i<P.nbTask;i++){
    b[i]=IloNumVarArray(env,P.D+1,0, P.bmax(i), ILOFLOAT);
  }
  return 0;
}

int createConstraints(const Problem<double>& P,IloEnv& env,IloModel& model,
		      IloNumVarMatrix& x,IloNumVarMatrix& y,IloNumVarMatrix& b,
		      IloNumVarMatrix& w){
  int i,t,tau;
  const int nbTaskInt=P.nbTask;	
  /*
   IloExpr expr(env);
  for (i=0;i<nbTaskInt;++i){
    for (t=0;t<=P.D;++t)
      expr += b[i][t];
  }
  model.add(IloMinimize(env,expr));
  expr.end();
  */
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
    for (t=0;t<P.A[i].di;t++){
      IloExpr expr(env);
      for (tau=P.A[i].ri;tau<=t;tau++)
	expr+=x[i][tau]-y[i][tau];
      model.add(expr*P.A[i].bmin<=b[i][t]);
      expr.end();
    }
  }

  //contrainte bit=0 si t < start
  for (i=0;i<nbTaskInt;i++)  {
    for (t=0;t<P.d(i);t++){
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
    for (t=0;t<=P.D;t++){
      IloExpr expr(env);
      for (tau=P.r(i);tau<=t;++tau)
	expr+=x[i][tau]-y[i][tau];
      for (int q=0;q<P.A[i].Fi.nbPiece;++q)
	model.add(P.A[i].Fi.F[q].f.a*b[i][t]+P.A[i].Fi.F[q].f.c*(expr)>=w[i][t]);
      expr.end();
    }
  }

  for (i=0; i<nbTaskInt; i++) {
    for (t=0; t<P.A[i].ri;t++)      {
	model.add(b[i][t]==0);
	model.add(x[i][t]==0);
      }
    for (t=P.A[i].di-1; t<=P.D;t++){
	model.add(b[i][t]==0);
	model.add(x[i][t]==0);
      }
    for (t=0; t<=P.A[i].ri;t++)
      model.add(y[i][t]==0);
    for (t=P.A[i].di; t<=P.D;t++)
      model.add(y[i][t]==0);
  }
  return 0;
}

