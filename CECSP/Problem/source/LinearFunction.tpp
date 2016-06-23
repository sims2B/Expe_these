#include <iostream>
#include <cassert>

template <typename type>
LinearFunction<type>::LinearFunction() : a(1.0) , c(0.0) {}

template <> LinearFunction<int>::LinearFunction();

template <typename type>
LinearFunction<type>::LinearFunction(const type& _a,const type& _c) : a(_a) , c(_c) {
  assert(a<=max_a && c<=max_c);
}

template <typename type>
void LinearFunction<type>::displayLinearFunction() const{
  if (c!=0.0) std::cout << " b -> " << a << "b + " << c << std::endl;
  else std::cout << " b -> " << a << "b"<< std::endl;
}

template <> void LinearFunction<int>::displayLinearFunction() const;

