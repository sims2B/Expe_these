#ifndef DEF_HYBRIDBB
#define DEF_HYBRIDBB

#include "Satisfiability.hpp"
#include "Solution.hpp"
#include "timeModel.hpp"
//#include "onOffModel.h"
//#include "ttdr.h"
#include <stack>
#include <limits>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <fstream>


///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type,typename type2 =type>
using ptrTest=int(*)(Problem<type,type2>&);
template<typename type,typename type2 =type>
using ptrVar=int(*)(const Problem<type,type2>&,type);

////////////////////////////////////////////
///////////variable choice///////////////////
///////////////////////////////////////////


//lexicographic order
template<typename type,typename type2 =type>
int chooseVar1(const Problem<type,type2>&,type);
//random
template<typename type,typename type2 =type>
int chooseVar2(const Problem<type,type2>&,type);
//min domain (min=numericlimits::max)
template<typename type,typename type2 =type>
int chooseVar3(const Problem<type,type2>&,type);
//max domain (max=numericlimits::min)
template<typename type,typename type2 =type>
int chooseVar4(const Problem<type,type2>&,type);
//fi(bmax)(di-ri)-wi min
template<typename type,typename type2 =type>
int chooseVar5(const Problem<type,type2>&,type);
//fi(bmax)(di-ri)-wi max
template<typename type,typename type2 =type>
int chooseVar6(const Problem<type,type2>&,type);

///////////////////////////////////////

//le dernier parametre permet de regler l'endroit ou l'on coupe l'intervalle (0.5 on coupe a la moitié, 0.33 au 2eme tiers...)
template<typename type,typename type2 =type> 
void createBranch(Problem<type,type2>&,int,std::stack<Problem<type,type2>>&,double);

template<typename type,typename type2= type>
int BranchBound(Problem<type,type2>&, Solution<type,type2>&, ptrVar<type,type2>, ptrTest<type,type2>,type,std::ofstream&,double parameter=0.5);



///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////


template<typename type,typename type2>
int chooseVar1(const Problem<type,type2>& P, type epsilon){
  int i=0;
  while (i<P.nbTask){
    if (P.smax(i) - P.r(i) > epsilon) return i;
    if (P.d(i) - P.emin(i) > epsilon) return i+P.nbTask;
    ++i;
  }
  return -1;
}


template<typename type,typename type2>
int chooseVar2(const Problem<type,type2>& P,type epsilon){
  const uint n = P.nbTask;
  int real_size = 0;
  uint i;
  int s, j = -1;
  for (i = 0 ; i < n ; ++i) {
    if (P.smax(i) - P.r(i) > epsilon) ++real_size;
    if (P.d(i) - P.emin(i) > epsilon) ++real_size;
  }
  if (!real_size) return -1;
  if (real_size == 1) s = 1;
  else s = rand() % real_size;
  for (i = 0 ; i < n && j < 0 ; ++i) {
    if (P.smax(i) - P.r(i) > epsilon) --s;
    if (!s) j = i;
    else {
      if (P.d(i) - P.emin(i)> epsilon) --s;
      if (!s) j = i +n;
    }
  }
  return j;
}

template<typename type,typename type2>
int chooseVar3(const Problem<type,type2>& P,type epsilon){
  type min=std::numeric_limits<type>::max();
  int j=-1;
  int i=0;
  while (i<P.nbTask){
    if (P.smax(i) - P.r(i)<=min && P.smax(i) - P.r(i)>epsilon)	{
      j=i;
      min=P.smax(i) - P.r(i);
    }
    if (P.d(i) - P.emin(i) < min && P.d(i) - P.emin(i)>epsilon){ 
      j=i+P.nbTask;
      min=P.d(i) - P.emin(i);
    }
    ++i;
  }
  return j;
}

template<typename type,typename type2>
int chooseVar4(const Problem<type,type2>& P, type epsilon){  
  type max=std::numeric_limits<type>::min();
  int j=-1;
  int i=0;
  while (i<P.nbTask){
    if (P.smax(i) - P.r(i) >= max && P.smax(i) - P.r(i)>epsilon){ 
      j=i;
      max=P.smax(i) - P.r(i);
    }
    if (P.d(i) - P.emin(i) >= max && P.d(i) - P.emin(i)>epsilon)	{  
      j=i+P.nbTask;
      max=P.d(i) - P.emin(i);
    }
    ++i;
  }
  return j;
}

