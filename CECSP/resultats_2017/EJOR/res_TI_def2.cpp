#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector> 
#include <iomanip>
using namespace std;
struct res{
  int size;
  double time_first_sol;
  double gap_first_sol;
  double time_end;
  double gap_end;
  double obj_first;
  double best_obj;
  double obj;
  int trouve;
  int solved;
  int opt;
  //constructeur par defaut, cas unsolved?
  res();
};

struct cell{
  string name;
  double best;
};
res::res() : size(0), time_first_sol(0.0), gap_first_sol(0.0), time_end(0.0), gap_end(0.0), trouve (0) , solved(0),opt(0),obj_first(0.0) , obj (0.0){}

int main( int argc, char* argv[]){

  std::ifstream res_file(argv[1],std::ios::in);
  std::vector<cell> cell_vec;
  std::ifstream res_file2(argv[2],std::ios::in);
  cell_vec.resize(1000);
  int cpt=0;
  while (!res_file2.eof()){
    res_file2 >> cell_vec[cpt].name; 
    res_file2 >> cell_vec[cpt].best; 
    cpt++;
  }
  res_file2.close();
  
  if (!res_file)  {    
    std::cout << "Ouverture res impossible" << std::endl;
    return 1;
  }
  int nbTO=0;
  int j=0;
  int size;
  double val;
  std::string trash;
  char prout;
  cpt=0;
  std::vector<res> res_vec;
  while (!res_file.eof()){  
    if (prout == 'i')
      res_file.seekg(-1, res_file.cur);
    string inst_name;
    res res_line;
    getline(res_file, inst_name);
    for (int i=0;i<cell_vec.size();++i)
      if (inst_name == cell_vec[i].name)
	res_line.best_obj=cell_vec[i].best;
    res_file.seekg(-inst_name.size()-1, res_file.cur);
    res_line.solved=1;
    res_file.ignore(4);
    res_file >> size;
    res_line.size=size;
    res_file.ignore(1000,'\n');
    res_file.ignore(1000,'\n');
    res_file.ignore(1000,'\n');
    res_file >> trash;
    if (trash == "status"){     
      res_file.ignore(1000,':');
      res_file >> trash;
      res_file >> trash;
      res_file >> val;
      res_line.time_end=val;
      res_line.trouve=0;
      res_line.solved=1;
    }
    else if (trash== "Iloexception") {
      nbTO++;
      res_line.time_first_sol=7200;
      res_line.time_end=7200;
      res_line.trouve=0;
      res_line.solved=0;
      res_line.opt=0;
    }
    /*if (trash== "Sol")*/else {
      res_file.ignore(1000,':');
      res_file >> val;
      res_line.obj_first=val;
      res_file.ignore(1000,':');
      res_file >> val;
      res_line.time_first_sol=val;
      res_file.ignore(1000,':');
      res_file >> val;
      res_line.gap_first_sol=val;
      res_file >> trash;
      if (trash=="**************************************************" || trash=="Iloexception"){
	nbTO++;
	res_line.time_end=7200;
	res_line.trouve=0;
	res_line.solved=0;
	res_line.opt=0;
	if (trash=="**************************************************"){
	  res_file.ignore(1000,'\n');
	  res_vec.push_back(res_line);
	  if (res_file.eof()) break;
	  else continue;
	}
      }
      else{
	res_file.ignore(1000,':');
	res_file >> trash;
	if (trash=="Feasible"){
	  res_line.opt=0;
	  res_line.trouve=1;
	  res_line.solved=1;
	  res_line.time_end=7200;
	  res_file.ignore(1000,':');
	  res_file >> val;
	  res_line.obj=val;
	}
	else{
	  res_line.trouve=1;
	  res_line.solved=1;
	  res_line.opt =1;
	  res_file >> trash;
	  res_file >> val;
	  res_line.time_end=min(7200.0,val);
	  res_file.ignore(1000,':');
	  res_file >> val;
	  res_line.obj=val;
	}   
	res_file.ignore(1000,'\n');
	res_file.ignore(1000,':');
	res_file >> val;
	res_line.gap_end=val;
      }
    }
    res_file.ignore(10000,'*');
    res_file.ignore(1000,'\n');
    std:: cout << "$"<<
      res_line.size << "$ & $" <<
      res_line.time_first_sol << "$ & $" <<
      res_line.gap_first_sol <<"$ & $" << 
      res_line.time_end  << "$ & $" <<
      res_line.gap_end << "$ & $" << res_line.solved <<  "$ & $" 
		    << res_line.opt<<  " $ \\\\ \n";
    res_vec.push_back(res_line);
    prout=res_file.get();  

    if (res_file.eof()) break;
  }

  std:: cout << "\\begin{center}\n";
  std:: cout << "	\\begin{tabular}{|c|cc|cccc|}\n";
  std:: cout << "	\\hline\n";
  std:: cout << "   \\multirow{2}{*}{\\#tasks} & \\multicolumn{2}{c|}{first sol.}&                                                                  \\multicolumn{4}{c|}{final sol.} \\\\ \n "; 
  std:: cout << "	\\cline{2-7} \n";
  std:: cout << " & time(s) & gap & time & gap &\\%solved &  \\%opt \\\\ \n";
  std:: cout << " \\hline \n";


  for (int j=0;j<5;++j){
    res res_line;
    int nb=0;
    int nb_trouv=0;
    int nb_sol=0;
    for (uint i=0; i<res_vec.size();++i){
      if (j==0 && res_vec[i].size==10 ){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.solved+=res_vec[i].solved;
	res_line.time_end+= res_vec[i].time_end;
	if (res_vec[i].trouve)
	  res_line.opt+=res_vec[i].opt;
	if (res_vec[i].trouve){
	    ++nb_trouv;
	  if (res_vec[i].solved ){
	    res_line.gap_first_sol+=100*(1- res_vec[i].best_obj/res_vec[i].obj_first);
	    res_line.time_first_sol+=res_vec[i].time_first_sol;
	    if (res_vec[i].opt || res_vec[i].gap_end!=0.0) {
	      ++nb_sol;
	      res_line.gap_end+=res_vec[i].gap_end;
	    }
	  }
	}
      }
      if (j==1 && res_vec[i].size==20){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.solved+=res_vec[i].solved;
	res_line.time_end+= res_vec[i].time_end;
	if (res_vec[i].trouve)
	  res_line.opt+=res_vec[i].opt;
	if (res_vec[i].trouve){
	    ++nb_trouv;
	  if (res_vec[i].solved ){
	    res_line.gap_first_sol+=100*(1- res_vec[i].best_obj/res_vec[i].obj_first);
	 	    res_line.time_first_sol+=res_vec[i].time_first_sol;
	    if (res_vec[i].opt || res_vec[i].gap_end!=0.0) {
	      ++nb_sol;
	      res_line.gap_end+=res_vec[i].gap_end;
	    }
	  }
	}
      }
      if (j==2 && res_vec[i].size==25){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.solved+=res_vec[i].solved;
	res_line.time_end+= res_vec[i].time_end;
	if (res_vec[i].trouve){
	  res_line.opt+=res_vec[i].opt;
	  ++nb_trouv;
	  if (res_vec[i].solved ){
	    res_line.gap_first_sol+=100*(1- res_vec[i].best_obj/res_vec[i].obj_first);
	    res_line.time_first_sol+=res_vec[i].time_first_sol;
	    if (res_vec[i].opt || res_vec[i].gap_end!=0.0) {
	      ++nb_sol;
	      res_line.gap_end+=res_vec[i].gap_end;
	    }
	  }
	}
      }

      if (j==3 && res_vec[i].size==30){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.solved+=res_vec[i].solved;
	res_line.time_end+= res_vec[i].time_end;
	if (res_vec[i].trouve)
	  res_line.opt+=res_vec[i].opt;
	if (res_vec[i].trouve){
	    ++nb_trouv;
	  if (res_vec[i].solved ){
	    res_line.gap_first_sol+=100*(1- res_vec[i].best_obj/res_vec[i].obj_first);
	    res_line.time_first_sol+=res_vec[i].time_first_sol;
	    if (res_vec[i].opt || res_vec[i].gap_end!=0.0) {
	      ++nb_sol;
	      res_line.gap_end+=res_vec[i].gap_end;
	    }
	  }
	}
      }
      if (j==4 && res_vec[i].size==60){
	nb++;
	res_line.size+=res_vec[i].size;
	res_line.solved+=res_vec[i].solved;
	res_line.time_end+= res_vec[i].time_end;
	if (res_vec[i].trouve)
	  res_line.opt+=res_vec[i].opt;
	if (res_vec[i].trouve){
	    ++nb_trouv;
	  if (res_vec[i].solved ){
	    res_line.gap_first_sol+=100*(1- res_vec[i].best_obj/res_vec[i].obj_first);
	    res_line.time_first_sol+=res_vec[i].time_first_sol;
	    if (res_vec[i].opt || res_vec[i].gap_end!=0.0) {
	      ++nb_sol;
	      res_line.gap_end+=res_vec[i].gap_end;
	    }
	  }
	}
      }
    }
    if (nb != 0){
      res_line.size=res_line.size/nb;
      res_line.solved=100*res_line.solved / nb;
      res_line.time_end= res_line.time_end/nb;
    }
    if( nb_trouv!= 0){
      res_line.gap_first_sol=res_line.gap_first_sol/nb_trouv;
      res_line.time_first_sol=res_line.time_first_sol/nb_trouv;
      res_line.opt= 100*res_line.opt / nb_trouv;
    }
    if (nb_sol!=0){
      res_line.gap_end=res_line.gap_end/nb_sol;
    }
    if (nb_sol==0 && j==0) res_line.size=10;
    if (nb_sol==0 && j==1) res_line.size=20;
    if (nb_sol==0 && j==2) res_line.size=25;
    if (nb_sol==0 && j==3) res_line.size=30;
    if (nb_sol==0 && j==4) res_line.size=60;
    
    std:: cout << "$"<<
      res_line.size << "$ & $" <<
      res_line.time_first_sol << "$ & $" <<
      res_line.gap_first_sol <<"$ & $" << 
      res_line.time_end  << "$ & $" <<
      res_line.gap_end << "$ & $" << res_line.solved <<  "$ & $" 
		    << res_line.opt<<  " $ \\\\ \n";
    
  }
  std::cout << "\\hline \n";
  std::cout << "\\end{tabular}\n";
  std::cout << "\\end{center}\n";
  cout << nbTO << endl;
  res_file.close();
  return 0;
}
