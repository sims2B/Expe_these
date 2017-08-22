#include "model_RCPSP_SEE.h"

int main(int /*argc*/, char* argv[]){
  std::ifstream instance(argv[1],std::ios::in);
  if (!instance)  {    
    std::cout << "Ouverture fichier instance impossible" << std::endl;
    return 1;
  }
  int _nbTask,nbRes;
  instance >> _nbTask >> nbRes;
  problem P(_nbTask,nbRes);
  P.readFromFile(instance);
  instance.close();
  std::ifstream TW(argv[2],std::ios::in);
  if (TW) P.timeWindows(TW);
  P.displayProblem();
  solution s(_nbTask);
  solveSEE(P,s);
  std::cout << "is valid?? " << s.isValid(P) << std::endl;
  return 0;
}
