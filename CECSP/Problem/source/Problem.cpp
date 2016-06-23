#include <iostream>
#include <cassert>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "Problem.h"

/*

template<>
int Problem<int>::totalEnergyConsumption(const Interval<int> &I) const{
  int conso=0;
  for (int i=0;i<nbTask;++i)
    conso+=A[i].energyConsumption(I);
  return conso;
}


template<>
int Problem<int>::totalResourceConsumption(const Interval<int> &I) const{
  int conso=0;
  for (int i=0;i<nbTask;++i){
     conso+=A[i].resourceConsumption(I); 
  }
  return conso;
}
template<>
inline int Problem<int>::energeticReasonning(const Interval<int>& I,int total) const{
return (B*(I.t2-I.t1) - total >= 0);
}





template<>
int Problem<int>::adjustmentLS(const Interval<int> &I, int i, int total){
  const int capacity=B*(I.t2-I.t1);
  int  SL=capacity-total+A[i].resourceConsumption(I);
  int LS=A[i].resourceConversion(A[i].leftShift_adjust(I),I);
  assert(total <= capacity );
  if ( SL-LS< 0) {
    A[i].emin=std::max(emin(i),I.t2+(LS-SL)/bmax(i));
    if (bmin(i)!=0)
      A[i].ri=std::max(r(i),I.t2-(int)ceil(SL/(double)bmin(i)));
    A[i].updateEMin();  
    return 1;
  }
  return 0;
}


template<> 
int Problem<int>::adjustmentRS(const Interval<int> &I,int i, int total){
  const int capacity=B*(I.t2-I.t1);
  int SL=capacity-total+A[i].resourceConsumption(I);
  int RS=A[i].resourceConversion(A[i].rightShift_adjust(I),I);
  assert(total <= capacity );
    if (SL-RS< 0) {
      A[i].smax=std::min(smax(i),I.t1-(RS-SL)/bmax(i));
      if (bmin(i)!=0)
	A[i].di=std::min(d(i),I.t1+(int)ceil(SL/(double)bmin(i)));
      A[i].updateSMax();  
      return 1;
  }
  return 0;
}*/
