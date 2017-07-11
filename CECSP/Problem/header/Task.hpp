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
template<typename type>
struct Task {
  
  //!!! Penser a updater smax et emin si changement Fi!!!
  
  //giving a release date, a deadline, an energy quantity, a minimum and maximum 
  //requirement and a list of Piece, construct the associated task and compute the 
  //latest start time and earliest end time 
  Task(type,type,type,type,type,PieceList<type>); 
  
  //giving a release date, a deadline, an energy quantity, a minimum and maximum 
  //requirement and a Function, construct the associated task and compute the 
  //latest start time and earliest end time 
  Task(type,type,type,type,type,Function<type>);

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
  type energyConsumption(const Interval<type>&) const;
  //compute the minimum resource consumption of a task inside an interval I
  type resourceConsumption(const Interval<type>&) const;
  //compute the quantity of resource a task has to consume in an interval I to 
  //bring an energy "energy" to the task
  type resourceConversion(const type& energy, const Interval<type>&) const;

  //compute the energy consumption of a task inside an interval I if the task is
  // left-shifted
  type leftShift(const Interval<type>&) const;
  //compute the energy consumption of a task inside an interval I if the task is
  // right-shifted
  type rightShift(const Interval<type>&) const;
  //compute the energy consumption of a task inside an interval I if the task is
  // both-shifted
  type bothShift(const Interval<type>&) const;
  
  type ri;
  type di;
  type Wi;
  type bmin;
  type bmax;
  Function<type> Fi;
  type emin;
  type smax;
};


///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////


template<typename type>
Task<type>::Task(type _ri,type _di,type _Wi,type _bmin,type _bmax,PieceList<type> P) : 
  ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(P){
  emin=ri+Wi/Fi(bmax);
  smax=di-Wi/Fi(bmax);
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,PieceList<int> P) ;

template<typename type>
Task<type>::Task(type _ri,type _di,type _Wi,type _bmin,type _bmax,Function<type> _Fi) : 
  ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+Wi/Fi(bmax);
  smax=di-Wi/Fi(bmax);
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,Function<int> _Fi) ;


