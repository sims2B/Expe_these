#include "satisfiability.h"

template<typename type>
int intervalTest(const Problem<type>& P){
  IntervalList<type> listInt;
  listInt.reserve(4*P.nbTask);
  computeCheckInterval(listInt,P);
  type total;
  for (uint i=0;i<listInt.size();++i){
    total=P.totalResourceConsumption(listInt[i]);
    if (listInt[i].t1 < listInt[i].t2 && !P.energeticReasonning(listInt[i],total))
      return 0; 
  } 
  return 1;
}

template<typename type>
int intervalTotalAdjust(Problem<type>& P)  {
  int performed=0;
  IntervalList<type> listInt;
  listInt.reserve(4*P.nbTask);
  computeCheckInterval(listInt,P);
  type total;
  for (uint i=0; i<listInt.size();++i){
    if (listInt[i].t1 < listInt[i].t2){
      total=P.totalResourceConsumption(listInt[i]);
      for (int j=0 ; j < P.nbTask ; ++j ){
	if (P.adjustmentLS(listInt[i],j,total))
	  ++performed;
	if (P.adjustmentRS(listInt[i],j,total))
	  ++performed;
      }
    }
  } 
  for (int j=0 ; j < P.nbTask ; ++j ){
    listInt.clear();
    computeRSInterval(listInt,P,j);
    for (uint i=0; i<listInt.size();++i){
      if (P.adjustmentRS(listInt[i],j,total))
	++performed;
    }
    listInt.clear();
    computeLSInterval(listInt,P,j);
    for (uint i=0; i<listInt.size();++i){   
      if (P.adjustmentLS(listInt[i],j,total))
	++performed;
    }
  }   
  return performed;
}



template<typename type>
int intervalTotalTest(Problem<type>& P){
  IntervalList<type> listInt;
  listInt.reserve(4*P.nbTask);
  computeCheckInterval<type>(listInt,P);
  type total;
  int adjust=0;
  for (uint i=0; i<listInt.size();++i){
    total=P.totalResourceConsumption(listInt[i]);
    if (!P.energeticReasonning(listInt[i],total))
      return 0;
    else for (int j=0;j<P.nbTask;++j){
	if (P.adjustmentLS(listInt[i],j,total))
	  ++adjust;
	if (P.adjustmentRS(listInt[i],j,total))
	  ++adjust;
      }
  } 
  for (int i=0;i<P.nbTask;++i){
    IntervalList<type> LA;
    computeLSInterval(LA,P,i); 
    for (uint j=0; j<LA.size();++j){
      total =P.totalResourceConsumption(LA[j]);  
      /*if (!P.energeticReasonning(LA[j],total))
	return 0;*/
      if (P.adjustmentLS(LA[j],i,total))
	++adjust;
    }
    IntervalList<type> RA;
    computeRSInterval(RA,P,i);
    for (uint j=0; j<RA.size();++j){
      total =P.totalResourceConsumption(RA[j]);  
      /*      if (!P.energeticReasonning(RA[j],total))
	      return 0;*/
      if (P.adjustmentRS(RA[j],i,total))
	++adjust;  
    }
  }
  if (adjust)   return (adjust+1);
  else  return 1;
}

