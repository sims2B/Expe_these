#include "flot_model.h"

int main(int,char* argv[]){
  int nbTask;
  std::ifstream instance(argv[1],std::ios::in);
  instance >> nbTask;
  Problem<int> P(nbTask);
  P.readFromFile(instance);
  instance.close();

  //P.displayProblem();
  if (solve(P))
    std::cout << "Hurray!\n";
  else
    std::cout << "...\n";
  std::cout << "************************************************\n";
  if (flowTotalTest(P))
    std::cout << "Hurray!\n";
  else
    std::cout << "...\n";
  std::cout << "************************************************\n";
  return 0;
}
