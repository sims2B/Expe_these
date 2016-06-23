#ifndef DEF_FUNCTION
#define DEF_FUNCTION

#include <vector>
#include "Piece.h"

////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!! fi(bmin)!=0!!!!!!!!!!!!!!!!!!!!!//
//////////////////////////////////////////////////////
template<typename type> struct Function;

template<typename type>
using PieceList=std::vector<Piece<type>>;

//Fonction linéaire par morceaux, continue et croissante
template<typename type>
struct Function{
  Function(int);
  Function(PieceList<type>);
  Function(const Function&);
  
  void displayFunction() const;
  
  //bornes de F triées et F(x) bien défini
  //si F(x) non défini, retourne -1
  type operator()(type) const;

  //renvoit le point de meilleur rendement
  //type bestPoint() const;
  

  int nbPiece;
  PieceList<type> F;
};
#include "Function.tpp"
#endif
