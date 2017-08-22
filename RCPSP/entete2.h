#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <time.h>
#include <ilcplex/ilocplex.h>

//---------------------------------------------------------------------------------------------------

#define infini 100000
#define NbTaskMax 300
#define NbResoucresMax 20
//---------------------------------------------------------------------------------------------------
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ios;

//---------------------------------------------------------------------------------------------------

typedef IloArray<IloIntArray> IloIntMatrix;
typedef IloArray<IloNumArray> IloNumMatrix;
typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;

typedef IloArray<IloNumVarArray> IloNumVarMatrix;
typedef IloArray<IloIntVarArray> IloIntVarMatrix;

typedef IloArray<IloNumVarMatrix> IloNumVarArray3;
typedef IloArray<IloIntVarMatrix> IloIntVarArray3;

string  NFichier, NFichier0, FicVerif, NFichierT, ResultFile;
char NomFichier[75], FichierVerif[75], NomFichierT[75];

IloIntArray B, NbSucc, p, ES, LS;
IloIntMatrix r, Succ;
IloNumVar Cmax;
IloEnv env;
IloNum start;
IloNum finish;
IloInt  N, n, K,H; //M, i & j taches, N Nb tache, e event, n nb event, k ressource, K nb ressource
bool FictifTask = true;
bool Dbug = false;
//---------------------------------------------------------------------------------------------------

