#ifndef DEF_TASK
#define DEF_TASK

#include "Function.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdio.h>
#include <math.h> 
#include <stdlib.h>
#include <random>

//!!!!!!!!!!!bmin!=0!!!!!!!!!!!!!!!!


///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

//struct representing a task
template<typename type , typename type2 = type>
struct Task {
  
  //!!! Penser a updater smax et emin si changement Fi!!!
  
  //giving a release date, a deadline, an energy quantity, a minimum and maximum 
  //requirement and a list of Piece, construct the associated task and compute the 
  //latest start time and earliest end time 
  Task(type,type,type2,type2,type2,PieceList<type2>); 
  
  //giving a release date, a deadline, an energy quantity, a minimum and maximum 
  //requirement and a Function, construct the associated task and compute the 
  //latest start time and earliest end time 
  Task(type,type,type2,type2,type2,Function<type2>);

  //generate a concave piecewise linear function and set the function Fi to this 
  //function
  void addConcavePiecewiseFunction();
  //generate a convex piecewise linear function and set the function Fi to this 
  //function
  void addConvexPiecewiseFunction();
		
  void displayTask() const;

  //attention arrondi si integer!!

  //update of latest start time and earliest end time
  inline void updateEMin();
  inline void updateSMax();

  // check the data consistency (energy demand not too high comparing to the 
  // time windows)
  inline int dataConsistency() const {return (Fi(bmax)*(di-ri) - Wi >=NEGATIVE_ZERO);}
 
  //compute the minimum energy consumption of a task inside an interval I
  type2 energyConsumption(const Interval<type2>&) const;
  //compute the minimum resource consumption of a task inside an interval I
  type2 resourceConsumption(const Interval<type2>&) const;
  //compute the quantity of resource a task has to consume in an interval I to 
  //bring an energy "energy" to the task
  type2 resourceConversion(const type2& energy, const Interval<type2>&) const;

  //compute the energy consumption of a task inside an interval I if the task is
  // left-shifted
  type2 leftShift(const Interval<type2>&) const;
  //compute the energy consumption of a task inside an interval I if the task is
  // right-shifted
  type2 rightShift(const Interval<type2>&) const;
  //compute the energy consumption of a task inside an interval I if the task is
  // both-shifted
  type2 bothShift(const Interval<type2>&) const;
  
  type ri;
  type di;
  type2 Wi;
  type2 bmin;
  type2 bmax;
  Function<type2> Fi;
  type emin;
  type smax;
};
int rdtsc();

///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////


template<typename type,typename type2>
Task<type,type2>::Task(type _ri,type _di,type2 _Wi,type2 _bmin,type2 _bmax,PieceList<type2> P) : 
  ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(P){
  emin=ri+Wi/Fi(bmax);
  smax=di-Wi/Fi(bmax);
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int,int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,PieceList<int> P) ;


template<>
Task<int,double>::Task(int _ri,int _di,double _Wi,double _bmin,double _bmax,PieceList<double> P) ;

template<typename type,typename type2>
Task<type,type2>::Task(type _ri,type _di,type2 _Wi,type2 _bmin,type2 _bmax,Function<type2> _Fi) : 
  ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+Wi/Fi(bmax);
  smax=di-Wi/Fi(bmax);
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int,int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,Function<int> _Fi) ;

template<>
Task<int,double>::Task(int _ri,int _di,double  _Wi,double  _bmin,double _bmax,Function<double> _Fi) ;

