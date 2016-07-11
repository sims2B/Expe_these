#include "hybridBB.h"
//#include "timeModel.h"
#include "onOffModel.h"
#include <limits>
#include <iostream>
#include <vector>
#include <stack>
#include <sys/time.h>

template<typename type>
int chooseVar1(const Problem<type>& P, type epsilon){
  int i=0;
  while (i<P.nbTask){
    if (P.smax(i) - P.r(i) > epsilon) return i;
    if (P.d(i) - P.emin(i) > epsilon) return i+P.nbTask;
    ++i;
  }
  return -1;
}


template<typename type>
int chooseVar2(const Problem<type>& P,type epsilon){
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

template<typename type>
int chooseVar3(const Problem<type>& P,type epsilon){
  type min=std::numeric_limits<double>::max();
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

template<typename type>
int chooseVar4(const Problem<type>& P, type epsilon){  
  type max=0.0;
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

template<typename type>
int chooseVar5(const Problem<type>& P,type epsilon){
  type min=std::numeric_limits<double>::max();
  int j=-1;
  int i=0;
  while (i<P.nbTask){
    if ((P.smax(i)-P.r(i))/(P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i))<=min && P.smax(i) - P.r(i)>epsilon){
	  j=i;
	  min=(P.smax(i)-P.r(i))/(P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i));
	}
      if ((P.d(i)-P.emin(i))/(P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i)) <min && P.d(i) - P.emin(i)>epsilon){ 
	  j=i+P.nbTask;
	  min=(P.emin(i)-P.d(i))/(P.A[i].Fi(P.bmax(i))*(P.d(i)-P.r(i))-P.W(i));
	}
      ++i;
    }
  return j;
}

template<typename type>
int chooseVar6(const Problem<type>& P,type epsilon){
  type max=0.0;
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

template<typename type>
int testFlow(const Problem<type>& /*P*/,Solution<type,type> &/*s*/){     
  return 0;
}

template<>
void createBranch(Problem<int>& P,int x,std::stack<Problem<int>>& explore, double param);

template<typename type>
void createBranch(Problem<type>& P,int x,std::stack<Problem<type>>& explore, double param){
  Problem<type> Q(P);
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

template<typename type,typename type2= type>
int BranchBound(Problem<type>& P,Solution<type,type2>& s,ptrVar<type> choiceVar, ptrTest<type> TotalTest,  type epsilon, double param){
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
  Problem<type> P1(P);
  std::stack<Problem<type>> explore;
  explore.push(P);
  
  while (!explore.empty() && t2-t1<=7200.0) { 
    ++nbNode;
    P1=explore.top();
    explore.pop();
    if (P1.dataConsistency()) {
      if (0!=(test=TotalTest(P1))) {
	if (test ==2) ++cptAdjust;
	x=choiceVar(P1,epsilon);
	if (x!=-1) 
	  createBranch(P1,x,explore,param);
	else {
	  ++cptMIP;
	  P1.updateHorizon();
	  if (!solveOO(P1,s,timeMIP)){
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
	    return 1;
	  }
	}
      }
      else
	++cptConsistency;
    }
    gettimeofday(&tim,NULL);
    t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  }
    std::cout << "le nombre de noeud : " << nbNode<<std::endl;
    std::cout << "le nombre de MIP : " << cptMIP <<std::endl;
    std::cout << "le nombre de consistensy fail : " << cptConsistency<<std::endl;
    std::cout << "le nombre de adjust : " << cptAdjust<<std::endl;
  return 0;
}
