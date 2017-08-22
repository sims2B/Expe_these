#include "LotProblem.h"
#include <vector> 
#include <iostream>
#include <fstream>
#include <sstream>
  
  LotProblem::LotProblem()
  {}

  LotProblem::LotProblem(int n,int m,int Q, int t) : nbLot(n) , nbLine(m), horizon(0), delta(0), nbSublot(Q) , nbHP(t)
  {
    subset.resize(n);
    setup.resize(n);
    speed.resize(n);
    demand.reserve(n);
    beginHP.reserve(t);
    endHP.reserve(t);
    durMaint.reserve(m);

    for (int i=0;i<n;++i)
      {
	setup[i].reserve(n);
	speed[i].reserve(m);
	subset[i].reserve(m);
      }
  }


int LotProblem::readFromFile(std::ifstream &instance)
{
  int n=nbLot;
  int m=nbLine;
  //int Q=nbSublot;
  int t=nbHP;
  std::string line;
  int i,j;
  double val;
  instance >> val;
  horizon=val;
  instance >> val;
  delta=val;

  for (i=0;i<n;++i){
    instance >> val;
    demand.push_back(val);
  }
  
  for (i=0;i<n;++i){
    for (j=0;j<n;++j){
      instance >> val;
      setup[i].push_back(val);
    }
  }

  for (i=0;i<n;++i){
    for (j=0;j<m;++j){
      instance >> val;
      speed[i].push_back(val);
    }
  }
  

    std::getline(instance,line);
  //ok if there is less than 10 machineq
    for (i=0;i<n;++i){
    std::getline(instance,line);
    int nb=(line.size()+1)/2;
    std::stringstream s(line);
    for (j=0;j<nb;++j){
      s >> val;
      if (val!=-1)
	subset[i].push_back(val);
    }
  }
  for (j=0;j<m;++j){
    instance >> val;
    durMaint.push_back(val);
  }
  
  //hp de 18 a 23 tous les jours
  for (i=0;i<t;++i){
    beginHP.push_back(24*i+18);
    endHP.push_back(24*i+23);
  }
  return 0;
}


int LotProblem::displayProblem() const
{
  std:: cout << "the number of lot is " << nbLot << std::endl;
  std:: cout << "the number of ligne is " << nbLine << std::endl;
  std:: cout << "the number of sous-lot is " << nbSublot << std::endl;
  std:: cout << "the number of HP is " << nbHP << std::endl;
  std:: cout << "the common deadline is " << horizon << std::endl;
  std::cout << "THe machine consumption is " << delta << std::endl;

  for (int i=0;i<nbLot;++i) {
    std::cout << "The unauthorized machines for job " << i << " are: ";
    for (uint k=0;k<subset[i].size();++k) 
      std::cout << subset[i][k] << " ";
    std::cout <<std::endl;
  }

  std::cout << "THe speeds are: " << std::endl;
  for (int i=0;i<nbLot;++i) {
    for (int k=0;k<nbLine;++k) 
      std::cout << speed[i][k] << " ";
    std::cout <<std::endl;
  }
  
  std::cout << "The demands are " << std::endl;
  for (int i=0;i<nbLot;++i)
    std::cout << demand[i] << " ";
  std::cout <<std::endl;

  std::cout << "The setup are:" << std::endl;
  for (int i=0;i<nbLot;++i) {
    for (int k=0;k<nbLot;++k) 
      std::cout << setup[i][k] << " ";
    std::cout <<std::endl;
  }
  return 0;
}

int LotProblem::isAuthorized(int i, int k) const{
  for (int j=0;j<subset[i].size();++j){
    if (subset[i][j]==k)
      return 0;
  }
  return 1;
}

int LotProblem::sameLine(int i, int j) const{
  for (int k=0;k<nbLine;++k){
    if (isAuthorized(i,k) && isAuthorized(j,k))
      return 1;
  }
  return 0;
}

int LotProblem::triangularSetup() const {
  for (int i=0;i<nbLot;++i) {
    for (int j=0;j<nbLot;++j) {
      for (int k=0;k<nbLot;++k) {
	if (sameLine(i,j) && sameLine(j,k) && setup[i][j]+setup[j][k] < setup[i][k])
	  return 0;
      }
    }
  }
  return 1;
}

int LotProblem::makeTriangularSetup() {
  for (int i=0;i<nbLot;++i) {
    for (int j=0;j<nbLot;++j) {
      for (int k=0;k<nbLot;++k) {
	if (sameLine(i,j) && sameLine(j,k) && setup[i][j]+setup[j][k] < setup[i][k])
	  {
	    if (setup[j][k]==0)
	      setup[j][k]=setup[i][k]-setup[i][j];
	    else if (setup[i][j]==0)
	      setup[i][j]=setup[i][k]-setup[j][k];
	    else
	      setup[i][k]=setup[i][j]+setup[j][k];
	  }
      }
    }
  }
  return 0;
}
