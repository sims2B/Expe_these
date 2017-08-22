#include <iostream>
#include <limits>
#include "interval.h"

interval::interval() : t1(0) , t2(std::numeric_limits<int>::max()) {}

interval::interval(type _t1,type _t2) : t1(_t1) , t2(_t2) 
{
  if (t1 > t2)
    {
      type aux=t1;
      t1=t2;
      t2=aux;
    }
}

/*int interval::reverseinterval()
{
  if (t1 > t2)
    {
      type aux=t1;
      t1=t2;
      t2=aux;
      return 0;
    }
  return 1;
  }*/

void interval::displayInterval() const
{
  std::cout << "[" << t1 << "," << t2 << "]";
}