void define_data(IloEnv env) {
   
   cout<<"  + Initialization data   --   start"<<endl;
   int i,j,k;
   strncpy(NomFichier, NFichier.c_str(), 100 );
   ifstream LectureCommentaire(NomFichier,ios::in);
   if(!LectureCommentaire){
      cerr<<"        !!!!! OPENING OF FILE ("<<NFichier<<") IMPOSSIBLE"<<endl;
      exit(1);
   };
   cout<<"        Fichier de donnees : "<<NFichier<<" Ouvert"<<endl;
   LectureCommentaire>>N;//lit nbtask
   if (FictifTask == false) N=N-2;
   LectureCommentaire>>K; //lit nbresource
   //n = (int)(N*(1.0));
   p = IloIntArray(env,N, 0, infini); //duration
   B = IloIntArray(env,K, 0, infini); //capacity
   r = IloIntMatrix(env,N); //demand
   NbSucc = IloIntArray(env,N, 0, N-1); //nbsucc for each task
   Succ = IloIntMatrix(env,N); // succ
   Cmax = IloNumVar(env, 0, infini); 
   for (i=0;i<N;i++) r[i] = IloIntArray(env,K, 0, infini);
   for(k=0;k<K;k++)  LectureCommentaire>>B[k]; //lit capacity
   if (FictifTask == false){//skipline
      int step,NbSucc;
      LectureCommentaire>>step;
      for(k=0;k<K;k++) LectureCommentaire>>step;
      LectureCommentaire>>NbSucc;
      for(j=0;j<NbSucc;j++) LectureCommentaire>>step;
   };
   for(i=0;i<N;i++){
       LectureCommentaire>>p[i];//lit duration
       for(k=0;k<K;k++)  LectureCommentaire>>r[i][k]; 
       LectureCommentaire>>NbSucc[i];
       Succ[i] = IloIntArray(env, N, 0, infini);
       for(j=0;j<NbSucc[i];j++){
             LectureCommentaire>>Succ[i][j];
             Succ[i][j] = Succ[i][j]-1; 
             if (FictifTask == false) Succ[i][j] = Succ[i][j]-1;
       };
   };
   for(i=0;i<N;i++) H = H + p[i]; //M = H;
   cout <<"        #Task = "<<N<<", #Resources = "<<K<<", Time horizon = "<<H<<endl;
   LectureCommentaire.close();
   cout <<"     Initialization data   --   end"<< endl;
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void Affiche()
{
		cout<<"  + Data Display"<<endl;
                int i=0, j=0,k=0;
                cout<<"		i"<<'\t'<<"p[i]"<<'\t';
                for(k=0;k<K;k++) cout<<"ri"<<k<<'\t';
		cout<<"Nbprec"<<'\t'<<"	Succ"<<endl;
		for(i=0;i<N;i++) {
				cout<<"		"<<i<<'\t'<<p[i]<<'\t';
				for(k=0;k<K;k++) cout<<r[i][k]<<'\t';
				cout<<NbSucc[i]<<'\t';
				for(j=0;j<NbSucc[i];j++) cout<<'\t'<<Succ[i][j];
				cout<<endl;
	        };
		cout<<"		B["<<i<<"] = "<<'\t';
                for(i=0;i<K;i++) cout<<B[i]<<'\t';
                cout<<endl;
}



//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
///*
void TimeWindows() {
        int i, NbTask, Blabla;;
        cout<<"  + Time window file : "<<NFichierT<<endl;
        strncpy( NomFichierT, NFichierT.c_str(), 75 );
        ifstream File(NomFichierT);
        if (!File){
                cerr << "   !!!Error opening file for reading Time window file ..." << endl;
                exit(1);
        };
        i=0;
        File>>NbTask;
        ES = IloIntArray(env,NbTask, 0, infini);
        LS = IloIntArray(env,NbTask, 0, infini);
        if (FictifTask == false){
           NbTask = NbTask-2;
	   File>>Blabla>>Blabla>>Blabla;
	};
        while (!File.eof() ){
             if(i<NbTask){
                File>>Blabla>>ES[i]>>LS[i];
                i=i+1;
             } 
             else if(i=NbTask)File>>Blabla>>ES[NbTask]>>LS[NbTask];
        };
        //cout<<"		i	ESi	LSi"<<endl;
        //for(i=0;i<NbTask;i++) cout<<"		"<<i<<'\t'<<ES[i]<<'\t'<<LS[i]<<endl;
        cout<<"     Reading Time window file  --  end"<<endl;
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


void ShowTimeWindows() {
     int i;
     cout<<"		i	ESi	LSi"<<endl;
     for(i=0;i<N;i++) cout<<"		"<<i<<'\t'<<ES[i]<<'\t'<<LS[i]<<endl;
}

// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*
int Start_time[150], Start_Event[150], Tb_Event[150], Nb_Event;
void ReadFileEvt(string  NFichier){
   string  NFichierTemp;
   int i, j, temp, rang, size, NB;
   char  SolFile[100]; 
   bool AddItem;
   //NFichierTemp = "./timewindows/j30/" + NFichier0 + ".sol";
   //NFichierTemp = "./instances/j30Rcp/" + NFichier0 +".rcp.sol"; NFichier
   //NFichierTemp = NFichier +".sol"; 
   //NFichierTemp = "./instances/j30RcpModif/jA15_B25/" + NFichier0 + ".rcp.sol";
   //NFichierTemp = "./instances/pack-50/" + NFichier0 + ".rcp.sol";
   //NFichierTemp = "./instances/j20-pack/" + NFichier0 + ".rcp.sol";
   NFichierTemp = "./instances/bl/" + NFichier0 + ".rcp.sol";
   strncpy(SolFile, NFichierTemp.c_str(), 100 );
   ifstream readFile(SolFile,ios::in);
      if(!readFile){
         cerr<<"        !!!!! OPENING OF FILE ("<<SolFile<<") IMPOSSIBLE"<<endl;
         exit(1);
      };
   readFile>>NB;
   int Size = 0;
   for(i=0;i<NB;i++){
      AddItem=true;
      readFile>>temp>>Start_time[i];
      for(j=0;j<Size; j++)
         if(Start_time[i] == Tb_Event[j]){
            AddItem = false; 
            rang = j;
          };
      if(AddItem==true){
         Tb_Event[Size]= Start_time[i];
         Start_Event[i]= Size;
         Size = Size+1;
      };
      if (AddItem==false) Start_Event[i]= rang;
    };
    Nb_Event = Size;
    readFile.close();
    //if(Dbug==true)
       cout<<" #Event = "<<Nb_Event<<endl;
}
//*/
