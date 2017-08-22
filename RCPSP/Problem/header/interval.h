#ifndef DEF_INTERVAL
#define DEF_INTERVAL

#include <algorithm>

typedef int type;

struct interval
{
  interval();
  interval(type,type);

  // int reverseInterval();

  void  displayInterval() const;

  type t1;
  type t2;
};

inline type sizeIntersection(const interval& I,const interval& J)
{
  return std::max(0,std::min(J.t2,I.t2) - std::max(I.t1,J.t1));
}

#endif
