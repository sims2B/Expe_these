#include <iostream>
#include <algorithm>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <random>

template<typename type>
Task<type>::Task(int _nbPiece) :  ri(0.0) , di(0.0) , Wi(0.0), bmin(0.0) , bmax(0.0) , Fi(_nbPiece) ,emin(0.0) , smax(0.0) {}

template<typename type>
Task<type>::Task(type _ri,type _di,type _Wi,type _bmin,type _bmax,Function<type> _Fi) : ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+Wi/Fi(bmax);
  smax=di-Wi/Fi(bmax);
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<typename type>
Task<type>::Task(type _ri,type _di,type _Wi,type _bmin,type _bmax,type _a,type _c) : ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax), Fi(1){
  Interval<type> I(bmin,bmax);
  Piece<type> P(I,LinearFunction<type>(_a,_c));
  Fi.F.push_back(P);
  emin=ri+Wi/Fi(bmax);
  smax=di-Wi/Fi(bmax);
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<typename type>
Task<type>::Task(type _ri,type _di,type _Wi,type _bmin,type _bmax,int _nbPiece) : ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax) ,  Fi(_nbPiece){
  if (Fi.nbPiece==1){
    Interval<type> I(bmin,bmax);
    Piece<type> P(I);
    Fi.F.push_back(P);
    emin=ri+Wi/Fi(bmax);
    smax=di-Wi/Fi(bmax);
    assert(Wi <= Fi(bmax) * (di -ri) );
  }
}

template<> Task<int>::Task(int _nbPiece) ;
template<> Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,Function<int> _Fi) ;
template<>Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,int _a,int _c) ;
template<> Task<int>::Task(int _ri,int _di,int _Wi,int _bmin,int _bmax,int _nbPieceInt);

template<typename type>
void  Task<type>::addPiecewiseFunction(){  
  //srand(rdtsc());
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(bmin, bmax);

  int _nbPiece;
  type _c=0;
  if (bmax - bmin > POSITIVE_ZERO || bmax-bmin < NEGATIVE_ZERO) 
    _nbPiece = 1 + dis(gen)%(int)(bmax-bmin) ;
  else
    _nbPiece=1;
  type t1=bmin;
  type _a = _nbPiece + dis(gen)%(10-_nbPiece);

  Fi.F.resize(_nbPiece);
  Fi.nbPiece=_nbPiece;
  if (_nbPiece==1){
    if (bmin > POSITIVE_ZERO || bmin < NEGATIVE_ZERO)
      _c= dis(gen)%10;
    type t2=bmax;
    Fi.F[0]=Piece<type>(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
  }
  else{
    if (bmin > POSITIVE_ZERO || bmin < NEGATIVE_ZERO)
      _c= dis(gen)%10;
    int t2=t1+(bmax - bmin)/_nbPiece;
    Fi.F[0]=Piece<type>(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
    for (int i=1;i<_nbPiece-1;++i) {
      t1=t2;
      t2=t1+(bmax-bmin)/_nbPiece;
      if ((_a-1)-_nbPiece+i > POSITIVE_ZERO || (_a-1)-_nbPiece+i < NEGATIVE_ZERO)
 	_a= _nbPiece-i+dis(gen)%(int)((_a-1)-_nbPiece+i);
      else 
 	_a= _nbPiece-i;
      _c=Fi(t1)-_a*t1;

      Fi.F[i]=Piece<type>(Interval<type>(t1,t2),LinearFunction<type>(_a,_c));
    }
    if (_nbPiece!=1){
      t1=t2;
      _a= 1+dis(gen)%(int)_a;
      _c=Fi(t1)-_a*t1;
      Fi.F[_nbPiece-1]=Piece<type>(Interval<type>(t1,bmax),LinearFunction<type>(_a,_c));
    }
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

   
/*template<> inline void Task<int>::updateSMax() ;
  template<> inline void Task<int>::updateEMin() ;
  template<> inline void Task<int>::updateRi() ;
  template<> inline void Task<int>::updateDi() ;
  template<> inline int Task<int>::dataConsistency() const;


  template<typename type>
  type Task<type>::leftShift_adjust(const Interval<type> &I) const{
  return std::min(leftShift(I),std::max(Fi(bmin)*(I.t2-std::max(I.t1,ri)),
  Wi-Fi(bmax)*(std::max(I.t1-ri,0.0)+std::max(0.0,di-I.t2))));
  }

  template<typename type>
  type Task<type>::rightShift_adjust(const Interval<type> &I) const{
  return std::min(rightShift(I),std::max(Fi(bmin)*(std::min(I.t2,di)-I.t1),
  Wi-Fi(bmax)*(std::max(0.0,I.t1-ri)+std::max(0.0,di-I.t2))));
  }*/

template<typename type>
type Task<type>::leftShift(const Interval<type> &I) const{
  return std::max(0.0,Wi-Fi(bmax)*(std::max(0.0,I.t1-ri)));
}

template<typename type>
type Task<type>::rightShift(const Interval<type> &I) const{
  return std::max(0.0,Wi-Fi(bmax)*(std::max(0.0,di-I.t2)));
}

template<typename type>
type Task<type>::bothShift(const Interval<type> &I) const{
  return std::max(Fi(bmin)*(I.t2-I.t1),
		  Wi-Fi(bmax)*(std::max(0.0,I.t1-ri) + std::max(di-I.t2,0.0)));
}

template<typename type>
type Task<type>::energyConsumption(const Interval<type>& I) const{
  return std::min(std::min(leftShift(I),rightShift(I)), bothShift(I));
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

/*template<> int Task<int>::resourceConversion(const int& energy,const Interval<int> &I) const; 
  template<> int Task<int>::rightShift(const Interval<int> &I) const;
  template<>int Task<int>::leftShift(const Interval<int> &I) const;
  template<> int Task<int>::leftShift_adjust(const Interval<int> &I) const;
  template<> int Task<int>::rightShift_adjust(const Interval<int> &I) const;*/


