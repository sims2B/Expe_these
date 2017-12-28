#include "timeModel.hpp"
#include <iostream>
#include <fstream>


int main(int,char *argv[]){
  int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
 
  instance >> nbTask ;
  Problem<int,double> P(nbTask);
  P.readFromFile(instance);
  instance.close();
  Solution<int,double> s(nbTask,P.D);
  if (!solve(P,s,"linear")){
    //    s.displaySolution();
    std::ofstream output(argv[2],std::ios::out);
    Problem<int,double> Q(P);
    Q.addLinearFunction();
    for (int i = 0 ; i < Q.nbTask ; ++i){
      double W = 0.0;
      for (uint t = 0 ; t < s.b[i].size() ; ++t){
	if (!isEqual(s.b[i][t],0.0))
	  W += Q.A[i].Fi(s.b[i][t]);
      }
      Q.A[i].Wi=(int)floor(W);
      if (Q.A[i].Wi < Q.A[i].Fi(Q.bmin(i)) )
	Q.A[i].Wi=Q.A[i].Fi(Q.bmin(i));
    }
    Q.writeInFile(output);
     std::cout << "Hurray!\n"; 
    std::cout << "valide? " << s.isValid(P) << std::endl; 
    
  }
  else  
    std::cout << "...\n";
  std::cout << "**************************************************" <<std::endl;
  
  return 0;
}
/*
  return 0;
  }

  ///////////////////////Ecriture solution dans un fichier ///////////////////////
  //////////////plus création instance a partir de la solution //////////////
    
   
  std::ofstream instance_PL(argv[3],std::ios::out);
  s.writeInFile(output);
  output.close();
  Problem<int,double> Q(P);
  Q.addConvexPiecewiseFunction();
  for (int i=0;i<Q.nbTask;++i){
  double W=0.0;
  for (uint t=0;t<s.b[i].size();++t){
  if (!isEqual(s.b[i][t],0.0))
  W= W+Q.A[i].Fi(s.b[i][t]);
  }
  Q.A[i].Wi=floor(W);
  }
  Q.writeInFile(instance_PL);


  std::ifstream TW(argv[2],std::ios::in);
  P.readtimeWindows(TW);
  
  Solution<int,double> s(nbTask,P.D);  
  if (!LPsolveConvex(P)){
 
  }

 
*/
