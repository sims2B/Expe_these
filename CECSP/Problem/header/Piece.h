#ifndef DEF_PIECE
#define DEF_PIECE

#include "LinearFunction.h"
#include "Interval.h"

template<typename type>
struct Piece{
  Piece();
  Piece(Interval<type>);
  Piece(Interval<type>,LinearFunction<type>);

  void displayPiece() const;

  Interval<type> I;
  LinearFunction<type> f;
};

#include "Piece.tpp"
#endif
