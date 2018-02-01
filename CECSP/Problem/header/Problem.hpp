#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
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
  int readtimeWindows(std::ifstream&);
  //generate random linear functions for all task  
  void addLinearFunction();
  //generate random concave piecewise functions for all task and set Wi to a 
  //random number in [1/2*fi(bmax) * (di-ri),fi(bmax) * (di-ri)] 
  void addConcavePiecewiseFunction();
  //generate random convex piecewise functions for all task and set Wi to a 
  //random number in [1/2*fi(bmax) * (di-ri),fi(bmax) * (di-ri)]
  void addConvexPiecewiseFunction();
  //write the instance of Problem in a file
  void writeInFile(std::ofstream& new_inst) const;
  //write the time windows of each task (ri, smax, emin, di) in a file
  void writeTWInFile(std::ofstream& new_inst) const;

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
  type2 totalEnergyConsumption(const Interval<type>&) const;
  //giving an interval I, compute the some of all task mandatory energy 
  //consumption inside I
  type2 totalResourceConsumption(const Interval<type>&) const;
  //return 0 if the ER in interval I fails
  inline int energeticReasonning(const Interval<type>& I,type2 total) const
  {int res=(B*(I.t2-I.t1) - total>= NEGATIVE_ZERO);
    /*    if (!res) {
      displayProblem();
      std::cout << "Interval qui chie: " ;
      I.displayInterval();
      std:: cout << "et res req= "<< total << std::endl;}
    */return res;
  }

  //vérifier ajustement !!!
  int adjustmentEmin(const Interval<type>&,int,type2);
  int adjustmentRi(const Interval<type>&,int,type2);
  int adjustmentSmax(const Interval<type>&,int,type2);
  int adjustmentDi(const Interval<type>&,int,type2);
  int totalTest(const Interval<type>&);

  int nbTask;
  type2 B;
  type R;
  type D;
  TaskList<type,type2> A;
};


  //try to generate according to David's framework....

