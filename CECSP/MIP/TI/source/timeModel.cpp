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


ILOUSERCUTCALLBACK6(energyCuts, const Problem<type>&, P, IloEnv&, env, 
		    IloModel, &model,IloNumVarMatrix&, x,
		    IloNumVarMatrix&,  y, IloNum, eps, IloInt&, nbCut, IloInt&, nodeDepth, IloInt , depthMax) {
  if ( !isAfterCutLoop() )
    return;

  if ( nodeDepth <= depthMax && nbCut < USERCUTLIMIT ) {    
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