template<typename type,typename type2>
int chooseVar5(const Problem<type,type2>& P,type epsilon){
  type2 min=std::numeric_limits<type2>::max();
  int j=-1;
  int i=0;
  while (i<P.nbTask){
    if (P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i)<=min && P.smax(i) - P.r(i)>epsilon) {
      j=i;
      min=P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i);
    }
    if (P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i)<min && P.d(i)-P.emin(i)>epsilon){ 
      j=i+P.nbTask;
      min=P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i);
    }
    ++i;
  }
  return j;
}

template<typename type,typename type2>
int chooseVar6(const Problem<type,type2>& P,type epsilon){
  type2 max=std::numeric_limits<type2>::min();
  int j=-1;
  int i=0;
  while (i<P.nbTask){
    if (P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i)>=max && P.smax(i) - P.r(i)>epsilon) {
      j=i;
      max=P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i);
    }
    if (P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i)>max && P.d(i)-P.emin(i)>epsilon){ 
      j=i+P.nbTask;
      max=P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i);
    }
    ++i;
  }
  return j;
}


template<typename type,typename type2>
void createBranch(Problem<type,type2>& P,int x,std::stack<Problem<type,type2>>& explore, double param){
  Problem<type,type2> Q(P);
  if (x < P.nbTask){
    P.A[x].smax-=(P.smax(x)-P.r(x))*(1.0-param);
    explore.push(P);
    Q.A[x].ri+=(Q.smax(x)-Q.r(x))*param;
    Q.A[x].updateEMin();
    explore.push(Q);
  }
  else  { 
    P.A[x-P.nbTask].emin+=(P.d(x-P.nbTask)-P.emin(x-P.nbTask))*(1.0-param);
    explore.push(P);
    Q.A[x-Q.nbTask].di-=(Q.d(x-Q.nbTask)-Q.emin(x-Q.nbTask))*param;
    Q.A[x-Q.nbTask].updateSMax();
    explore.push(Q);
  }
}


template<>
void createBranch(Problem<int,int>& P,int x,std::stack<Problem<int,int>>& explore, double param);
template<>
void createBranch(Problem<int,double>& P,int x,std::stack<Problem<int,double>>& explore, double param);

template<typename type,typename type2= type>
int BranchBound(Problem<type,type2>& P,Solution<type,type2>& s,ptrVar<type,type2> choiceVar, ptrTest<type,type2> TotalTest,  type epsilon, std::ofstream& file,double param){
  struct timeval tim;
  gettimeofday(&tim,NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  double t2=0.0;
  double timeMIP=0.0;
  int cptMIP=0;
  int cptConsistency=0;
  int cptAdjust=0;
  int nbNode=0;
  int x=0;
  int test=0;
  Problem<type,type2> P1(P);
  std::stack<Problem<type,type2>> explore;
  explore.push(P);
  
  while (!explore.empty() && t2-t1<=7200.0) { 
    ++nbNode;
    P1=explore.top();
    explore.pop();
    if (P1.dataConsistency()) {
      test=TotalTest(P1);
      if (0 != test) {
	cptAdjust+=test-1;
	x=choiceVar(P1,epsilon);
	if (x!=-1) 
	  createBranch(P1,x,explore,param);
	else {
	  ++cptMIP;
	  P1.updateHorizon();
	  //	  if (!SolveConvex(P1,s)){
	    gettimeofday(&tim,NULL);
	    t2=tim.tv_sec+(tim.tv_usec/1000000.0);
	    std::cout << "Instance résolue" << std::endl;
	    std::cout << "Total time : " <<t2-t1 << "\n";
	    std::cout << "Tree time : " <<t2-t1-timeMIP << "\n";
	    std::cout << "MIP time : " <<timeMIP << "\n";
	    std::cout << "le nombre de noeud : " << nbNode<<std::endl;
	    std::cout << "le nombre de MIP : " << cptMIP <<std::endl;
	    std::cout << "le nombre de consistensy fail : " << cptConsistency<<std::endl;
	    std::cout << "le nombre de adjust : " << cptAdjust<<std::endl;
	    P1.writeInFile(file);
	    return 1;
	    //  }
	}
      }
      else
	++cptConsistency;
    }
    gettimeofday(&tim,NULL);
    t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  }
  std::cout << "Instance non résolue" << std::endl;
  std::cout << "le nombre de noeud : " << nbNode<<std::endl;
  std::cout << "le nombre de MIP : " << cptMIP <<std::endl;
  std::cout << "le nombre de consistensy fail : " << cptConsistency<<std::endl;
  std::cout << "le nombre de adjust : " << cptAdjust<<std::endl;
  return 0;
}


#endif
