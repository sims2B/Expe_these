#ifndef DEF_INTERVAL
#define DEF_INTERVAL

#ifndef DEF_NEGATIVE_ZERO
#define NEGATIVE_ZERO -0.00001
#endif
#ifndef DEF_POSITIVE_ZERO
#define POSITIVE_ZERO 0.00001
#endif
#include <algorithm>
#include <iostream>
#include <cassert>

///////////////////////////////////////////////////////////////////
//////////////////////// DEFINITION /////////////////////////
///////////////////////////////////////////////////////////////////

//struct Interval : [t1,t2] is represented by t1 and t2
template<typename type> struct Interval
{
  //giving t1 and t2 construct [t1,t2]
  Interval(const type&,const type &);
  
  void  displayInterval() const;

  type t1;
  type t2;
};



///////////////////////////////////////////////////////////////////
////////////////// IMPLEMENTATION /////////////////////
///////////////////////////////////////////////////////////////////

template<typename type>
Interval<type>::Interval(const type& _t1,const type& _t2) : t1(_t1) , t2(_t2) {
  assert(t2 >= t1 + NEGATIVE_ZERO);
}

template<typename type>
void Interval<type>::displayInterval() const{
  std::cout << "[" << t1 << "," << t2 << "]"<<std::endl;
}


///////////////////////////////////////////////////////////////////
/////////////////////////// INLINE ////////////////////////////////
///////////////////////////////////////////////////////////////////

//compute the size of the intersection of two intervals I and J 
template<typename type> 
inline type sizeIntersection(const Interval<type>& I,const Interval<type>& J){
  return std::max((type)(0.0),std::min(J.t2,I.t2) - std::max(I.t1,J.t1));
}


#endif
