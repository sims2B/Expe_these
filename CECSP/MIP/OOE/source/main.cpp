#include "onOffModel.h"
#include <iostream>
#include <fstream>


int main(int argc,char *argv[]){
  if (argc != 12)
    std::cout << "wring argument number";
  int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
  instance >> nbTask ;
  Problem<double,double> P(nbTask);
  P.readFromFile(instance);
  instance.close();
  Solution<double,double> s(nbTask,2*nbTask-1);
  std::vector<int> config(9,0);
  for (int i=2;i<11 && i<argc;++i)
    config[i-2]=atoi(argv[i]);
   if (!LPsolveOO(P,config))
    if (!solveOO(P,s,config))  {
      std::cout << "Hurray!\n";
      std::cout << "valide? " << s.isValid(P) << std::endl;}
    else{
      std::cout << "...\n";
      std::cout << "valide? " << s.isValid(P) << std::endl;
    }
  std::cout << "**************************************************" <<std::endl;
  return 0;
  /*
    double temps;
  Problem<double> Q(P);
  //std::ofstream instance_PL(argv[2],std::ios::out);
  //std::ofstream instance_L(argv[3],std::ios::out);
 for (int i=0;i<P.nbTask;++i)
    P.A[i].Wi=P.W(i)*0.5;
 while (solveOO(P,s,bound,bd,0,0,0,0,3600)){
   for (int i=0;i<P.nbTask;++i)
     P.A[i].Wi=P.W(i)*0.5;
 }  
 while (!solveOO(P,s,bound,bd,0,0,0,0,1000)){
   Q=P;
   for (int i=0;i<P.nbTask;++i)
     P.A[i].Wi=P.W(i)+P.W(i)*0.05;
 }
 std::cout << "valide? " << s.isValid(P) << std::endl;
 Q.ecrire(instance_PL);
 std::cout << nbTask << " " << Q.B << "\n";
  for (int i=0;i<Q.nbTask;++i){
    std:: cout <<  Q.r(i) << " "<< Q.d(i) << " " <<Q.W(i) << " "<<
      Q.bmin(i) << " " << Q.bmax(i) << " " << Q.A[i].Fi.nbPiece<< " ";
    for (int q=0; q< Q.A[i].Fi.nbPiece-1; ++q)
      std::cout << Q.A[i].Fi.F[q].f.a << " " << Q.A[i].Fi.F[q].f.c << 
	" " << Q.A[i].Fi.F[q].I.t2 << " ";
    std::cout << Q.A[i].Fi.F[Q.A[i].Fi.nbPiece-1].f.a << " " << Q.A[i].Fi.F[Q.A[i].Fi.nbPiece-1].f.c << "\n";
  }
  std::cout << "**************************************************" <<std::endl;
  for (int i=0;i<Q.nbTask;++i){
    int a,c;
    if (Q.bmin(i)==Q.bmax(i))
      a=0;
    else
      a=(Q.A[i].Fi(Q.bmin(i))-Q.A[i].Fi(Q.bmax(i)))/(Q.bmin(i)-Q.bmax(i));
    c=Q.A[i].Fi(Q.bmin(i)) - a *Q.bmin(i);
    Q.A[i].Fi.F[0].f.c=c;
    Q.A[i].Fi.F[0].f.a=a;
    Q.A[i].Fi.nbPiece=1;
  }
 Q.ecrire(instance_L);
 std::cout << nbTask << " " << Q.B << "\n";
  for (int i=0;i<Q.nbTask;++i){
    std:: cout <<  Q.r(i) << " "<< Q.d(i) << " " <<Q.W(i) << " "<<
      Q.bmin(i) << " " << Q.bmax(i) << " " << Q.A[i].Fi.nbPiece<< " ";
    std::cout << Q.A[i].Fi.F[0].f.a << " " << Q.A[i].Fi.F[0].f.c << "\n";
  }
  std::cout << "**************************************************" <<std::endl;

  if (atoi(argv[2])) boundSepEvts(P,bound);
  if (atoi(argv[3])) boundEvts(P,bd);

  if (atoi(argv[4])) addSep=1;
  if (atoi(argv[5])) addTe=1;
  if (atoi(argv[6])) addPreem=1;
  if (atoi(argv[7])) addKnapsack=1;
  */
  // if (!solveOO(P,s,bound,bd,addTe,addSep,addPreem,addKnapsack,atof(argv[8])))
  
  /// !!!!!!!!!!!!!!! Changer constriante Bmin

}
