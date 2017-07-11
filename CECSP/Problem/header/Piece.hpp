#ifndef DEF_PIECE
#define DEF_PIECE

#include "LinearFunction.hpp"
#include "Interval.hpp"

///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////


//struct that represent a piece of a piecewise linear function
//define by a linear function and an interval
template<typename type> struct Piece{

  //given I=[t1,t2], construct ( f(x)=x on I )
  Piece(Interval<type>);
  
  //given I=[t1,t2] and a and c, construct ( f(x)= ax + c on I )
  Piece(Interval<type>,LinearFunction<type>);

  void displayPiece() const;

  Interval<type> I;
  LinearFunction<type> f;
};


///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////


template<typename type>
Piece<type>::Piece(Interval<type> _I) : I(_I) {}

template<typename type>
Piece<type>::Piece(Interval<type> _I, LinearFunction<type> _f) : I(_I) , f(_f) {}

template<typename type>
void Piece<type>::displayPiece() const{
  f.displayLinearFunction();
  std::cout << " , if b in " ;
  I.displayInterval();
}
#endif
