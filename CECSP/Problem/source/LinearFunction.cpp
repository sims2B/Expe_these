#include "LinearFunction.h"
#include <iostream>
#include <cassert>

template <>
LinearFunction<int>::LinearFunction() : a(1) , c(0) {}

template <>
void LinearFunction<int>::displayLinearFunction() const{
  if (c!=0) std::cout << " b -> " << a << "b + " << c << std::endl;
  else std::cout << " b -> " << a << "b"<< std::endl;
}

