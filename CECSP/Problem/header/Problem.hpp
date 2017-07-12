#ifndef DEF_PROBLEM
#define DEF_PROBLEM

#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "Task.hpp"


///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type,typename type2 =type>
using TaskList=std::vector<Task<type,type2>>;

//structure defining a Problem as a number of task, a vector of task (Tasklist), 
//a resource capacity and an upper and lower bound on  the time horizon
template<typename type,typename type2 =type>
struct Problem {
  //giving a number of task, construct an instance of type problem, with B=R=D=0
  //and reserve a vector of size nbtask
  Problem(int);
  //same as Problem(int) with the capacity of the resource
  Problem(int,type2);
  //giving a number of task, a resouce capacity and a list of task, construct the
  //associated instance of Problem
  Problem(int,type2,TaskList<type,type2>);
  //copy constructor
  Problem(const Problem&);

  //update of the upper and lower bound on the horizon (in case of update on 
  //task time window)
  void updateHorizon();
 
  //read an instance of Problem inclded in a file and set all the parameter to 
  //the correct value (do not construct the instance, use the constructor before)
  void readFromFile(std::ifstream&); 
  //generate random concave piecewise functions for all task and set Wi to a 
  //random number in [1/2*fi(bmax) * (di-ri),fi(bmax) * (di-ri)] 
  void addConcavePiecewiseFunction();
  //generate random convex piecewise functions for all task and set Wi to a 
  //random number in [1/2*fi(bmax) * (di-ri),fi(bmax) * (di-ri)]
  void addConvexPiecewiseFunction();
  //write the instance of Problem in a file
  void writeInFile(std::ofstream& new_inst) const;

  inline type r(int i) const {return A[i].ri;}
  inline type d(int i) const {return A[i].di;}
  inline type2 bmin(int i) const {return A[i].bmin;}
  inline type2 bmax(int i) const {return A[i].bmax;}
  inline type emin(int i) const {return A[i].emin;}
  inline type smax(int i) const {return A[i].smax;}
  inline type2 W(int i) const {return A[i].Wi;}
  inline Function<type2> f(int i) const {return A[i].Fi;}

  Problem<type,type2>& operator=(const Problem<type,type2>& P);

  void displayProblem() const;

  // check the data consistency (energy demand not too high comparing to the 
  // time windows)
  int dataConsistency() const;

  //giving an interval I, compute the some of all task mandatory energy 
  //consumption inside I
  type2 totalEnergyConsumption(const Interval<type2>&) const;
  //giving an interval I, compute the some of all task mandatory energy 
  //consumption inside I
  type2 totalResourceConsumption(const Interval<type2>&) const;
  inline int energeticReasonning(const Interval<type2>& I,type2 total) const
  {return (B*(I.t2-I.t1) - total>= NEGATIVE_ZERO);}

  //vérifier ajustement !!!
  int adjustmentLS(const Interval<type2>&,int,type2);
  int adjustmentRS(const Interval<type2>&,int,type2);
  int totalTest(const Interval<type2>&);

  int nbTask;
  type2 B;
  type R;
  type D;
  TaskList<type,type2> A;
};


///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////
template<typename type,typename type2>
Problem<type,type2>::Problem(int _nbTask) : nbTask(_nbTask) {
  A.reserve(nbTask);
}

template<typename type,typename type2>
Problem<type,type2>::Problem(int _nbTask,type2 _B) : nbTask(_nbTask), B(_B) {
  A.reserve(nbTask);
}

template<typename type,typename type2>
Problem<type,type2>::Problem(int _nbTask, type2 _B, TaskList<type,type2> _A) : nbTask(_nbTask) , B(_B) , A(_A) {
  R=r(0);
  D=d(0);
  for (int i=1;i<nbTask;++i){
    R = ((R > r(i))?  r(i) : R);
    D = ((D < d(i))?  d(i) : D);
  }
}


template<typename type,typename type2>
Problem<type,type2>::Problem(const Problem<type,type2> &P)  : nbTask(P.nbTask) , B(P.B) , R(P.R) , D(P.D), A(P.A)  {}

