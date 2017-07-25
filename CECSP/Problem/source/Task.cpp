#include "Task.hpp"
int rdtsc(){
    __asm__ __volatile__("rdtsc");
    return 0;
}


template<>
Task<int,int>::Task(int _ri, int _di, int _Wi, int _bmin, int _bmax, PieceList<int> P) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(P){
  emin=ri + ceil((double)Wi/(double)Fi(bmax));
  smax=di - ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int,double>::Task(int _ri, int _di, double _Wi, double _bmin, double _bmax, PieceList<double> P) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(P){
  emin=ri + ceil((double)Wi/(double)Fi(bmax));
  smax=di - ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int>::Task(int _ri,int _di,int _Wi,int _bmin, int _bmax,Function<int> _Fi) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+ceil((double)_Wi/(double)_Fi(_bmax));
  smax=di-ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int,double>::Task(int _ri,int _di,double _Wi,double _bmin, double _bmax,Function<double> _Fi) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+ceil((double)_Wi/(double)_Fi(_bmax));
  smax=di-ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>  
void Task<int>::updateEMin(){
  emin = std::max(emin,ri + (int)ceil((double)Wi/(double)Fi(bmax)));
}
template<>  
void Task<int,double>::updateEMin(){
  emin = std::max(emin,ri + (int)ceil((double)Wi/(double)Fi(bmax)));
}

template<> 
void Task<int>::updateSMax() 
{smax = std::min(smax,di - (int)ceil((double)Wi/(double)Fi(bmax)));
}
template<> 
void Task<int,double>::updateSMax() 
{smax = std::min(smax,di - (int)ceil((double)Wi/(double)Fi(bmax)));
}


template<>
int Task<int,int>::resourceConversionConcave(const int& energy,const Interval<int> &I) const{  const int J(sizeIntersection(I,Interval<int>(ri,di)));
  if ( bmin!=0.0 && energy <= Fi(bmin) * J) 
    return bmin* (int)ceil((double)energy/(double)Fi(bmin));
  else {  
    int max=0.0;
    for (int q=0;q<Fi.nbPiece;++q) 
      max= std::max((int)ceil((double)(energy - J *Fi.F[q].f.c)/(double)Fi.F[q].f.a),
		    max);
    return max;
  }
}

template<>
double Task<int,double>::resourceConversionConcave(const double& energy,const Interval<int> &I) const{ 
  // std::cout << "ri = " << ri << " et di = "<< di << std::endl;
  const int J(sizeIntersection(I,Interval<int>(ri,di)));
  if ( bmin!=0.0 && energy <= Fi(bmin) * J) 
    return bmin* (int)ceil((double)energy/(double)Fi(bmin));
  else {  
    double max=0.0;
    for (int q=0;q<Fi.nbPiece;++q) 
      max= std::max((energy - J *Fi.F[q].f.c)/Fi.F[q].f.a,
		    max);
    return max;
  }
}

template<>
int Task<int,int>::resourceConversionConvex(const int& energy,const Interval<int> &I) const{ const int J(sizeIntersection(I,Interval<int>(ri,di)));
  if (bmin!=0.0 &&  energy <= Fi(bmin) * J) 
    return bmin* (int)ceil((double)energy/(double)Fi(bmin));
  else {  
    IloEnv env;
    IloModel model(env);
    const int Q= Fi.nbPiece;
    int q;
    IloNumVarArray lambda(env,Q+1,0,J,ILOINT);
  
    IloExpr expr(env);
    for (q=0 ; q < Q ; ++q){
      expr += lambda[q] * Fi.F[q].I.t1;
    }
    expr += lambda[Q] * Fi.F[Q-1].I.t2;
    model.add(IloMinimize(env,expr));
    expr.end();

    IloExpr expr2(env);
    for (q=0 ; q<Q ; ++q){
      expr2 += lambda[q] * Fi(Fi.F[q].I.t1);
    }
    expr2 += lambda[Q] * Fi(Fi.F[Q-1].I.t2);
    model.add(expr2 >= energy);
    expr2.end();
 
    IloExpr expr3(env);
    for (q=0 ; q<Q+1 ; ++q){
      expr3 += lambda[q];
    }
    model.add(expr3 <= J);
    expr3.end();
    
    IloCplex cplex(model);
    cplex.setOut(env.getNullStream());
    if (cplex.solve())
      return cplex.getObjValue();
    else {
      std::cout << " houston nous avons un problèmeuh " << std::endl;
      return std::numeric_limits<int>::min();
    }
  }
}

/*template<>
double Task<int,double>::resourceConversionConvex(const double& energy,const Interval<int> &I) const {
    std:: cout << "*********début calcul conversion resource*****\n";
  displayTask();
  I.displayInterval();
  std::cout << "energy " << energy << std::endl;
  std:: cout << "size J " << J << std::endl;
  const int J=sizeIntersection(I,Interval<int>(ri,di));
  if (bmin!=0.0 &&  energy <= Fi(bmin) * J) 
    return bmin*(int)ceil((double)energy/(double)Fi(bmin));
  else {  
    IloEnv env;
    IloModel model(env);
    int t;
    IloNumVarArray bi(env, J,bmin,bmax,ILOFLOAT);
    IloNumVarArray wi(env,J,Fi(bmin),Fi(bmax),ILOFLOAT);
    
    IloExpr expr(env);
    for (t=0 ; t < J ; ++t)
      expr += bi[t] ;
    model.add(IloMinimize(env,expr));
    expr.end();

    IloNumArray bp(env,Fi.nbPiece-1); 
    IloNumArray slope(env,Fi.nbPiece);
    for (t=0;t<Fi.nbPiece;++t)
      slope[t]=Fi.F[t].f.a;
    for (t=0;t<Fi.nbPiece-1;++t)
      bp[t]=Fi.F[t+1].I.t1;
    for (t=0 ; t < J ; ++t)
      model.add(wi[t]==
		IloPiecewiseLinear(bi[t],bp,slope,bmin,Fi(bmin)));
    IloExpr expr2(env);
    for (t=0 ; t < J ; ++t)
      expr2 +=  wi[t];
    model.add(expr2 >= energy);
    expr2.end();
    
    IloCplex cplex(model);
    cplex.setOut(env.getNullStream());

    if (cplex.solve()){
      //   std:: cout << "*********fin calcul conversion resource*****\n";
      return cplex.getObjValue();    
    }
    else {
      std::cout << " houston nous avons un problèmeeeeeeeeuuuh " << std::endl;
      //std:: cout << "*********fin calcul conversion resource*****\n";
      return std::numeric_limits<int>::min();
    }
  }
}
*/
