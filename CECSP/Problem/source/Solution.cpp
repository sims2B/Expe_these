#include "Solution.hpp"
#include <algorithm>


template<>  
int Solution<int,double>::isValid(const Problem<int,double> &P) const{
  const int nbTask=st.size();
  const int E=event.size();
  int e,i;
  double sum=0.0;
  double energy=0.0;
  for (i=0;i<nbTask;++i) {
    if (P.r(i) > st[i]) return 0;
    if (P.d(i) < ft[i]) return 0;
  }  
  for (i=0;i<nbTask;++i) {
    for (e=0;e<E-1;++e){
      if ( event[e] < st[i] || event[e] >= ft[i] ){
	if (b[i][e]>POSITIVE_ZERO && P.bmin(i)!=0) return 0;
      }
    }
  } 
  for (i=0;i<nbTask;++i) {
    energy=0.0; 
    for (e=0;e<E-1;++e){
      if (event[e+1] != event[e]  && event[e] > st[i] && event[e] < ft[i] &&
	  (b[i][e] < P.bmin(i) + NEGATIVE_ZERO || 
	   b[i][e] > P.bmax(i) + POSITIVE_ZERO) )
	return 0;
    }

    for (e=0;e<E-1;++e){
      if (!isEqual(b[i][e],0.0) ) 
	energy+=(P.A[i].Fi.F[0].f.a*b[i][e]+P.A[i].Fi.F[0].f.c)*(event[e+1]-event[e]);
    }	
    if (energy < P.W(i) + NEGATIVE_ZERO) return 0;	 
  }
  for (e=0;e<E-1;++e) {
    sum=0.0;
    for (i=0;i<nbTask;++i)
      sum+=b[i][e];
    if (sum >P.B + POSITIVE_ZERO) return 0;
  }
  return 1;
}

template<>  
int Solution<int,int>::isValid(const Problem<int,int> &P) const{
  const int nbTask=P.nbTask;
  const int E=event.size();
  int e,i;
  int sum=0;
  int energy=0;
  for (i=0;i<nbTask;++i) {
    if (P.r(i) > st[i] ) return 0;
    if (P.d(i) < ft[i]) return 0;
  }
  for (i=0;i<nbTask;++i) {
    for (e=0;e<E-1;++e){
      if (event[e]<st[i] || event[e]>=ft[i]){
	if (b[i][e]>0 && P.bmin(i)!=0) return 0;
      }
    }
  }
  
  for (i=0;i<nbTask;++i) {
    energy=0; 
    for (e=0;e<E-1;++e){
      if ((b[i][e] < P.bmin(i) || 
	   b[i][e] > P.bmax(i)) && 
	  event[e+1]!=event[e])
	return 0;
      if (b[i][e]!=0)
	energy+=(P.A[i].Fi.F[0].f.a*b[i][e]+P.A[i].Fi.F[0].f.c)*(event[e+1]-event[e]);
    }	
    if (energy < P.W(i)) return 0;	 
  }

  for (e=0;e<E-1;++e) {
    sum=0;
    for (i=0;i<nbTask;++i)
      sum+=b[i][e];
    if (sum  > P.B ) return 0;
  }
  return 1;
}
