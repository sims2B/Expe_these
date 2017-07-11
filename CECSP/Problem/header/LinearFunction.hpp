#include <iostream>
#include <cassert>
#include <cmath> 

#ifndef DEF_LINEARFUNCTION 
#define DEF_LINEARFUNCTION

//to handle double precision

#ifndef DEF_NEGATIVE_ZERO
#define NEGATIVE_ZERO -0.00001
#endif
#ifndef DEF_POSITIVE_ZERO
#define POSITIVE_ZERO 0.00001
#endif


///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

//template struct representing a linear function
// a*x+c by its coefficient a and c.
template<typename type> 
struct LinearFunction{
  
//two constants limiting the coefficients
  //of the linear function
  static const type max_a ;
  static const type max_c;

  //default constructor : f(x)=x
  LinearFunction();
  
  //giving a and c, construct f(x)= ax + c 
  LinearFunction(const type&,const type&);

  // giving x, return f(x) 
  inline type operator()(type& x) const{
    return a*x+c;
  }

  void displayLinearFunction() const;

  type a;
  type c;
};

//function isEqual for float/double
//return true if |a-b| < 0.0001
template<typename type> 
inline int isEqual(const type &a, const type &b) {
  return ( fabs(a-b) < POSITIVE_ZERO);
}



///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type> const type LinearFunction<type>::max_a =10;
template<typename type> const type LinearFunction<type>::max_c=100;


template <typename type>
LinearFunction<type>::LinearFunction() : a(1.0) , c(0.0) {}

template <typename type>
LinearFunction<type>::LinearFunction(const type& _a,const type& _c) : 
  a(_a) , c(_c) {}

template <typename type>
void LinearFunction<type>::displayLinearFunction() const{
  if (c!=0.0) std::cout << " b -> " << a << "b + " << c ;
  else std::cout << " b -> " << a << "b";
}


#endif