template<typename type,typename type2>
void  Task<type,type2>::addConcavePiecewiseFunction(){  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(bmin, bmax);
  std::uniform_int_distribution<> dis1(1,1001);

  int _nbPiece;
  type2 _c=0;
  type2 t1=bmin;	
  type2 t2;
  
  //random generation of nbPiece in [1, bmax-bmin]
  if (!isEqual(bmax,bmin)) 
    _nbPiece = 1 + dis(gen)%(int)(bmax-bmin) ;
  else
    _nbPiece=1;
    
  //random generation of a0 in [nbPiece , 10]
  type2 _a = _nbPiece + dis1(gen)%(10-_nbPiece+1);

  Fi.F.erase(Fi.F.begin(),Fi.F.end());
  Fi.F.reserve(_nbPiece);
  Fi.nbPiece=_nbPiece;

  //random generation of c0 (if bmin=0 c1=0) in [0,10]
  if (!isEqual(bmin,(type2)0.0))
    _c= dis1(gen)%11;
  if (_nbPiece==1)
    t2=bmax;
  else
    t2=t1+ceil((double)(bmax - bmin)/(double)_nbPiece);
  Piece<type2> _P(Interval<type2>(t1,t2),LinearFunction<type2>(_a,_c));
  Fi.F.push_back(_P);
 
  //random generation of ai and ci (from i=1 to nbPiece-1)
  for (int i=1;i<_nbPiece-1;++i) {
    t1=t2;
    t2=t1+ceil((double)(bmax - bmin)/(double)_nbPiece);
    if (!isEqual(_a-1,(type2)_nbPiece-i))
      _a= _nbPiece-i+dis1(gen)%(int)((_a-1)-_nbPiece+i);
    else 
      _a= _nbPiece-i;
    _c=Fi(t1)-_a*t1;
    Piece<type2> _P(Interval<type2>(t1,t2),LinearFunction<type2>(_a,_c));
    Fi.F.push_back(_P);
  }
    
  //random generation of the last piece of the function
  if (_nbPiece!=1){
    t1=t2;
    _a= 1+dis1(gen)%(int)_a;
    _c=Fi(t1)-_a*t1;
    Piece<type2> _P(Interval<type2>(t1,bmax),LinearFunction<type2>(_a,_c));
    Fi.F.push_back(_P);
  }
}

template<typename type,typename type2 >
void  Task<type,type2>::addConvexPiecewiseFunction(){  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(bmin, bmax);
  std::uniform_int_distribution<> dis1(1,1001);
  
  int _nbPiece;
  type2 _c=0;
  type2 t1=bmin;	
  type2 t2;
  
  //random generation of nbPiece in [1, bmax-bmin]
  if (!isEqual(bmax,bmin)) 
    _nbPiece = 1 + dis(gen)%(int)(bmax-bmin) ;
  else
    _nbPiece=1;
    
  //random generation of a0 in [1 , 10 - nbPiece]
  type2 _a = 1 + dis1(gen)%(10 - _nbPiece + 1);
  Fi.F.erase(Fi.F.begin(),Fi.F.end());
  Fi.F.reserve(_nbPiece);
  Fi.nbPiece=_nbPiece;
  
  //random generation of c0 (if bmin=0 c1=0) in [0,10]
  if (!isEqual(bmin,(type2)0.0))
    _c= dis1(gen)%11;
  if (_nbPiece==1)
    t2=bmax;
  else
    t2=t1+(int)(bmax - bmin)/_nbPiece;
  Piece<type2> _P(Interval<type2>(t1,t2),LinearFunction<type2>(_a,_c));
  Fi.F.push_back(_P);
    
  //random generation of ai and ci (from i=1 to nbPiece-1)
  for (int i=1;i<_nbPiece-1;++i) {
    t1=t2;
    t2=t1+(int)(bmax - bmin)/_nbPiece;
    _c = _a *t1 + _c; 
    if (!isEqual(10 -(_nbPiece - i) +1 - _a ,(type2)0.0))
      _a= (_a+1) +dis1(gen)%(int)(10 -(_nbPiece - i) +1 - _a );
    else 
      _a=_a+1;
    _c= _c -_a*t1;
    Piece<type2> _P1(Interval<type2>(t1,t2),LinearFunction<type2>(_a,_c));
    Fi.F.push_back(_P1);
  }
    
  //random generation of the last piece of the function
  if (_nbPiece!=1){
    t1=t2; 
    _c = _a *t1 + _c; 
      if (!isEqual(_a+1,10.0))
	_a=(_a+1) +dis1(gen)%(int)(10 - _a );
      else _a = 10;
    _c=_c -_a*t1;
    Piece<type2> _P1(Interval<type2>(t1,bmax),LinearFunction<type2>(_a,_c));
    Fi.F.push_back(_P1);
  }
}

template<typename type,typename type2 >
void Task<type,type2>::displayTask() const{
  std::cout << "ri= " << ri<< std::endl;
  std::cout << "di= " << di <<std::endl;
  std::cout << "eimin= " << emin<< std::endl;
  std::cout << "simax= " << smax <<std::endl;
  std::cout << "bi_min= "<< bmin << std::endl;
  std::cout << "bi_max= "<< bmax << std::endl;
  std::cout << "Wi= " << Wi << std::endl;
  for (int q=0;q<Fi.nbPiece;++q) 
    Fi.F[q].f.displayLinearFunction();
}


