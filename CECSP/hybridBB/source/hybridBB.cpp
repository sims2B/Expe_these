#include "hybridBB.hpp" 

template<> 
void createBranch(Problem<int,int>& P,int x,std::stack<Problem<int,int>>& explore,double param){ 
  Problem<int,int> Q(P);
  if (x < P.nbTask){
    P.A[x].smax-=(int)(ceil((P.smax(x)-P.r(x))*(1.0-param)));
    explore.push(P);
    Q.A[x].ri+=(Q.smax(x)-Q.r(x))*param;
    Q.A[x].updateEMin();
    explore.push(Q);
  }
  else  { 
    P.A[x-P.nbTask].emin+=(int)(ceil((P.d(x-P.nbTask)-P.emin(x-P.nbTask))*(1.0-param)));
    explore.push(P);
    Q.A[x-Q.nbTask].di-=(Q.d(x-Q.nbTask)-Q.emin(x-Q.nbTask))*param;
    Q.A[x-Q.nbTask].updateSMax();
    explore.push(Q);
  }
}

template<> 
void createBranch(Problem<int,double>& P,int x,std::stack<Problem<int,double>>& explore,double param){ 
  Problem<int,double> Q(P);
  if (x < P.nbTask){
    P.A[x].smax-=(int)(ceil((P.smax(x)-P.r(x))*(1.0-param)));
    explore.push(P);
    Q.A[x].ri+=(Q.smax(x)-Q.r(x))*param;
    Q.A[x].updateEMin();
    explore.push(Q);
  }
  else  { 
    P.A[x-P.nbTask].emin+=(int)(ceil((P.d(x-P.nbTask)-P.emin(x-P.nbTask))*(1.0-param)));
    explore.push(P);
    Q.A[x-Q.nbTask].di-=(Q.d(x-Q.nbTask)-Q.emin(x-Q.nbTask))*param;
    Q.A[x-Q.nbTask].updateSMax();
    explore.push(Q);
  }
}


