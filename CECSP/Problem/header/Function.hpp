#ifndef DEF_FUNCTION
#define DEF_FUNCTION

//!!!!!!!!!!!!!!!!!! fi(bmin)!=0!!!!!!!!!!!!!!!!!!!!!//

#include <vector>
#include <limits>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "Piece.hpp"


///////////////////////////////////////////////////////////////////
/////////////////////////// DEFINITION ////////////////////////////
///////////////////////////////////////////////////////////////////

template<typename type>
using PieceList=std::vector<Piece<type>>;

//Piecewise linear function represented by a vector of Piece
template<typename type> struct Function{

  //given a vector of Piece, construct the associated function 
  //(test continuity and increasing)
  Function(PieceList<type>);
  
  //copy constructor
  Function(const Function&);
  
  void displayFunction() const;
  
  //return 1 if the function is continuous, 0 otw
  //also ensure Pieces are sorted and no interval overlap
  int isContinuous() const;
  //retrun 1 if the function is increasing (non necessarily strictly), 0 otw
  int isIncreasing() const;
  //retrun 1 if the function is convex, 0 otw
  int isConvex() const;
  //retrun 1 if the function is concave, 0 otw
  int isConcave() const;
    
  // giving x, return F(x)  (if F(x) undef, retourne -1)
  type operator()(type) const;

  int nbPiece;
  PieceList<type> F;
};

///////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION ///////////////////////////
///////////////////////////////////////////////////////////////////

/*template<typename type>
Function<type>::Function(int _nbPiece) : nbPiece(_nbPiece) {
  F.reserve(nbPiece);
}*/

template<typename type>
Function<type>::Function(PieceList<type> _F) : F(_F){
   nbPiece=(int)_F.size();
   assert(this->isContinuous() && this->isIncreasing());
 
}

template<typename type>
Function<type>::Function(const Function& G) : nbPiece(G.nbPiece) , F(G.F) {}


template<typename type>
int Function<type>::isContinuous() const{
  //consecutive interval and forall [t1,t2], [i1,i2] consecutive F(t2)=F(i1)
  for (int i=0;i<nbPiece-1;++i){
    if (!isEqual(F[i].I.t2,F[i+1].I.t1) ||
	!isEqual((*this)(F[i].I.t2),(*this)(F[i+1].I.t1)))
      return 0;
  }
  return 1;	
}

template<typename type>
int Function<type>::isIncreasing() const{
  //F(t1) < F(t2)	
  for (int i=0;i<nbPiece;++i){
    if ((*this)(F[i].I.t2) < (*this)(F[i].I.t1)) return 0;
  }
  return 1; 
}

template<typename type>
int Function<type>::isConvex() const{
  //slope is increasing
  type prev_slope=(F(F[0].I.t2) -F(F[0].I.t1))/(F[0].I.t2 - F[0].I.t1);
  type curr_slope;
  for (int i=1;i<nbPiece;++i){
    curr_slope = (F(F[i].I.t2) -F(F[i].I.t1))/(F[i].I.t2 - F[i].I.t1);
    if (curr_slope > prev_slope) return 0;
    prev_slope=curr_slope;
  }
  return 1; 
}

template<typename type>
int Function<type>::isConcave() const{
  //slope is decreasing
  type prev_slope=(F(F[0].I.t2) -F(F[0].I.t1))/(F[0].I.t2 - F[0].I.t1);
  type curr_slope;
  for (int i=1;i<nbPiece;++i){
    curr_slope = (F(F[i].I.t2) -F(F[i].I.t1))/(F[i].I.t2 - F[i].I.t1);
    if (curr_slope < prev_slope) return 0;
    prev_slope=curr_slope;
  }
  return 1; 
}

template<typename type>
void Function<type>::displayFunction() const{
  for (int i=0;i<nbPiece;++i)
      F[i].displayPiece();
}

template<typename type>
type Function<type>::operator()(type x) const{
  if ( x < F[0].I.t1 || x > F[nbPiece-1].I.t2)
    return std::numeric_limits<type>::min();
  else{
    int i=0;
    while (i < nbPiece){
      if (F[i].I.t1 <= x && F[i].I.t2 >= x)
	return F[i].f(x);
      else ++i;
    }
    return std::numeric_limits<type>::min();
  }
}

#endif
