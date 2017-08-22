#ifndef lot_problem
#define lot_problem

#include<vector>
#include <fstream>

//Batch minimum size
#define minSize 1

#define M 100000

// To avoid problem with non integer
#define NEGATIVE_ZERO -0.00001
#define POSITIVE_ZERO 0.00001

//coefficient for the objective function
#define ALPHA 1
#define BETA 499
#define GAMMA 1000

struct LotProblem
{
  int nbLot;
  int nbLine;
  double horizon;
  //machine consumption (MW/H)
  double delta;
  //nb of authorized sublot
  int nbSublot;
  //nb peak hour periods
  int nbHP;
  //unauthorized machines ( if l in subset[i] then we can not use line l to produce i)
  std::vector< std::vector<int> > subset;
  //setup
  std::vector< std::vector<double> > setup;
  //speed of i on machine l (speed[i][l])
  std::vector< std::vector<double> > speed;
  //demand of each lot
  std::vector<double> demand;
  //start time of peak hour
  std::vector<double> beginHP;
  //end time of peak hour
  std::vector<double> endHP;
  // maintenance duration
  std::vector<double> durMaint;


  LotProblem();


  LotProblem(int n,int m,int Q, int t);

  int readFromFile(std::ifstream &instance);

  int displayProblem() const;
  
  //return 1 if i can be executed on k
  int isAuthorized(int i, int k) const;
  //return 1 if i and k can be on the same line
  int sameLine(int i, int k) const;
  //chack if the setup time verify triangular inequality
  int triangularSetup() const;
  int makeTriangularSetup();
};
#endif
