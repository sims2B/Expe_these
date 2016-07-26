#include "timeModel.h"

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
	double _B=P.totalResourceConsumption(L[l]) ;
	if (L[l].t1 < L[l].t2){
	   for (i=0;i<P.nbTask;++i){
	    double val = 0.0;
	    IloExpr expr(getEnv());	  
	    IloRange cut;
	    _B= _B - P.A[i].resourceConsumption(L[l]);
	    _b=P.A[i].resourceConversion(P.W(i),L[l]);
	    /* for (t=0;t<std::min(L[l].t1,P.D+1);++t)
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
	      }*/
	  
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




template<>
int timeModel<int,int>::createB(const Problem<int>& P,IloEnv& env,
				IloNumVarMatrix& b){
  for (int i=0;i<P.nbTask;i++){
    IloNumArray ub_b(env,P.D); 
    for (int j=P.r(i);j<P.d(i);++j)
      ub_b[j]=P.bmax(i);
    b[i]=IloNumVarArray(env,0, ub_b, ILOINT);
  }
  return 0;
}

template<>
int timeModel<double,double>::Solve(const Problem<double>& P,Solution<double,double> &s,double time_limit,int ERIneq) {
  try{
    IloNum start,time_exec;
    const int n= P.nbTask;
    //IloInt cptCut=0;
  
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
  
    else if (ERIneq==2) {
      std::cout << " Starting resolution with ER inequalities in tree <10\n";
      IloInt nodeDepth=0;
      IloInt maxDepth=10;
      cplex.use(depth(env,nodeDepth));
      cplex.use(energyCuts(env,P,x,y, 0.01, nodeDepth,maxDepth));
    }
    else 
      std::cout << " Starting resolution without ER inequalities\n";
  
    cplex.setParam(IloCplex::TiLim, time_limit);
    cplex.setParam(IloCplex::Threads,2);
    //cplex.setOut(env.getNullStream());
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

template<>
int timeModel<int,int>::modelToSol(const Problem<int> &P, 
				   Solution<int,int> &s,IloCplex& cplex,
				   IloNumVarMatrix& x,IloNumVarMatrix& y,
				   IloNumVarMatrix& b){
  for (int i=0;i<P.nbTask;++i){
    for (int t=P.r(i);t<=P.smax(i);++t){
      if (cplex.getValue(x[i][t])==1)
	s.st[i]=t;
    }
    for (int t=P.emin(i);t<P.d(i);++t){
      if (cplex.getValue(y[i][t])==1) 
	s.ft[i]=t;
    }
    if (cplex.getValue(y[i][P.d(i)])==1)  
      s.ft[i]=P.d(i);
    for (int t=P.r(i);t<P.d(i);++t)
      s.b[i][t]=(int)((double)cplex.getValue(b[i][t])+0.5);
    
  }
  for (int i=0;i<=P.D;++i)
    s.event.push_back(i);
  return 0;
}

