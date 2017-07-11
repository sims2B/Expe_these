#include "Solution.hpp"
#include <iostream>
#include <fstream>

int main(int,char* argv[]){
  int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
  instance >> nbTask ;
  Problem<int> P(nbTask);
  P.readFromFile(instance);
  instance.close();
  // P.displayProblem();

  
  Task<int> T= P.A[0];
  std:: cout << "Energetic reasonning return " << 
    P.energeticReasonning(Interval<int>(0,2),
			  P.totalResourceConsumption(Interval<int>(0,2))) 
	     << std::endl;
  T.displayTask();
  std::cout << std::endl;
  std::cout << "left.shift = " << T.leftShift(Interval<int>(0,2))<< std::endl;
  std::cout << "right.shift = " << T.rightShift(Interval<int>(0,2))<< std::endl;

  std::cout << "energy requirement= " 
	    << T.energyConsumption(Interval<int>(0,2))<< std:: endl;
  std::cout << "resource consumption= " 
  << T.resourceConsumption(Interval<int>(0,2))<< std:: endl;

   std::cout << " \n ////////////////////TEST LINEAR FUNCTION/////////////////////// \n";

   /* LinearFunction<int> fint;
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

  std::cout << "  \n  ////////////////////////////TEST INTERVAL/////////////////////////// \n";

  Interval<double> Idoub(1,3);
  Interval<int> Jint(4,7);
  Interval<double> Jdoub(1,2);
  Interval<int> Iint(1,4);
  Interval<double> Kdoub(0,1);
  Interval<int> Kint(7,9);

  Iint.displayInterval();
  Idoub.displayInterval();
  Jint.displayInterval();
  Jdoub.displayInterval();
  
  std:: cout << "atttendu: sizeIntersection [1,3] et [1,2]=1 \t obtenu: " 
	     <<sizeIntersection(Idoub,Jdoub)<< std::endl;
  std:: cout << "atttendu: sizeIntersection [1,4] et [4,7]=0 \t obtenu: " 
	     <<sizeIntersection(Iint,Jint)<< std::endl;

    std::cout << " \n //////////////////////////////TEST PIECE//////////////////////////////// \n";
 
  Piece<int> Pint(Iint);
  Piece<double> Pdoub(Idoub,f);
  Piece<int> Qint(Jint);
  Piece<double> Qdoub(Jdoub,g);
  Piece<int> Rint(Kint,fint);
  Piece<double> Rdoub(Kdoub,f);

  Pint.displayPiece();
  Pdoub.displayPiece();
  Qint.displayPiece();
  Qdoub.displayPiece();
  Rint.displayPiece();
  Rdoub.displayPiece();
 
   std::cout << " \n //////////////////////////TEST FUNCTION//////////////////////////// \n";

   //Function<int> Fint(1);
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
   
   std::cout << " ********** Affichage de la fonction Fdoub ************\n";
   Fdoub.displayFunction();  
   std::cout << " ********** Affichage de la fonction Hdoub ************\n";
   Hdoub.displayFunction();
   
   double c3=2;
   double c4=7;
   std:: cout << "atttendu: Gi(2)=2 \t obtenu: " << Gint(c3)<< std::endl;
   std:: cout << "atttendu: Hd(2)=? \t obtenu: " << Hdoub(c3)<< std::endl;
   
   std:: cout << "atttendu: Gd(2)==Fd(7) =>?  \t obtenu: " 
   << isEqual(Gdoub(c3),Fdoub(c4)) << std::endl;
 
  std::cout << " \n //////////////////////////////TEST TASK//////////////////////////////// \n";
  
  Task<double> un(0,3,8,2,3,Fdoub);
  Task<double> deux(1,6,8,1,2,Gdoub);
  Task<double> trois(2,8,4,1,1,Hdoub);
  Task<double> quatre(2,8,7,1,2,Gdoub);
  un.displayTask();
  std::cout << std::endl;
  std::cout << "energy Cons of 1 in [0,0.2]=> 0 \t obtenu: " <<un.energyConsumption(Interval<double>(0,0.2))<< std::endl;
  std::cout << "energy Cons of 1 in [0,2]=> 5 \t obtenu: " <<un.energyConsumption(Interval<double>(0,2))<< std::endl;
  std::cout << "energy Cons of 1 in [2.8,3]=> 0 \t obtenu: " <<un.energyConsumption(Interval<double>(2.8,3))<< std::endl;
  std::cout << "energy Cons of 1 in [1,3]=> 5 \t obtenu: " <<un.energyConsumption(Interval<double>(1,3))<< std::endl;
  std::cout << "energy Cons of 1 in [0,4]=> 8 \t obtenu: " <<un.energyConsumption(Interval<double>(0,4))<< std::endl;
  std::cout << "energy Cons of 1 in [0,0.5]=> 0.5 \t obtenu: " <<un.energyConsumption(Interval<double>(0,0.5))<< std::endl;
  std::cout << "energy Cons of 1 in [2.5,3]=> 0.5 \t obtenu: " <<un.energyConsumption(Interval<double>(2.5,3))<< std::endl;
  std::cout << "energy Cons of 1 in [1,2]=> 2 \t obtenu: " <<un.energyConsumption(Interval<double>(1,2))<< std::endl;
  std::cout << "energy Cons of 1 in [0.5,2]=> 3.5 \t obtenu: " <<un.energyConsumption(Interval<double>(0.5,2))<< std::endl;
  std::cout << "left of 1 in [1,3]=> 2 \t obtenu: " <<un.leftShift(Interval<double>(1,3))<< std::endl;
  std::cout << "right of 1 in [1,3]=> 5 \t obtenu: " <<un.rightShift(Interval<double>(1,3))<< std::endl;
  std::cout << "both of 1 in [1,3]=> 2 \t obtenu: " <<un.bothShift(Interval<double>(1,3))<< std::endl;*/

   return 0;
 
}
