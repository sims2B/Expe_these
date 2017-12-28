#include "timeModel.hpp"

int createVars(const Problem<int, int>& P, IloEnv& env, IloNumVarMatrix& x, 
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
    b[i]=IloNumVarArray(env,0, ub_b, ILOINT);
    for (j=0;j<P.D;++j)
      if ( j==P.r(i) || j<P.d(i)) ub_w[j]=P.A[i].Fi(P.bmax(i));
      else ub_w[j]=0;
    w[i]=IloNumVarArray(env,0,ub_w, ILOFLOAT);
  }
  return 0;
}

int modelToSol(const Problem<int,int> &P, Solution<int,int> &s,IloCplex& 
			cplex,IloNumVarMatrix& x,IloNumVarMatrix& y, IloNumVarMatrix& b){
  int i,t;
  std::cout << "!!!!ATTETNTION !!!! peut etre une erreur d'arondi\n";
  for (i=0;i<P.nbTask;++i){
    for (t=P.r(i);t<=P.smax(i);++t)
      if (cplex.getValue(x[i][t])==1)
	s.st[i]=t;
    for (t=P.emin(i)-1;t<P.d(i);++t)
      if (cplex.getValue(y[i][t])==1) 
	s.ft[i]=t+1;
    for (t=P.r(i);t<P.d(i);++t)
      s.b[i][t]=(int)((double)cplex.getValue(b[i][t])+0.5);
  }
  for (i=0;i<P.D+1;++i)
    s.event.push_back(i);
  return 0;
}

int modelToSol(const Problem<int,double> &P, Solution<int,double> &s,IloCplex& 
	       cplex,IloNumVarMatrix& x,IloNumVarMatrix& y, IloNumVarMatrix& b){
  int i,t;
  std::cout << "!!!!ATTETNTION !!!! peut etre une erreur d'arondi\n"; 
  for (i=0 ; i < P.nbTask ; ++i){
    for (t= P.r(i) ; t <= P.smax(i) ; ++t){   
      if (isEqual((double)cplex.getValue(x[i][t]),1.0))
	s.st[i] = t;
    }
    for (t = P.emin(i)-1 ; t < P.d(i) ; ++t) {    
      if (isEqual((double)cplex.getValue(y[i][t]),1.0))
	s.ft[i] = t+1;
    }
    for (t=P.r(i);t<P.d(i);++t)
      s.b[i][t] = cplex.getValue(b[i][t]);
  }
  for (i=0;i<P.D+1;++i)
    s.event.push_back(i);
  return 0;
}


ILOINCUMBENTCALLBACK2(getFirstSolInfo, IloInt&, cpt, IloNum, startTime){
  if (cpt <1){
    std::cout << "Sol n°" << cpt+1 << " obj: \t" << getObjValue() << std::endl;
    std::cout << "Sol n°" << cpt+1 << " time: \t" << getCplexTime()-startTime << std::endl;
    std::cout << "Sol n°" << cpt+1 << " gap: \t" << getMIPRelativeGap() << std::endl;
    cpt=cpt+1;
  }
}




/*ILONODECALLBACK1(depth, IloInt& , nodeDepth) {
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
