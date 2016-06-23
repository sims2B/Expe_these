
#ifndef DEF_LINEARFUNCTION 
#define DEF_LINEARFUNCTION

#define NEGATIVE_ZERO -0.0001
#define POSITIVE_ZERO 0.0001

template<typename type> struct LinearFunction;


template<typename type> 
struct LinearFunction
{
  LinearFunction();
  LinearFunction(const type&,const type&);

  inline type operator()(type& x) const{
    return a*x+c;
  }

  void displayLinearFunction() const;
  static const type max_a ;
  static const type max_c;

  type a;
  type c;
};

template<typename type> 
inline int isEqual(const type &a, const type &b) {
  return (a-b<=POSITIVE_ZERO && a-b >= NEGATIVE_ZERO);
}

//template<typename type>
//const type max_a = 10;

template<typename type> const type LinearFunction<type>::max_a =10;
template<typename type> const type LinearFunction<type>::max_c=100;

#include "LinearFunction.tpp"
#endif

