#include "Task.hpp"
int rdtsc(){
    __asm__ __volatile__("rdtsc");
    return 0;
}


template<>
Task<int,int>::Task(int _ri, int _di, int _Wi, int _bmin, int _bmax, PieceList<int> P) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(P){
  emin=ri + ceil((double)Wi/(double)Fi(bmax));
  smax=di - ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int,double>::Task(int _ri, int _di, double _Wi, double _bmin, double _bmax, PieceList<double> P) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(P){
  emin=ri + ceil((double)Wi/(double)Fi(bmax));
  smax=di - ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int>::Task(int _ri,int _di,int _Wi,int _bmin, int _bmax,Function<int> _Fi) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+ceil((double)_Wi/(double)_Fi(_bmax));
  smax=di-ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>
Task<int,double>::Task(int _ri,int _di,double _Wi,double _bmin, double _bmax,Function<double> _Fi) : 
	ri(_ri) , di(_di) , Wi(_Wi), bmin(_bmin) , bmax(_bmax),  Fi(_Fi){
  emin=ri+ceil((double)_Wi/(double)_Fi(_bmax));
  smax=di-ceil((double)Wi/(double)Fi(bmax));
  assert(Wi <= Fi(bmax) * (di -ri) );
}

template<>  
void Task<int>::updateEMin(){
  emin = std::max(emin,ri + (int)ceil((double)Wi/(double)Fi(bmax)));
}
template<>  
void Task<int,double>::updateEMin(){
  emin = std::max(emin,ri + (int)ceil((double)Wi/(double)Fi(bmax)));
}

template<> 
void Task<int>::updateSMax() 
{smax = std::min(smax,di - (int)ceil((double)Wi/(double)Fi(bmax)));
}
template<> 
void Task<int,double>::updateSMax() 
{smax = std::min(smax,di - (int)ceil((double)Wi/(double)Fi(bmax)));
}




template<>
int Task<int>::leftShift(const Interval<int> &I) const{
int nrj=0;
  if (I.t1 < ri + (int)ceil((double)Wi/(double)Fi(bmax)) ){
    if (I.t2  >= di ){
      if (I.t1 >= ri ) nrj=Wi-Fi(bmax)*(I.t1-ri);
      else nrj = Wi;
    }
    else if (I.t1 <= ri ){
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
double Task<int,double>::leftShift(const Interval<double> &I) const{
double nrj=0;
  if (I.t1 < ri + ceil((double)Wi/(double)Fi(bmax)) ){
    if (I.t2  >= di ){
      if (I.t1 >= ri ) nrj=Wi-Fi(bmax)*(I.t1-ri);
      else nrj = Wi;
    }
    else if (I.t1 <= ri ){
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

//attention aux arrondi et aux smax et emin
template<>
int Task<int>::rightShift(const Interval<int> &I) const{
  int nrj=0;
  if (I.t2 > di - (int)ceil((double)Wi/(double)Fi(bmax)) ){
    if (I.t1 <= ri  ){
      if (I.t2 <= di) nrj=Wi-Fi(bmax)*(di-I.t2);
      else nrj = Wi;
    }
    else if (I.t2 >= di ){
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
double Task<int,double>::rightShift(const Interval<double> &I) const{
  double nrj=0;
  if (I.t2 > di - ceil((double)Wi/(double)Fi(bmax)) ){
    if (I.t1 <= ri  ){
      if (I.t2 <= di) nrj=Wi-Fi(bmax)*(di-I.t2);
      else nrj = Wi;
    }
    else if (I.t2 >= di ){
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