Problem<int,int> generate(int,int);


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
int Problem<type,type2>::readtimeWindows(std::ifstream& fileTW){
  int i,EST,LST,EET,LET;
  while (!fileTW.eof()){
    fileTW >> i >> EST >> LST >> EET >> LET;
    A[i].ri=EST;
    A[i].smax = LST;
    A[i].emin=EET;
    A[i].di= LET;
  }
  return 0;
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

template<typename type, typename type2>
void Problem<type,type2>::addLinearFunction(){
  for (int i=0;i<nbTask;++i) 
    A[i].addLinearFunction();
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
void Problem<type,type2>::writeTWInFile(std::ofstream& new_inst) const{
  for (int i=0;i<nbTask;++i) {
    new_inst << i << " " 
	     << A[i].ri << " " 
	     << A[i].smax << " " 
	     << A[i].emin << " " 
	     << A[i].di ;
    new_inst << std::endl;
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
type2 Problem<type,type2>::totalEnergyConsumption(const Interval<type> &I) const{
  type2 conso=0.0;
  for (int i=0;i<nbTask;++i)
    conso+=A[i].energyConsumption(I);
  return conso;
}


template<typename type,typename type2>
type2 Problem<type,type2>::totalResourceConsumption(const Interval<type> &I) const{
  // std:: cout << "***************début calcul total conso resource***********\n";
  type2 conso=0.0;
  for (int i=0;i<nbTask;++i){
    conso+=A[i].resourceConsumption(I); 
    /*    std::cout << "conso de " << i << " dans " ;
    I.displayInterval();
    std::cout << " "<<A[i].resourceConsumption(I) << std::endl;
    */}  //  std:: cout << "***************fin calcul total conso resource***********\n";
  return conso;
}

//attention au floor/ceil pour le cas entier
template<typename type,typename type2>
int Problem<type,type2>::adjustmentEmin(const Interval<type> &I,int i,type2 total){
  if (I.t2 < d(i)+ NEGATIVE_ZERO && I.t2 > emin(i) + POSITIVE_ZERO){
    int performed=0;
    const type2 LS = A[i].resourceConversion(A[i].leftShift(I),I);
    const type2 SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL + POSITIVE_ZERO < LS) {
      type temp=emin(i);
      A[i].emin=std::max(emin(i) , (type) I.t2 + ( LS - SL) /bmax(i));
      if (!isEqual(A[i].emin,temp))
	performed=1;
    }
    return performed; 
  }
  else return 0;
}

template<>
int Problem<int>::adjustmentEmin(const Interval<int> &I,int i,int total);

template<>
int Problem<int,double>::adjustmentEmin(const Interval<int> &I,int i,double total);

template<typename type,typename type2>
int Problem<type,type2>::adjustmentRi(const Interval<type> &I,int i,type2 total){
  if (I.t1 > r(i) + POSITIVE_ZERO  && I.t1 < d(i) + NEGATIVE_ZERO){
    int performed=0;
    const type2 LSboth = A[i].resourceConversion(std::min(A[i].leftShift(I),
							  A[i].bothShift(I)),I);
    const type2 SL = B*(I.t2-I.t1) - total + A[i].resourceConsumption(I);
    if (SL + POSITIVE_ZERO < LSboth && bmin(i)!=0.0) {
      type temp=r(i);
      A[i].ri=std::max(r(i),(type)I.t2-SL/bmin(i));  
      if (!isEqual(A[i].ri,temp))
	performed=1;
    }
    return performed;
  }
  else return 0;
}

template<>
int Problem<int>::adjustmentRi(const Interval<int> &I,int i,int total);

template<>
int Problem<int,double>::adjustmentRi(const Interval<int> &I,int i,double total);

template<typename type,typename type2> 
int Problem<type,type2>::adjustmentSmax(const Interval<type> &I,int i,type2 total){
  if (I.t1 > r(i) + POSITIVE_ZERO  && I.t1 < smax(i) + NEGATIVE_ZERO){
    int performed=0;
    const type2 SL=B*(I.t2-I.t1) -total+A[i].resourceConsumption(I);
    const type2 RS=A[i].resourceConversion(A[i].rightShift(I),I);
    if (SL + POSITIVE_ZERO < RS) {
      type temp=smax(i);
      A[i].smax=std::min(smax(i),(type) I.t1 - (RS-SL)/bmax(i));
      if (!isEqual(A[i].smax,temp))
	performed=1;
    }
    return performed;
  }
  else return 0;
}

template<>
int Problem<int>::adjustmentSmax(const Interval<int> &I,int i,int total);

template<>
int Problem<int,double>::adjustmentSmax(const Interval<int> &I,int i,double total);

template<typename type,typename type2> 
int Problem<type,type2>::adjustmentDi(const Interval<type> &I,int i,type2 total){
  if (I.t2 < d(i) + NEGATIVE_ZERO && I.t2 > r(i) + POSITIVE_ZERO){
    int performed=0;
    const type2 SL=B*(I.t2-I.t1)-total+A[i].resourceConsumption(I);
    const type2 RSboth=A[i].resourceConversion(std::min(A[i].rightShift(I),
							  A[i].bothShift(I)),I);
    if (SL + POSITIVE_ZERO < RSboth && bmin(i)!=0.0) {
      type temp=d(i);
      A[i].di=std::min(d(i),(type)I.t1 + SL/bmin(i)); 
      if (!isEqual(A[i].di,temp))
	performed=1;
    }
    return performed;
  }
  else return 0;
}

template<>
int Problem<int>::adjustmentDi(const Interval<int> &I,int i,int total);

template<>
int Problem<int,double>::adjustmentDi(const Interval<int> &I,int i,double total);

template<typename type,typename type2>
int Problem<type,type2>::totalTest(const Interval<type>& current){
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
