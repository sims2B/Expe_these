#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector> 
#include <iomanip>
using namespace std;
struct res_cut{
  int size;
  double time_first_sol;
  double obj_first_sol;
  double time_end;
  double obj_end;
  double solved;
  double opt;
  double dev;
  //constructeur par defaut, cas unsolved?
  res_cut();
};
res_cut::res_cut() : size(0), time_first_sol(0.0), obj_first_sol(0), time_end(0.0), obj_end(0), solved(0), opt(0),dev(0.0){}




int main( int argc, char* argv[]){
  
  std::ifstream res_file(argv[1],std::ios::in);
  if (!res_file)  {    
    std::cout << "Ouverture res impossible" << std::endl;
    return 1;
  }
  int j=0;
  int size;
  double val;
  std::string trash;
  char prout;
  int solved,opt;
  std::vector<res_cut> res_vec;
  
  while (!res_file.eof()){
    res_cut res_line;
    res_file.ignore(4);
    res_file >> size;
    res_line.size=size;
    res_file.ignore(100, '\n');
    res_file.ignore(100, '\n');
    res_file >> trash;
    if (trash=="Sol"){
      res_file.ignore(100, ':');
      res_file >> res_line.obj_first_sol;
      res_file.ignore(1000,':');
      res_file >> res_line.time_first_sol;
      res_file.ignore(1000,':');
      res_file.ignore(1000,':');
      res_file >> trash;
      if (trash=="Feasible"){
	res_line.solved=1; res_line.opt=0;}
      if (trash=="Optimal"){
	res_line.solved=1; res_line.opt=1;}
      res_file >> trash;
      res_file >> res_line.time_end;
      res_file.ignore(1000,':');
      res_file >> val;
      res_line.dev=((res_line.obj_first_sol-val)/(double)val)*100;
      res_file.ignore(1000,':');
      res_file >> val;
      res_line.obj_end=val;
      res_vec.push_back(res_line);
    }

    else if (trash=="status"){	 
      res_file >> trash;
      res_file >> trash;
      res_file >> res_line.time_end;
      res_line.opt=1;
      res_line.solved=1;
      res_vec.push_back(res_line);
    }
    else if (trash=="..."){	    
      res_line.opt=0;
      res_line.solved=0;
      res_vec.push_back(res_line);
    }
    if (res_file.eof()) break;
    res_file.ignore(1000,'*');
    res_file.ignore(1000,'\n');
  }
  std:: cout << "\\begin{center}\n";
  std:: cout << "	\\begin{tabular}{|c|cc|cccc|}\n";
  std:: cout << "	\\hline\n";
  std:: cout << "	\\#tasks & \\multicolumn{2}{|c|}{first sol.}& \\multicolumn{4}{c|}{final sol.}\\\\ \n";
  std:: cout << "	\\hline \n";
  std:: cout << " & time(s) &  dev & time &gap &\\%opt & \\%solved \\\\ \n";
  std:: cout << " \\hline \n";


  for (int j=0;j<5;++j){
    res_cut res_line;
    res_line.time_end=0.0;
    int nb=0;
    int nb_sol=0;
    for (uint i=0; i<res_vec.size();++i){
      if (j==0 && res_vec[i].size==10 ){
	nb++;
	if (res_vec[i].time_end>0.0 ){
	  ++nb_sol;
	  res_line.size+=res_vec[i].size;
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.dev+=res_vec[i].dev;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.opt+=res_vec[i].opt;
	  res_line.solved+=res_vec[i].solved;
	  res_line.obj_end+=res_vec[i].obj_end;
	}
      }
      if (j==1 && res_vec[i].size==20){
	nb++;
	if (res_vec[i].time_end>0.0){
	  ++nb_sol;
	  res_line.size+=res_vec[i].size;
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.dev+=res_vec[i].dev;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.opt+=res_vec[i].opt;
	  res_line.solved+=res_vec[i].solved;
	  res_line.obj_end+=res_vec[i].obj_end;
	}
      }
      if (j==2 && res_vec[i].size==25){
	nb++;
	if (res_vec[i].time_end>0.0){
	  nb_sol++;
	  res_line.size+=res_vec[i].size;
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.dev+=res_vec[i].dev;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.opt+=res_vec[i].opt;
	  res_line.solved+=res_vec[i].solved;
	  res_line.obj_end+=res_vec[i].obj_end;
	}
      }

      if (j==3 && res_vec[i].size==30){
	nb++;
	if ( res_vec[i].time_end>0.0){
	  ++nb_sol;
	  res_line.size+=res_vec[i].size;
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.dev+=res_vec[i].dev;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.opt+=res_vec[i].opt;
	  res_line.solved+=res_vec[i].solved;
	  res_line.obj_end+=res_vec[i].obj_end;
	}
      }
      if (j==4 && res_vec[i].size==60){
	nb++;
	if ( res_vec[i].time_end>0.0){
	  ++nb_sol;
	  res_line.size+=res_vec[i].size;
	  res_line.time_first_sol+=res_vec[i].time_first_sol;
	  res_line.dev+=res_vec[i].dev;
	  res_line.time_end+= res_vec[i].time_end;
	  res_line.opt+=res_vec[i].opt;
	  res_line.solved+=res_vec[i].solved;
	  res_line.obj_end+=res_vec[i].obj_end;
	}
      }
    }
    if (nb_sol!=0){
      res_line.size=res_line.size/nb_sol;
      res_line.time_first_sol=res_line.time_first_sol/nb_sol;
      res_line.dev=res_line.dev/nb_sol;
      res_line.time_end= (res_line.time_end+(nb-nb_sol)*100.0)/nb;
      res_line.opt=res_line.opt/nb;
      res_line.solved=res_line.solved/nb;
      res_line.obj_end=res_line.obj_end/nb_sol;
      if (nb_sol==0 && j==0) res_line.size=10;
      if (nb_sol==0 && j==1) res_line.size=20;
      if (nb_sol==0 && j==2) res_line.size=25;
      if (nb_sol==0 && j==3) res_line.size=30;
      if (nb_sol==0 && j==4) res_line.size=60;
    
      std:: cout << setprecision(2) << "$" <<
	res_line.size << " $&$ " <<
	res_line.time_first_sol << " $&$ " <<
	res_line.dev <<" $&$ " << 
	res_line.time_end  << " $&$ " <<
	100*(res_line.obj_end) << "$ &$ " <<
	100*res_line.opt<< " $&$ " <<
	100*res_line.solved << "$ \\\\ \n";
    
    }
  }
    std::cout << "\\hline \n";
    std::cout << "\\end{tabular}\n";
    std::cout << "\\end{center}\n";

    res_file.close();


  return 0;
}
