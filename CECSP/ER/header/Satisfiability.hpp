#ifndef DEF_SATISFIABILITY
#define DEF_SATISFIABILITY
#include <vector>

#include "ListInterval.hpp"

#include <iostream>
#include <fstream>
///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

//checker (return 0 if ER fails)
template<typename type,typename type2=type>
int intervalTest(const Problem<type,type2>&);
//filter n (return nbAdjust)
template<typename type,typename type2=type>
int intervalTotalAdjust(Problem<type,type2> &, std::ofstream&);
//checker+filter
//return 0 if the ER fails, and nbAdjust+1 otherwise
template<typename type,typename type2=type>
int intervalTotalTest(Problem<type,type2> &);

///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type,typename type2>
int intervalTest(const Problem<type,type2>& P){
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

template<typename type,typename type2>
int intervalTotalAdjust(Problem<type,type2>& P,std::ofstream& file)  {
  int performed=0;
  IntervalList<type> listInt;
  listInt.reserve(4*P.nbTask);
  computeCheckInterval(listInt,P);
  type total;
  for (uint i=0; i<listInt.size();++i){
    total=P.totalResourceConsumption(listInt[i]);
    for (int j=0 ; j < P.nbTask ; ++j ){
      if (P.adjustmentEmin(listInt[i],j,total))
	++performed;
      if (P.adjustmentRi(listInt[i],j,total))
	++performed;
      if (P.adjustmentSmax(listInt[i],j,total))
	++performed;
      if (P.adjustmentDi(listInt[i],j,total))
	++performed;
      P.writeInFile(file);
    }
  } 
  for (int j=0 ; j < P.nbTask ; ++j ){
    listInt.clear();
    computeRSInterval(listInt,P,j);
    for (uint i=0; i<listInt.size();++i){
      total=P.totalResourceConsumption(listInt[i]);
      if (P.adjustmentSmax(listInt[i],j,total))
	++performed;
      if (P.adjustmentDi(listInt[i],j,total))
	++performed;
    }
    listInt.clear();
    computeLSInterval(listInt,P,j);
    for (uint i=0; i<listInt.size();++i){   
      total=P.totalResourceConsumption(listInt[i]);
      if (P.adjustmentRi(listInt[i],j,total))
	++performed;
      if (P.adjustmentEmin(listInt[i],j,total))
	++performed;
    }
  }
  return performed;
}



template<typename type,typename type2>
int intervalTotalTest(Problem<type,type2>& P){
 IntervalList<type> listInt;
  listInt.reserve(4*P.nbTask);
  computeCheckInterval<type>(listInt,P);
  type total;
  int adjust=0;
  for (uint i=0; i<listInt.size();++i){
    total=P.totalResourceConsumption(listInt[i]);
    //listInt[i].displayInterval(); 
    if (!P.energeticReasonning(listInt[i],total))
      return 0;
    else for (int j=0;j<P.nbTask;++j){
	if (P.adjustmentEmin(listInt[i],j,total))
	  ++adjust;
	if (P.adjustmentRi(listInt[i],j,total))
	  ++adjust;
	if (P.adjustmentSmax(listInt[i],j,total))
	  ++adjust;
	if (P.adjustmentDi(listInt[i],j,total))
	  ++adjust;
      }
  }
  for (int i=0;i<P.nbTask;++i){
    IntervalList<type> LA;
    computeLSInterval(LA,P,i); 
    for (uint j=0; j<LA.size();++j){
      total=P.totalResourceConsumption(LA[j]);
      if (P.adjustmentEmin(LA[j],i,total))
	++adjust;
      if (P.adjustmentRi(LA[j],i,total))
	++adjust;
    }
    IntervalList<type> RA;
    computeRSInterval(RA,P,i);
    for (uint j=0; j<RA.size();++j){
      total=P.totalResourceConsumption(RA[j]);
      if (P.adjustmentSmax(RA[j],i,total))
	++adjust; 
      if (P.adjustmentDi(RA[j],i,total))
	++adjust; 
    }
  }
  //  std::cout << "  " <<adjust << '\n';  

  return (adjust+1);
}


#endif