template<typename type>
void  Task<type>::addConcavePiecewiseFunction(){  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(bmin, bmax);

  int _nbPiece;
  type _c=0;
  type t1=bmin;	
  type t2;
  
  //random generation of nbPiece in [1, bmax-bmin]
  if (!isEqual(bmax,bmin)) 
    _nbPiece = 1 + dis(gen)%(int)(bmax-bmin) ;
  else
    _nbPiece=1;
    
  //random generation of a0 in [nbPiece , 10]
  type _a = _nbPiece + dis(gen)%(10-_nbPiece+1);

  Fi.F.resize(_nbPiece);
  Fi.nbPiece=_nbPiece;

  //random generation of c0 (if bmin=0 c1=0) in [0,10]
  if (!isEqual(bmin,(type)0.0))
    _c= dis(gen)%11;
  if (_nbPiece==1)
    t2=bmax;
  else
    t2=t1+(bmax - bmin)/_nbPiece;
  Fi.F[0]=Piece<type>(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
 
  //random generation of ai and ci (from i=1 to nbPiece-1)
  for (int i=1;i<_nbPiece-1;++i) {
    t1=t2;
    t2=t1+(bmax-bmin)/_nbPiece;
    if (!isEqual(_a-1,_nbPiece-i))
      _a= _nbPiece-i+dis(gen)%(int)((_a-1)-_nbPiece+i);
    else 
      _a= _nbPiece-i;
    _c=Fi(t1)-_a*t1;
    Fi.F[i]=Piece<type>(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
  }
    
  //random generation of the last piece of the function
  if (_nbPiece!=1){
    t1=t2;
    _a= 1+dis(gen)%(int)_a;
    _c=Fi(t1)-_a*t1;
    Fi.F[_nbPiece-1]=Piece<type>(Interval<type>(t1,bmax),LinearFunction<type>(_a,_c));
  }
}

template<typename type>
void  Task<type>::addConvexPiecewiseFunction(){  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(bmin, bmax);

  int _nbPiece;
  type _c=0;
  type t1=bmin;	
  type t2;
  
  //random generation of nbPiece in [1, bmax-bmin]
  if (!isEqual(bmax,bmin)) 
    _nbPiece = 1 + dis(gen)%(int)(bmax-bmin) ;
  else
    _nbPiece=1;
    
  //random generation of a0 in [nbPiece , 10]
  type _a = _nbPiece + dis(gen)%(10-_nbPiece+1);

  Fi.F.resize(_nbPiece);
  Fi.nbPiece=_nbPiece;

  //random generation of c0 (if bmin=0 c1=0) in [0,10]
  if (!isEqual(bmin,(type)0.0))
    _c= dis(gen)%11;
  if (_nbPiece==1)
    t2=bmax;
  else
    t2=t1+(bmax - bmin)/_nbPiece;
  Fi.F[_nbPiece-1]=Piece<type>(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
 
  //random generation of ai and ci (from i=1 to nbPiece-1)
  for (int i=_nbPiece-1;i<1;--i) {
    t1=t2;
    t2=t1+(bmax-bmin)/_nbPiece;
    if (!isEqual(_a-1,_nbPiece-i))
      _a= _nbPiece-i+dis(gen)%(int)((_a-1)-_nbPiece+i);
    else 
      _a= _nbPiece-i;
    _c=Fi(t1)-_a*t1;
    Fi.F[i]=Piece<type>(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
  }
    
  //random generation of the last piece of the function
  if (_nbPiece!=1){
    t1=t2;
    _a= 1+dis(gen)%(int)_a;
    _c=Fi(t1)-_a*t1;
    Fi.F[0]=Piece<type>(Interval<type>(t1,bmax),LinearFunction<type>(_a,_c));
  }
}

template<typename type>
void Task<type>::displayTask() const{
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


template<typename type>  
void Task<type>::updateEMin(){
  emin = std::max(emin,ri +Wi/Fi(bmax));
}
template<> void Task<int>::updateEMin();
  
template<typename type> 
void Task<type>::updateSMax() 
{smax = std::min(smax,di - Wi/Fi(bmax));
}

template<> void Task<int>::updateSMax();

template<typename type>
type Task<type>::leftShift(const Interval<type> &I) const{
  type nrj=(type)0.0;
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
int Task<int>::leftShift(const Interval<int> &I) const;

//attention aux arrondi et aux smax et emin
template<typename type>
type Task<type>::rightShift(const Interval<type> &I) const{
  type nrj=(type)0.0;
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
int Task<int>::rightShift(const Interval<int> &I) const;

template<typename type>
type Task<type>::bothShift(const Interval<type> &I) const{
  return std::max(Fi(bmin)*(I.t2-I.t1),
		  Wi-Fi(bmax)*(std::max((type)0.0,I.t1-ri) + std::max(di-I.t2,(type)0.0)));
}

template<typename type>
type Task<type>::energyConsumption(const Interval<type>& I) const{
  return /*std::min(*/std::min(leftShift(I),rightShift(I))/*, bothShift(I))*/;
}

template<typename type>
type Task<type>::resourceConversion(const type& energy,const Interval<type> &I) const{
  type max=0.0;
  Interval<type> J(ri,di);
  for (int q=0;q<Fi.nbPiece;++q) 
    max= std::max((energy- sizeIntersection(J,I)*Fi.F[q].f.c)/Fi.F[q].f.a,
		  max);
  if (bmin!=0.0)  
    return std::max(bmin*energy/Fi(bmin),
		    max);
  else
    return max;
}


template<typename type>
type Task<type>::resourceConsumption(const Interval<type> &I) const{
  return resourceConversion(this->energyConsumption(I),I);
}

int rdtsc();
#endif
