#include "startEndModel.h"
#include <iostream>
#include <fstream>


int main(int argc,char *argv[]){
  int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
  instance >> nbTask ;
  Problem<double> P(nbTask);
  P.readFromFile(instance);
  instance.close();
  Solution<double> s(nbTask,2*nbTask);
  
  std::vector<int> config(7,0);
  for (int i=2;i<9 && i<argc;++i)
    config[i-2]=atoi(argv[i]);
  if (!solveSE(P,s,config))  
    std::cout << "Hurray!\n";
  else
    std::cout << "...\n";
  //s.printSol();
  std::cout << "valide? " << s.isValid(P) << std::endl;
  std::cout << "**************************************************" <<std::endl;
  return 0;
}