template<typename type,typename type2>
void Problem<type,type2>::updateHorizon(){
  R=r(0);
  D=d(0);
  for (int i=1;i<nbTask;++i){
    R = ((R > r(i))?  r(i) : R);
    D = ((D < d(i))?  d(i) : D);
  }
}

template<typename type,typename type2>
void Problem<type,type2>::readFromFile(std::ifstream& instance){
  int i,_nbPiece;
  type2 _B,_Wi,_bmin,_bmax,_a,_c,t1,t2;
  type _ri,_di;
  instance >> _B;
  B=_B;
  for (i=0;i<nbTask;++i){
    instance >> _ri >> _di >> _Wi >> _bmin >> _bmax >>_nbPiece;
    instance >> _a >> _c;
    t1=_bmin;
    if (_nbPiece==1) 
      t2=_bmax;
    else
      instance >> t2;	
    Piece<type2> _P1(Interval<type2>(t1,t2),LinearFunction<type2>(_a,_c));
    PieceList<type2> P(1,_P1);
    t1=t2;
    for (int q=1;q<_nbPiece-1;++q) {
      instance >> _a >> _c >> t2;
      Piece<type2> _P(Interval<type2>(t1,t2),LinearFunction<type2>(_a,_c));
      P.push_back(_P);
      t1=t2;
    }
    if (_nbPiece!=1){
    instance >> _a >> _c;
    Piece<type2> _P(Interval<type2>(t1,_bmax),LinearFunction<type2>(_a,_c));
    P.push_back(_P);
    }
    Function<type2> _F(P);
    Task<type,type2> T(_ri,_di,_Wi,_bmin,_bmax,_F);
    A.push_back(T);	
    
  }
  this->updateHorizon();
}

template<typename type,typename type2>
void Problem<type,type2>::addConcavePiecewiseFunction(){  
  int min;
  int max;
  srand(rdtsc());
  for (int i=0;i<nbTask;++i) {
    A[i].addConcavePiecewiseFunction();
    min=(int)(A[i].Fi(bmin(i))*(d(i)-r(i)));
    max=(int)(A[i].Fi(bmax(i))*(d(i)-r(i))* 0.5);
    if (max-min > POSITIVE_ZERO && max-min < NEGATIVE_ZERO)
      A[i].Wi=min + rand()%(max-min);
    else  
      A[i].Wi=min;
  }
}
template<typename type,typename type2>
void Problem<type,type2>::addConvexPiecewiseFunction(){  
  int min;
  int max;
  srand(rdtsc());
  for (int i=0;i<nbTask;++i) {
    A[i].addConvexPiecewiseFunction();
    min=(int)(A[i].Fi(bmin(i))*(d(i)-r(i)));
    max=(int)(A[i].Fi(bmax(i))*(d(i)-r(i))* 0.5);
    if (max-min > POSITIVE_ZERO && max-min < NEGATIVE_ZERO)
      A[i].Wi=min + rand()%(max-min);
    else  
      A[i].Wi=min;
  }
}



template<typename type,typename type2>
void Problem<type,type2>::writeInFile(std::ofstream& new_inst) const{
  new_inst << nbTask << " " << B << std::endl;
  for (int i=0;i<nbTask;++i) {
    int P=A[i].Fi.nbPiece;
    new_inst << A[i].ri << " " 
	     << A[i].di << " " 
	     << A[i].Wi << " "
	     << A[i].bmin << " " 
	     << A[i].bmax << " " 
	     << P << " "
	     << A[i].Fi.F[0].f.a << " " 
	     << A[i].Fi.F[0].f.c;
    if (A[i].Fi.nbPiece==1)
      new_inst << std::endl;
    else{
      new_inst << " " << A[i].Fi.F[1].I.t1;
      for (int j=1;j<P-1;++j){
	new_inst << " " << A[i].Fi.F[j].f.a 
		 << " " << A[i].Fi.F[j].f.c 
		 << " " << A[i].Fi.F[j].I.t2;
      }
      new_inst << " " << A[i].Fi.F[P-1].f.a 
	       << " " << A[i].Fi.F[P-1].f.c 
	       << std::endl ;
    }
  }
}

