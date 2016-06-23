#include <iostream>
#include <cassert>

template<typename type>
Interval<type>::Interval() : t1(0.0) , t2(0.0) {}

template<> Interval<int>::Interval() ;

template<typename type>
Interval<type>::Interval(const type& _t1,const type& _t2) : t1(_t1) , t2(_t2) {
  assert(t2 >= t1 +NEGATIVE_ZERO);
}

template<typename type>
void Interval<type>::displayInterval() const{
  std::cout << "[" << t1 << "," << t2 << "]"<<std::endl;
}

