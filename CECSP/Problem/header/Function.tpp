#include <limits>
#include <iostream>
#include <cassert>
#include <algorithm>

template<typename type>
Function<type>::Function(int _nbPiece) : nbPiece(_nbPiece) {
  F.reserve(nbPiece);
}

template<typename type>
Function<type>::Function(PieceList<type> _F) : F(_F){
  nbPiece=(int)_F.size();
}

template<typename type>
void Function<type>::displayFunction() const{
  for (int i=0;i<nbPiece;++i)
      F[i].displayPiece();
}

template<typename type>
Function<type>::Function(const Function& G) : nbPiece(G.nbPiece) , F(G.F) {}

template<typename type>
type Function<type>::operator()(type x) const{
  int i=0;
  while (i < nbPiece){
    if (F[i].I.t1 <= x && F[i].I.t2 >= x)
      return F[i].f(x);
    else ++i;
  }
  assert(i<=nbPiece);
  return std::numeric_limits<type>::min();
}
