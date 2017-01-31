#ifndef DEF_INTERVAL
#define DEF_INTERVAL

#define NEGATIVE_ZERO -0.0001
#define POSITIVE_ZERO 0.0001
#include <algorithm>
template<typename type> struct Interval;

template<typename type>
struct Interval
{
  Interval();
  Interval(const type&,const type &);
  
  void  displayInterval() const;

  type t1;
  type t2;
};

template<typename type>
inline type sizeIntersection(const Interval<type>& I,const Interval<type>& J){
  return std::max(0.0,std::min(J.t2,I.t2) - std::max(I.t1,J.t1));
}

template<>
inline int sizeIntersection(const Interval<int>& I, const Interval<int>& J){
  return std::max(0,std::min(J.t2,I.t2) - std::max(I.t1,J.t1));
}
#include "Interval.tpp"
#endif
