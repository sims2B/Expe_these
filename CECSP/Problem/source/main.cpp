#include "Solution.h"
#include <iostream>
#include <fstream>

int main(int,char * argv[]){


int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
  instance >> nbTask ;
  Problem<double> P(nbTask);
  P.readFromFile(instance);
  instance.close();
  P.displayProblem();

  // std:: cout << "Energetic reasonning return " << P.energeticReasonning(Interval<int>(0,8)) << std::endl;
  Task<double> T= P.A[0];
  T.displayTask();/*
  std::cout << "left.shift = " << T.leftShift(Interval<int>(0,8))<< std::endl;
  std::cout << "right.shift = " << T.rightShift(Interval<int>(0,8))<< std::endl;
  std::cout << "both.shift = " << T.bothShift(Interval<int>(0,8))<< std::endl;

  std::cout << "energy requierement= " 
	    << T.energyConsumption(Interval<int>(0,8))<< std:: endl;
  std::cout << "resource consumption= " 
  << T.resourceConsumption(Interval<int>(0,8))<< std:: endl;*/
  return 0;

  /*
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////TEST LINEAR FUNCTION///////////////////////
  //////////////////////////////////////////////////////////////////////////////
  LinearFunction<int> fint;
  LinearFunction<int> gint(2,3);
  LinearFunction<double> f;
  LinearFunction<double> g(2.0,3.0);

  f.displayLinearFunction();
  g.displayLinearFunction();
  
  double c1= 2;
  double c2=7;
  std:: cout << "atttendu: f(2)=2 \t obtenu: " << f(c1)<< std::endl;
  std:: cout << "atttendu: g(2)=7 \t obtenu: " << g(c1)<< std::endl;
  
  std:: cout << "atttendu: g(2)==f(7) => true  \t obtenu: " 
<< isEqual(g(c1),f(c2)) << std::endl;
  
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////TEST INTERVAL///////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  Interval<int> Iint;
  Interval<double> Idoub(1,3);
  Interval<int> Jint(2,7);
  Interval<double> Jdoub(1,2);
  Interval<double> Kdoub(1,1);

Iint.displayInterval();
  Idoub.displayInterval();
  Jint.displayInterval();
  Jdoub.displayInterval();
  
  std:: cout << "atttendu: sizeIntersection [0,0] et [-2,10]=0 \t obtenu: " 
  <<sizeIntersection(Idoub,Jdoub)<< std::endl;



  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////TEST PIECE////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  Piece<int> Pint;
  Piece<double> Pdoub(Idoub,f);
  Piece<int> Qint(Jint);
  Piece<double> Qdoub(Jdoub,f);
  Piece<int> Rint(Jint,fint);
  Piece<double> Rdoub(Kdoub,f);

  Pint.displayPiece();
  Pdoub.displayPiece();
  Qint.displayPiece();
  Qdoub.displayPiece();
  Rint.displayPiece();
  Rdoub.displayPiece();

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////TEST FUNCTION////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  Function<int> Fint(1);
  PieceList<int> PLint;
  PLint.push_back(Pint);
  PLint.push_back(Qint);
  PLint.push_back(Rint);
  PieceList<double> PLdoub;
  PLdoub.push_back(Pdoub);
  Function<double> Fdoub(PLdoub);
  PLdoub[0]=Qdoub;
  Function<double> Gdoub(PLdoub);
  PLdoub[0]=Rdoub;
  Function<int> Gint(PLint);
  Function<int> Hint(Gint);
  Function<double> Hdoub(Gdoub);

  Fdoub.displayFunction();
  Hdoub.displayFunction();

  double c1=2;
  double c2=7;
  std:: cout << "atttendu: Gi(2)=2 \t obtenu: " << Gint(c1)<< std::endl;
  std:: cout << "atttendu: Hd(2)=? \t obtenu: " << Hdoub(c1)<< std::endl;
  
  std:: cout << "atttendu: Gd(2)==Fd(7) =>?  \t obtenu: " 
<< isEqual(Gdoub(c1),Fdoub(c2)) << std::endl;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////TEST TASK////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  
  Task<double> un(0,3,5,1,3,Fdoub);
  Task<double> deux(1,6,8,1,2,Gdoub);
  Task<double> trois(2,8,4,1,1,Hdoub);
  Task<double> quatre(2,8,7,1,2,Gdoub);
  un.displayTask();
  std::cout << "energy Cons of 4 in [4,6]=> 0 \t obtenu: " <<un.resourceConsumption(Interval<double>(4,6))<< std::endl;
  std::cout << "left of 1 in [1,3]=> 2 \t obtenu: " <<un.leftShift(Interval<double>(1,3))<< std::endl;
  std::cout << "right of 1 in [1,3]=> 5 \t obtenu: " <<un.rightShift(Interval<double>(1,3))<< std::endl;
  std::cout << "both of 1 in [1,3]=> 2 \t obtenu: " <<un.bothShift(Interval<double>(1,3))<< std::endl;
*/

}