template<typename type,typename type2>
Problem<type,type2>& Problem<type,type2>::operator=(const Problem<type,type2>& P){
  nbTask=P.nbTask;
  B=P.B;
  A=P.A;
  return *this;
}

template<typename type,typename type2>
void Problem<type,type2>::displayProblem() const{
  if (nbTask >0){
    std::cout << "L'instance est composée de " << nbTask 
	      << " tâches et la ressource est de capacité " << B << std::endl;
    std::cout << "************************************************" << std::endl;
    for (int i=0;i<nbTask; i++){ 
      std::cout << "La tâche "<< i+1 << " possède les attributs suivants :" << std::endl;
      A[i].displayTask();
      std::cout << std::endl;
      std::cout << "*************************************************" << std::endl;
    }
  }
  else std::cout << "l'instance est vide";
}

template<typename type,typename type2> 
int Problem<type,type2>::dataConsistency() const{
  for (int i=0;i<nbTask;++i)
    if (!A[i].dataConsistency())
      return 0;
  return 1;
}


template<typename type,typename type2> 
type2 Problem<type,type2>::totalEnergyConsumption(const Interval<type2> &I) const{
  type2 conso=0.0;
  for (int i=0;i<nbTask;++i)
    conso+=A[i].energyConsumption(I);
  return conso;
}


template<typename type,typename type2>
type2 Problem<type,type2>::totalResourceConsumption(const Interval<type2> &I) const{
  type2 conso=0.0;
  for (int i=0;i<nbTask;++i)
    conso+=A[i].resourceConsumption(I); 
  return conso;
}

//attention au floor/ceil pour le cas entier
template<typename type,typename type2>
int Problem<type,type2>::adjustmentLS(const Interval<type2> &I,int i,type2 total){
  int performed=0;
  if (I.t2 < d(i)){
    const type2 capacity=B*(I.t2-I.t1);
    type2  SL,LS/*,LSboth*/; 
    SL=capacity-total+A[i].resourceConsumption(I);
    LS=A[i].resourceConversion(A[i].leftShift(I),I);
    //    LSboth=std::min(LS,A[i].resourceConversion(A[i].bothShift(I),I));
    if (SL+POSITIVE_ZERO < LS) {
      type temp=emin(i);
      A[i].emin=std::max(emin(i),(type)I.t2+(LS-SL)/bmax(i));
      if (A[i].emin != temp)
	performed=1;
    }
    if (SL + POSITIVE_ZERO < LS && bmin(i)!=0.0) {
      type temp=r(i);
      A[i].ri=std::max(r(i),(type)I.t2-SL/bmin(i));  
      if (A[i].ri != temp)
	performed=1;
    }
  }
  return performed;
}


template<typename type,typename type2> 
int Problem<type,type2>::adjustmentRS(const Interval<type2> &I,int i,type2 total){
  int performed=0;
  if (I.t1 > r(i)){
    const type2 capacity=B*(I.t2-I.t1);
    type2 SL,RS/*,RSboth*/;  
    SL=capacity-total+A[i].resourceConsumption(I);
    RS=A[i].resourceConversion(A[i].rightShift(I),I);
    //RSboth=std::min(RS,A[i].resourceConversion(A[i].bothShift(I),I));
    if (SL- RS< NEGATIVE_ZERO) {
      type temp=smax(i);
      A[i].smax=std::min(smax(i),(type)I.t1-(RS-SL)/bmax(i));
      if ( A[i].smax != temp)
	performed=1;
    }
    if (bmin(i)!=0.0 && SL-RS < NEGATIVE_ZERO) {
      type temp=d(i);
      A[i].di=std::min(d(i),(type)I.t1+SL/bmin(i)); 
      if (A[i].di != temp)
	performed=1;
    }
  }
  return performed;
}



template<typename type,typename type2>
int Problem<type,type2>::totalTest(const Interval<type2>& current){
  const type2 total=totalResourceConsumption(current);
  if (!energeticReasonning(current,total))
    return 0;
  else {
    adjustmentLS(current,total);
    adjustmentRS(current,total);
  }
  return 1;
}

#endif
