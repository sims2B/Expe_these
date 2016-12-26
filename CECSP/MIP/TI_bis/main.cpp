#include "time_model.h"
#include <iostream>
#include <fstream>


int main(int,char *argv[]){
  int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
  instance >> nbTask ;
  Problem<double> P(nbTask);
  P.readFromFile(instance);
  instance.close();
  
  Solution<double,double> s(nbTask,P.D+1);  
  if (Solve(P,s,atof(argv[2]),atoi(argv[3])))
    std::cout << "Hurray!\n"; 
      else  
    std::cout << "...\n";
  std::cout << "valide? " << s.isValid(P) << std::endl;
  std::cout << "**************************************************" <<std::endl;

  return 0;
}

  // std::ofstream instance_PL(argv[2],std::ios::out);
  //std::ofstream instance_L(argv[3],std::ios::out);
  /*
  for (int i=0;i<P.nbTask;++i){
    P.A[i].Wi=P.W(i)-P.W(i)*0.05;
}
  // std::cout << "valide? " << s.isValid(P) << std::endl;
  P.ecrire(instance_PL);
  std::cout << nbTask << " " << P.B << "\n";
  for (int i=0;i<P.nbTask;++i){
    std:: cout <<  P.r(i) << " "<< P.d(i) << " " <<P.W(i) << " "<<
      P.bmin(i) << " " << P.bmax(i) << " " << P.A[i].Fi.nbPiece<< " ";
    for (int q=0; q< P.A[i].Fi.nbPiece-1; ++q)
      std::cout << P.A[i].Fi.F[q].f.a << " " << P.A[i].Fi.F[q].f.c << 
	" " << P.A[i].Fi.F[q].I.t2 << " ";
    std::cout << P.A[i].Fi.F[P.A[i].Fi.nbPiece-1].f.a << " " << P.A[i].Fi.F[P.A[i].Fi.nbPiece-1].f.c << "\n";
  }
  std::cout << "**************************************************" <<std::endl;
  for (int i=0;i<P.nbTask;++i){
    int a,c;
    if (P.bmin(i)==P.bmax(i))
      a=0;
    else
      a=(P.A[i].Fi(P.bmin(i))-P.A[i].Fi(P.bmax(i)))/(P.bmin(i)-P.bmax(i));
    c=P.A[i].Fi(P.bmin(i)) - a *P.bmin(i);
    P.A[i].Fi.F[0].f.c=c;
    P.A[i].Fi.F[0].f.a=a;
    P.A[i].Fi.nbPiece=1;
  }
 //P.ecrire(instance_L);
 std::cout << nbTask << " " << P.B << "\n";
  for (int i=0;i<P.nbTask;++i){
    std:: cout <<  P.r(i) << " "<< P.d(i) << " " <<P.W(i) << " "<<
      P.bmin(i) << " " << P.bmax(i) << " " << P.A[i].Fi.nbPiece<< " ";
    std::cout << P.A[i].Fi.F[0].f.a << " " << P.A[i].Fi.F[0].f.c << "\n";
  }
  std::cout << "**************************************************" <<std::endl;
*/
