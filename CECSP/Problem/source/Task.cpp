#include "Task.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <random>


template<> Task<int>::Task(int _nbPiece) :  ri(0) , di(0) , Wi(0), bmin(0) , bmax(0) , Fi(_nbPiece) ,emin(0) , smax(0) {}

template<> Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,Function<int> _Fi) : ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+(int)ceil(Wi/(double)Fi(bmax));
  smax=di-(int)ceil(Wi/(double)Fi(bmax)); 
  assert(Wi <= Fi(bmax) * (di -ri) );
}


template<> Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,int _a,int _c) : ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax), Fi(1){
  Interval<int> I(bmin,bmax);
  Piece<int> P(I,LinearFunction<int>(_a,_c));
  Fi.F.push_back(P);
  emin=ri+(int)ceil(Wi/(double)Fi(bmax));
  smax=di-(int)floor(Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<> Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,int _nbPiece) : ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax) ,  Fi(_nbPiece){
  if (Fi.nbPiece==1){
    Interval<int> I(bmin,bmax);
    Piece<int> P(I);
    Fi.F.push_back(P);
    emin=ri+(int)ceil(Wi/(double)Fi(bmax));
    smax=di-(int)ceil(Wi/(double)Fi(bmax));
    assert(Wi <= Fi(bmax) * (di -ri) );
  }
} 


template<> inline void Task<int>::updateSMax() {
  emin = std::max(emin,di- (int)ceil(Wi/(double)Fi(bmax)));
}

template<> inline void Task<int>::updateEMin() {
  emin = std::max(emin,ri + (int)ceil(Wi/(double)Fi(bmax)));
}

template <> inline void Task<int>::updateRi() {
  if (bmin!=0) ri = std::max(ri,emin -(int)ceil(Wi/(double)Fi(bmin)));
}

template<> inline void Task<int>::updateDi() {
  if (bmin!=0) di = std::min(di,smax + (int)ceil(Wi/(double)Fi(bmin)));
}

template<> inline int Task<int>::dataConsistency() const {
  return (Fi(bmax)*(di-ri) - Wi >= 0);
}
/*
template<> int Task<int>::resourceConversion(const int& energy,const Interval<int> &I) const{
  int max=0;
  Interval<int> J(ri,di);
  //changer / par div_sup(ceil)
  for (int q=0;q<Fi.nbPiece;++q) 
    max= std::max((energy- sizeIntersection(J,I)*Fi.F[q].f.c)/Fi.F[q].f.a,
		  max);
  if (bmin!=0)  
    return std::max(bmin*energy/Fi(bmin),
		    max);
  else
    return std::max(0,max);
}

template<> int Task<int>::rightShift(const Interval<int> &I) const{
    if (I.t1 >= di || I.t2 <=ri)
    return 0;
    else
      return std::max(0,Wi-Fi(bmax)*(std::max(0,di-I.t2)));
}  

template<> int Task<int>::leftShift(const Interval<int> &I) const{
    if (I.t1 >= di || I.t2 <=ri)
    return 0;
    else
      return std::max(0,Wi-Fi(bmax)*(std::max(0,I.t1-ri)));
}  


template<> int Task<int>::leftShift_adjust(const Interval<int> &I) const{
  return std::min(leftShift(I),std::max(Fi(bmin)*(I.t2-std::max(I.t1,ri)),
					Wi-Fi(bmax)*(std::max(I.t1-ri,0)+std::max(0,di-I.t2))));
}


template<> int Task<int>::rightShift_adjust(const Interval<int> &I) const{
  return std::min(rightShift(I),std::max(Fi(bmin)*(std::min(I.t2,di)-I.t1),
					 Wi-Fi(bmax)*(std::max(0,I.t1-ri)+std::max(0,di-I.t2))));
}
*/

int rdtsc(){
    __asm__ __volatile__("rdtsc");
    return 0;
}



