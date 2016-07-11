#include <iostream>
#include <cassert>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

template<typename type>
Problem<type>::Problem(int _nbTask) : nbTask(_nbTask) {
  A.reserve(nbTask);
}

template<typename type>
Problem<type>::Problem(int _nbTask,type _B) : nbTask(_nbTask), B(_B) {
  A.reserve(nbTask);
}

template<typename type>
Problem<type>::Problem(int _nbTask, type _B, TaskList<type> _A) : nbTask(_nbTask) , B(_B) , A(_A) {
  R=r(0);
  D=d(0);
  for (int i=1;i<nbTask;++i){
    R = ((R > r(i))?  r(i) : R);
    D = ((D < d(i))?  d(i) : D);
  }
}


template<typename type>
Problem<type>::Problem(const Problem<type> &P)  : nbTask(P.nbTask) , B(P.B) , R(P.R) , D(P.D), A(P.A)  {}

template<typename type>
void Problem<type>::updateHorizon(){
  R=r(0);
  D=d(0);
  for (int i=1;i<nbTask;++i){
    R = ((R > r(i))?  r(i) : R);
    D = ((D < d(i))?  d(i) : D);
  }
}

template<typename type>
void Problem<type>::readFromFile(std::ifstream& instance){
  int i;
  type _B;
  instance >> _B;
  B=_B;
 
  for (i=0;i<nbTask;++i){
    type _ri,_di,_Wi,_bmin,_bmax,_a,_c;
    int _nbPiece;
    instance >> _ri >> _di >> _Wi >> _bmin >> _bmax >>_nbPiece;
    if (_nbPiece==1) {
      instance >> _a >> _c;
      Task<type> T(_ri,_di,_Wi,_bmin,_bmax,_a,_c);
      A.push_back(T);
    }
    else {
      Function<type> _F(_nbPiece);
      type t1,t2;
      t1=_bmin;
      for (int q=0;q<_nbPiece-1;++q) {
	instance >> _a >> _c >> t2;
	Piece<type> _P(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
	_F.F.push_back(_P);
	t1=t2;
      }
      instance >> _a >> _c;
      Piece<type> _P(Interval<type>(t1,_bmax),LinearFunction<type>(_a,_c));
      _F.F.push_back(_P);
      Task<type> T(_ri,_di,_Wi,_bmin,_bmax,_F);
      A.push_back(T);	
    }
  }
  this->updateHorizon();
}

template<typename type>
void Problem<type>::addPiecewiseFunction(){  
  int min;
  int max;
  srand(rdtsc());
  for (int i=0;i<nbTask;++i) {
    A[i].addPiecewiseFunction();
    min=(int)(A[i].Fi(bmin(i))*(d(i)-r(i)));
    max=(int)(A[i].Fi(bmax(i))*(d(i)-r(i))* 0.8);
    if (max-min > POSITIVE_ZERO && max-min < NEGATIVE_ZERO)
      A[i].Wi=min + rand()%(max-min);
    else  
      A[i].Wi=min;
  }
}


template<typename type>
void Problem<type>::ecrire(std::ofstream& new_inst) const{
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

template<typename type>
Problem<type>& Problem<type>::operator=(const Problem<type>& P){
  nbTask=P.nbTask;
  B=P.B;
  A=P.A;
  return *this;
}

template<typename type>
void Problem<type>::displayProblem() const{
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

template<typename type> 
int Problem<type>::dataConsistency() const{
  for (int i=0;i<nbTask;++i)
    if (!A[i].dataConsistency())
      return 0;
  return 1;
}

/*
template<> int Problem<int>::totalEnergyConsumption(const Interval<int> &I) const;


template<> int Problem<int>::totalResourceConsumption(const Interval<int> &I) const;
*/
template<typename type> 
type Problem<type>::totalEnergyConsumption(const Interval<type> &I) const{
  type conso=0.0;
  for (int i=0;i<nbTask;++i)
    conso+=A[i].energyConsumption(I);
  return conso;
}


template<typename type>
type Problem<type>::totalResourceConsumption(const Interval<type> &I) const{
  type conso=0.0;
  for (int i=0;i<nbTask;++i)
    conso+=A[i].resourceConsumption(I); 
  return conso;
}

/*template<> inline int Problem<int>::energeticReasonning(const Interval<int>& I,int total) const;
 */

template<typename type>
int Problem<type>::adjustmentLS(const Interval<type> &I,int i,type total){
  int performed=0;
  if (I.t2 > d(i)){
    const type capacity=B*(I.t2-I.t1);
    type  SL,LS,LSboth; 
    SL=capacity-total+A[i].resourceConsumption(I);
    LS=A[i].resourceConversion(A[i].leftShift(I),I);
    LSboth=std::min(LS,A[i].resourceConversion(A[i].bothShift(I),I));
    if (SL-LS< NEGATIVE_ZERO) {
      A[i].emin=std::max(emin(i),I.t2+(LS-SL)/bmax(i));
      performed=1;
    }
    if (SL-LSboth < NEGATIVE_ZERO && bmin(i)!=0.0) {
      A[i].ri=std::max(r(i),I.t2-SL/bmin(i));  
      performed=1;
    }
  }
  return performed;
}


template<typename type> 
int Problem<type>::adjustmentRS(const Interval<type> &I,int i,type total){
  int performed=0;
  if (I.t1 < r(i)){
    const type capacity=B*(I.t2-I.t1);
    type SL,RS,RSboth;  
    SL=capacity-total+A[i].resourceConsumption(I);
    RS=A[i].resourceConversion(A[i].rightShift(I),I);
    RSboth=std::min(RS,A[i].resourceConversion(A[i].bothShift(I),I));
    if (SL- RS< NEGATIVE_ZERO) {
      A[i].smax=std::min(smax(i),I.t1-(RS-SL)/bmax(i));
      performed=1;
    }
    if (bmin(i)!=0.0 && SL-RSboth < NEGATIVE_ZERO) {
      A[i].di=std::min(d(i),I.t1+SL/bmin(i)); 
      performed=1;
    }
  }
  return performed;
}



template<typename type>
int Problem<type>::totalTest(const Interval<type>& current){
  const type total=totalResourceConsumption(current);
  if (!energeticReasonning(current,total))
    return 0;
  else {
    adjustmentLS(current,total);
    adjustmentRS(current,total);
  }
  return 1;
}
/*
template<> int Problem<int>::adjustmentLS(const Interval<int> &I, int i, int total);


template<> int Problem<int>::adjustmentRS(const Interval<int> &I,int i, int total);
*/