template<typename type,typename type2>  
void Task<type,type2>::updateEMin(){
  emin = std::max(emin,ri +Wi/Fi(bmax));
}

template<> void Task<int,int>::updateEMin();
template<> void Task<int,double>::updateEMin();
  
template<typename type,typename type2> 
void Task<type,type2>::updateSMax() 
{smax = std::min(smax,di - Wi/Fi(bmax));
}

template<> void Task<int,int>::updateSMax();
template<> void Task<int,double>::updateSMax();

template<typename type,typename type2>
type2 Task<type,type2>::leftShift(const Interval<type2> &I) const{
  type2 nrj=(type2)0.0;
  if (I.t1 < ri + Wi/Fi(bmax) + NEGATIVE_ZERO){
    if (I.t2  >= di + NEGATIVE_ZERO){
      if (I.t1 >= ri + POSITIVE_ZERO) nrj=Wi-Fi(bmax)*(I.t1-ri);
      else nrj = Wi;
    }
    else if (I.t1 <= ri + NEGATIVE_ZERO  ){
      nrj=std::max(Wi-Fi(bmax)*(di-I.t2),Fi(bmin)*(I.t2-ri));
    }
    else {
      nrj = std::min(std::max(Wi-Fi(bmax)*(di-I.t2+I.t1-ri),
			      Fi(bmin)*(I.t2-I.t1))
		     ,Wi-Fi(bmax)*(I.t1-ri));
    }
  }
  return nrj;
}

template<>
int Task<int,int>::leftShift(const Interval<int> &I) const;
template<>
double Task<int,double>::leftShift(const Interval<double> &I) const;

//attention aux arrondi et aux smax et emin
template<typename type,typename type2>
type2 Task<type,type2>::rightShift(const Interval<type2> &I) const{
  type2 nrj=(type2)0.0;
  if (I.t2 > di - Wi/Fi(bmax) + POSITIVE_ZERO){
    if (I.t1 <= ri + POSITIVE_ZERO ){
      if (I.t2 <= di + NEGATIVE_ZERO) nrj=Wi-Fi(bmax)*(di-I.t2);
      else nrj = Wi;
    }
    else if (I.t2 >= di + POSITIVE_ZERO){
      nrj=std::max(Wi-Fi(bmax)*(I.t1-ri),Fi(bmin)*(di-I.t1));
    }
    else {
      nrj = std::min(std::max(Wi-Fi(bmax)*(di-I.t2+I.t1-ri),
			      Fi(bmin)*(I.t2-I.t1))
		     ,Wi-Fi(bmax)*(di-I.t2));
    }
  }
  return nrj;
}

template<>
int Task<int,int>::rightShift(const Interval<int> &I) const;
template<>
double Task<int,double>::rightShift(const Interval<double> &I) const;

template<typename type,typename type2>
type2 Task<type,type2>::bothShift(const Interval<type2> &I) const{
  return std::max(Fi(bmin)*(I.t2-I.t1),
		  Wi-Fi(bmax)*(std::max((type)0.0,I.t1-ri) + std::max(di-I.t2,(type)0.0)));
}

template<typename type,typename type2>
type2 Task<type,type2>::energyConsumption(const Interval<type2>& I) const{
  return /*std::min(*/std::min(leftShift(I),rightShift(I))/*, bothShift(I))*/;
}

template<typename type,typename type2>
type2 Task<type,type2>::resourceConversion(const type2& energy,const Interval<type2> &I) const{
  type2 max=0.0;
  Interval<type2> J(ri,di);
  for (int q=0;q<Fi.nbPiece;++q) 
    max= std::max((energy- sizeIntersection(J,I)*Fi.F[q].f.c)/Fi.F[q].f.a,
		  max);
  if (bmin!=0.0)  
    return std::max(bmin*energy/Fi(bmin),
		    max);
  else
    return max;
}


template<typename type,typename type2>
type2 Task<type,type2>::resourceConsumption(const Interval<type2> &I) const{
  return resourceConversion(this->energyConsumption(I),I);
}

int rdtsc();
#endif
