#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <iomanip>
#include <vector> 
using namespace std;
struct res{
  int size;
  int status;
  double time_end;
  int nb_node;
  res();
};
res::res() : size(0), status(0),time_end (0.0), 
	      nb_node(0){}


int main( int argc, char* argv[]){
  std::ifstream res_file(argv[1],std::ios::in);
  if (!res_file)  {    
    std::cout << "Ouverture res impossible" << std::endl;
    return 1;
  }
  std::string trash;
  char prout;
  std::vector<res> res_vec;
  while (!res_file.eof()){
    res res_line;
    res_file.ignore(4);
    res_file >> res_line.size;
    res_file.ignore(1000,'\n');
    res_file.ignore(1000,'\n');
    res_file >> trash;
    //std::cout << trash << "eee \n";
    if (trash!= "..."){
      res_file.ignore(1000, ':');
      res_file >> trash;
      //std::cout << trash;
      if (trash == "Infeasible")
	res_line.status=1;
      else if (trash == "Optimal")
	res_line.status=3;
      else if (trash == "Feasible")
	res_line.status=2;
	res_file >> trash;
	res_file >> res_line.time_end;
	res_file >> trash;
	if (trash!="..."){
	  res_file.ignore(1000,':');
	  res_file >> res_line.nb_node;
	}
    }
    res_file.ignore(10000,'*');
    res_file.ignore(1000,'\n'); 
    res_vec.push_back(res_line);  
    
    if (res_file.eof()) break;
    /*    std:: cout << "$"<<
      res_line.size << " $& $" <<    
      res_line.time_end << "$ & $" <<
      res_line.nb_node << "$ & $"
	       << " $ \\\\ \n";
    */    
  }

  for (int j=0;j<5;++j){
    res res_line;
    res_line.time_end=0.0;
    int nb=0;
    int nb_sol=0;
    int nb_opt=0;
    int nb_feas=0;
    for (uint i=0; i<res_vec.size()-1;++i){
      if (j==0 && res_vec[i].size==10 ){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }
      
      if (j==1 && res_vec[i].size==20){	
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }
      
      if (j==2 && res_vec[i].size==25){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }
      
      if (j==3 && res_vec[i].size==30){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }
    
      if (j==4 && res_vec[i].size==60){
	nb++;
	res_line.size+=res_vec[i].size;
	if (res_vec[i].status>0){
	  nb_sol++;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.nb_node+= res_vec[i].nb_node;
	}
	if (res_vec[i].status>1){
	  nb_feas++;
	}
	if (res_vec[i].status==3) nb_opt++;
	if (res_vec[i].status==0) res_line.time_end+= 7200;
      }
    }
    if (nb !=0){
    res_line.size=res_line.size/nb;
    res_line.time_end= res_line.time_end/nb;}
    if (nb_sol !=0 ) res_line.nb_node= res_line.nb_node/nb_sol;
    
    if (nb_sol==0 && j==0) res_line.size=10;
    if (nb_sol==0 && j==1) res_line.size=20;
    if (nb_sol==0 && j==2) res_line.size=25;
    if (nb_sol==0 && j==3) res_line.size=30;
    if (nb_sol==0 && j==4) res_line.size=60;
    if (nb!= 0){std::cout << fixed << setprecision(2);
    std:: cout << "$"<<
      res_line.size << " $& $" <<    
      res_line.time_end << "$ & $" <<
      100*nb_opt/nb<< "$ & $" <<
      100*nb_sol/nb << "$ & $" <<
      res_line.nb_node 
	       << " $ \\\\ \n";
    }
  }
  res_file.close();
  
  return 0;
}
