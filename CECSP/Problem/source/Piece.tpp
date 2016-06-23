#include <iostream>

template<typename type>
Piece<type>::Piece(){
  f.a=0.0;
}

template<> Piece<int>::Piece();

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
