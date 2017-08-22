#include "model_mip_simpl.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

 
int solveSimple(const LotProblem& LP, Solution &s){
  try{
    int n=LP.nbLot;
    int Q=LP.nbSublot;
    IloNum start,time_exec;

    IloEnv env;
    IloModel model(env);
    IloNumVarMatrix t(env,n*Q+1);
    IloNumVar4DMatrix x(env,n);
    IloNumVar4DMatrix p(env,n);
    IloNumVarMatrix BM(env,n*Q+1);
    createModel(env,model,LP,x,p,BM,t);
    IloCplex cplex(model);
    
    initMIP(LP,env,model,cplex,t,x,p,BM);
    cplex.setParam(IloCplex::EpGap, 0.01);
    cplex.setParam(IloCplex::TiLim, time_limit_simpl);
    //cplex.setOut(env.getNullStream());
    cplex.setParam(IloCplex::Threads, 1);
    start=cplex.getCplexTime();
    bool solfind=cplex.solve();
    time_exec=cplex.getCplexTime()-start;
    std::cout << "status model simple: "<< cplex.getStatus() << 
      " en " << time_exec<< std::endl;
    if (solfind){
      std::cout << "obj model simple (conso)=" << 
	cplex.getObjValue() << std::endl;
      fillSolution(LP,s,cplex,x,p,BM,t);   
      //s.printSol(LP);
      //afficheSol(LP,env,cplex,BM,x,p,t);
    }
    env.end();
    return 0;
  }
  catch (IloException& e) {
    std::cout <<e << "\n";
    e.end();
    return 1;
  }
}

int printSol(const LotProblem& LP,IloEnv& env,  const IloCplex& cplex, 
	       const IloNumVarMatrix& BM, const IloNumVar4DMatrix& x,
	       const IloNumVar4DMatrix& p, const IloNumVarMatrix& t){
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  IloNumArray v(env);
  for (int i=0;i<n;++i) {
    for (int q=0;q<Q;q++) {
      for (int e=0;e<n*Q+1;++e) {
	for (int k=0;k<LP.nbLine;++k){
	  if (cplex.getValue(x[i][q][e][k])==1){
	    std::cout << "x["<<i<<"]["<<q<<"]["<<e<<"]["<<k<<"]="
		      <<cplex.getValue(x[i][q][e][k])<<std::endl; 
	    std::cout << "p["<<i<<"]["<<q<<"]["<<e<<"]["<<k<<"]="
		      <<cplex.getValue(p[i][q][e][k])<<std::endl;
	  }
	}
      }
    }
  }
  for (int e=0;e<n*Q+1;++e) {
    cplex.getValues(t[e],v);
    std::cout << "t["<<e<<"]="<<v<<std::endl;
    for (int k=0;k<LP.nbLine;++k){
      if (cplex.getValue(BM[e][k])==1)
	std::cout << "BM["<<e<<"]["<<k<<"]="
		  <<cplex.getValue(BM[e][k])<<std::endl;
    }
  }
  return 0;
}

int createModel(IloEnv& env,IloModel & m,const  LotProblem &LP,IloNumVar4DMatrix &x, IloNumVar4DMatrix &p,IloNumVarMatrix &BM,IloNumVarMatrix &t)
{
  return createVar(env,LP,x,p,BM,t) || 
    createConstraint(env,m,LP,x,p,BM,t);
}

int createVar(IloEnv& env,const  LotProblem &LP,IloNumVar4DMatrix &x, IloNumVar4DMatrix &p,IloNumVarMatrix &BM,IloNumVarMatrix &t)
{
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  int m=LP.nbLine;

  int i,q,e,k;
  for (i=0;i<n;++i) {
    x[i]=IloNumVar3DMatrix(env,Q);
    p[i]=IloNumVar3DMatrix(env,Q);
    for (q=0;q<Q;++q) {
      x[i][q]=IloNumVarMatrix(env,n*Q+1);
      p[i][q]=IloNumVarMatrix(env,n*Q+1);
      for (e=0;e<n*Q+1;++e) {
	x[i][q][e]=IloNumVarArray(env,m,0,1,ILOINT);
	p[i][q][e]=IloNumVarArray(env,m,0,LP.demand[i],ILOFLOAT);
      }
    }
  }
  for (e=0;e<n*Q+1;++e) {
    //    t[i]=IloNumVarArray(env,m,0,LP.horizon,ILOFLOAT);
    t[e]=IloNumVarArray(env,m,0,IloInfinity,ILOFLOAT);
    BM[e]=IloNumVarArray(env,m,0,1,ILOINT);
  }

  return 0;
}

int findSolTrivial(const LotProblem& LP,std::vector<double> loadLine,
		   std::vector<int>& affLine){
  const int m=LP.nbLine;
  const int n=LP.nbLot;
  int i,k;

  for (i=0;i<n;++i){
    if (LP.demand[i]>0){
      double minLoad=IloInfinity;
      int leastLoadedLine=-1;
      for (k=0;k<m;++k){
	uint l=0;
	while (l<LP.subset[i].size() && k!=LP.subset[i][l])
	  ++l;
	if (l==LP.subset[i].size() && loadLine[k]<minLoad){
	  minLoad=loadLine[k];
	  leastLoadedLine=k;
	}
      }
      affLine[i]=leastLoadedLine;
      loadLine[leastLoadedLine]+=LP.demand[i]/LP.speed[i][leastLoadedLine];
    }
  }
  return 0;
}
 
int initMIP(const LotProblem& LP, IloEnv& env, IloModel& model,IloCplex& cplex, const IloNumVarMatrix& t,const IloNumVar4DMatrix& x, const IloNumVar4DMatrix& p,const IloNumVarMatrix& BM){
  srand(time(NULL));  
  const int m=LP.nbLine;
  const int n=LP.nbLot;
  const int Q=LP.nbSublot;
  int i,k;
  std::vector<double> loadLine(m,0.0);
  std::vector<int> affLine(n,-1);
  findSolTrivial(LP,loadLine,affLine);
  //MIP start
  IloNumVarArray startVar(env);
  IloNumArray startVal(env);
  int evt=0;

  for (k=0;k<m;++k){
    bool affMaint=0;
    evt=0;
    for (i=0;i<n;++i){
      if (affLine[i]==k){
	if (!affMaint){
	  int affTask=rand()%2;
	  if ((affTask == 1 && LP.durMaint[k] + loadLine[k] + LP.demand[i]/LP.speed[i][k] > LP.horizon) || affTask==0) {
	    startVar.add(BM[evt][k]);
	    startVal.add(1); 
	    ++evt;
	    affMaint=1;
	  }
	  startVar.add(p[i][0][evt][k]);
	  startVal.add(LP.demand[i]);
	  startVar.add(x[i][0][evt][k]);
	  startVal.add(1);
	  loadLine[k]+=LP.demand[i]/LP.speed[i][k];
	  ++evt;
	}
	else {
	  startVar.add(p[i][0][evt][k]);
	  startVal.add(LP.demand[i]);
	  startVar.add(x[i][0][evt][k]);
	  startVal.add(1);	    
	  loadLine[k]+=LP.demand[i]/LP.speed[i][k];
	  ++evt;
	}
      }
      for (int q=1;q<LP.nbSublot;++q) {
	for (int e=0;e<LP.nbSublot+1;++e){
	  startVar.add(x[i][q][e][k]);
	  startVal.add(0);	    
	}
      }
    }   
  }
  
  cplex.addMIPStart(startVar,startVal);
  startVar.end();
  startVal.end();
  return 0;
}

int createConstraint(IloEnv& env,IloModel & model, const LotProblem &LP, const IloNumVar4DMatrix &x, const IloNumVar4DMatrix &p, const IloNumVarMatrix &BM, const IloNumVarMatrix &t)
{
  uint n=LP.nbLot;
  uint Q=LP.nbSublot;
  uint m=LP.nbLine;

  uint i,e,q,j,k;


  std::vector <std::vector<int>> unsubset;
  unsubset.resize(n);
  for (i=0;i<n;++i){
    k=0; j=0;
    while (k<m && j<LP.subset[i].size()) {
      if (LP.subset[i][j]!=k)
	unsubset[i].push_back(k);
      else
	j++;   
      k++;
    }
    while (k<m) {
      unsubset[i].push_back(k);
      k++;
    }
  }


  IloNumVarArray Tard(env,m,0,IloInfinity);
  IloExpr expr(env);
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      for (k=0;k<unsubset[i].size();++k) {
	for (e=0;e<n*Q+1;++e)
	  expr+=LP.delta*p[i][q][e][unsubset[i][k]]
	    /LP.speed[i][unsubset[i][k]];
      }
    }
  }
  model.add(IloMinimize(env,100000*IloSum(Tard)+expr));
  expr.end();

  //ordonne event
  for (k=0;k<m;++k){
    for (e=0;e<n*Q;e++)
      model.add(t[e][k]<=t[e+1][k]); 
  }

  //souslot ordonnancer une fois max (Q nb max de sous lot mais il peut y en avoir moins)
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q)	{
      IloExpr expr1(env);
      for (k=0;k<unsubset[i].size();++k){
	for (e=0;e<n*Q+1;e++)
	  expr1+=x[i][q][e][unsubset[i][k]];
      }
      model.add(expr1 <= 1);
      expr1.end();
    }
  }
  //continuité des 1!
  for (k=0;k<m;++k){
    for (e=0;e<n*Q;e++){      
      IloExpr expr1(env);    
      IloExpr expr(env);
      for (i=0;i<n;++i) {
	for (q=0;q<Q;++q) {
	  expr+=x[i][q][e+1][k];
	  expr1+=x[i][q][e][k];
	}
      }
      expr1+=BM[e][k];
      expr+=BM[e+1][k];
      model.add(expr1 >= expr);
      expr1.end();
      expr.end();
    }
  }

  //une position correspond a un seul batch 
  for (k=0;k<m;++k){
    for (e=0;e<n*Q+1;e++){      
      IloExpr expr1(env);
      for (i=0;i<n;++i) {
	for (q=0;q<Q;++q)
	  expr1+=x[i][q][e][k];
      }
      expr1+=BM[e][k];
      model.add(expr1 <= 1);
      expr1.end();
    }
  }        

  //subset
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q)	{
      IloExpr expr1(env);
      for (k=0;k<LP.subset[i].size();++k){
	for (e=0;e<n*Q+1;e++)
	  expr1+=p[i][q][e][LP.subset[i][k]]+x[i][q][e][LP.subset[i][k]];
      }
      model.add(expr1 <= 0);
      expr1.end();
    }
  }    
 
 
  //un sous lot doit etre affecté pour produire
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) { 
      for (k=0;k<unsubset[i].size();++k){
	for (e=0;e<n*Q+1;++e)
	  model.add(LP.demand[i]*x[i][q][e][unsubset[i][k]]>=p[i][q][e][unsubset[i][k]]);
      }
    }
  }
    
  //minimum size batch
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) { 
      for (k=0;k<unsubset[i].size();++k){
	for (e=0;e<n*Q+1;++e)
	  model.add(minSize*x[i][q][e][unsubset[i][k]]<=p[i][q][e][unsubset[i][k]]);
      }
    }
  }
 
  //la demande de chaque lot est respectée
  for (i=0;i<n;++i){
    IloExpr expr(env);
    for (q=0;q<Q;++q) {
      for (e=0;e<Q*n+1;++e) {
	for (k=0;k<unsubset[i].size();++k)
	  expr+=p[i][q][e][unsubset[i][k]];
      }
    }
    model.add(expr==LP.demand[i]);
    expr.end();
  }

  //DEADLINE GENERALE DU PROJET
  /*for (k=0;k<m;++k){
    IloExpr expr(env);
    for (i=0;i<n;++i) {
      if (LP.speed[i][k]!=0){
	for (q=0;q<Q;++q) 
	  expr+= p[i][q][n*Q][k]/LP.speed[i][k];
      }
    }
    model.add(t[n*Q][k] + expr <=LP.horizon);
    expr.end();
  }*/
  
  //Tardiness
  for (k=0;k<m;++k) {
    IloExpr expr(env);
    for (i=0;i<n;++i) {
      if (LP.speed[i][k]!=0){
	for (q=0;q<Q;++q) 
	  expr+= p[i][q][n*Q][k]/LP.speed[i][k];
      }
    }
    model.add(Tard[k]>=t[n*Q][k] + expr - LP.horizon);
    model.add(Tard[k]>=t[n*Q][k]+BM[n*Q][k]*LP.durMaint[k] - LP.horizon); 
    expr.end();
  }
  
  //setup + durée
  for (e=0;e<n*Q;++e) {
    for (i=0;i<n;++i) {
      for (j=0;j<n;++j) {
	for (k=0;k<m;++k) {
	  if (LP.speed[i][k]!=0) {
	    IloExpr expr(env);
	    IloExpr expr2(env);
	    for (q=0;q<Q;++q) {
	      expr+= p[i][q][e][k];
	      expr2+= x[i][q][e][k]+x[j][q][e+1][k];
	    }
	    model.add(t[e+1][k] >= t[e][k] + expr/LP.speed[i][k]+LP.setup[i][j]*(expr2-1));
	    expr.end();
	    expr2.end();
	  }
	}
      }
    }
  }
  
  //OPERATION DE MAINTENANCE
  //si il y a une maintenance à faire sur la machine k alors un et un seul début
  for (k=0;k<m;++k){
    IloExpr expr(env);
    for (e=0;e<n*Q+1;++e){
      expr+=BM[e][k];
    }
    if (LP.durMaint[k]!=0)
      model.add(expr==1);
    else 
      model.add(expr==0);
    expr.end();
  }
    
  //A METTRE SI MAINTENANCE DOIT ETRE DANS HORIZON DE TEMPS!!!!!!!!!!!!!!!!!!!
  //l'operation doit etre avant son "start max"
  for (e=0;e<n*Q+1;++e) {
    for (k=0;k<m;++k)
      model.add(t[e][k]<=LP.horizon*(2-BM[e][k])-LP.durMaint[k]*BM[e][k]);
    //      model.add(t[e][k]<=2*LP.horizon-LP.durMaint[k]*BM[e][k]);
  }
    
  //si on commence une operation de maintenance sur la machine k, 
  //aucune tache ne peut commencer sur k avant la fin de cette operation
  //et aucune tache ne peut etre pendant la maintenance

   for (e=0;e<n*Q;++e) {
    for (k=0;k<m;++k){ 
      IloExpr expr(env);
      for (i=0;i<n;++i) {
	for (q=0;q<Q;++q) 
	  expr+=x[i][q][e+1][k];
      }
      model.add(t[e+1][k] >= t[e][k] + 
		LP.durMaint[k]*(BM[e][k]+expr-1));
      expr.end();
    }
  }
	   
  
  for (e=0;e<n*Q;++e) {
    for (k=0;k<m;++k){ 
      IloExpr expr(env);
      IloExpr expr1(env);
      for (i=0;i<n;++i) {
	if (LP.speed[i][k]!=0)
	  for (q=0;q<Q;++q) {
	    expr+=x[i][q][e][k];
	    expr1+=p[i][q][e][k]/LP.speed[i][k];
	  }
      }
      model.add(t[e+1][k] >= t[e][k] + expr1 -
		4*LP.horizon*(2-BM[e+1][k]-expr));
      expr.end();
      expr1.end();
    }
  }
  
    
  return 0;
}


int fillSolution(const LotProblem &LP, Solution &s,const IloCplex & cplex,const IloNumVar4DMatrix &x, const IloNumVar4DMatrix &p,const IloNumVarMatrix &BM,const IloNumVarMatrix &t){

  int i,q,k,e;
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  int m=LP.nbLine;
  for (i=0;i<n;++i){
    for (q=0;q<Q;++q){
      for (e=0;e<n*Q+1;++e){
	for (k=0;k<m;++k) {
	  if (cplex.getValue(x[i][q][e][k])-1<=POSITIVE_ZERO &&
 	      cplex.getValue(x[i][q][e][k])-1>=NEGATIVE_ZERO) {
	    s.st[i][q]=cplex.getValue(t[e][k]);
	    s.a[i][q]=k;
	    s.ft[i][q]=cplex.getValue(t[e][k])+
	      cplex.getValue(p[i][q][e][k])/LP.speed[i][k];
	    
	  }
	}
      }
    }
  } 
  
  for (k=0;k<m;++k){
    for (e=0;e<n*Q+1;++e){
      if (cplex.getValue(BM[e][k])-1<=POSITIVE_ZERO && 
	  cplex.getValue(BM[e][k])-1>=NEGATIVE_ZERO) 
	s.dm[k]=cplex.getValue(t[e][k]);
    }
  }

  return 0;  
} 

/*int fillSolutionBis(const LotProblem &LP, Solution &s,const IloCplex & cplex,const IloNumVar4DMatrix &x, const IloNumVar4DMatrix &p,const IloNumVarMatrix &BM,const IloNumVarMatrix &t){

  int i,q,k,e;
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  int m=LP.nbLine;
  for (i=0;i<n;++i){
    for (q=0;q<Q;++q){
      for (e=0;e<n*Q+1;++e){
	for (k=0;k<m;++k) {
	  if (cplex.getValue(x[i][q][e][k])-1<=POSITIVE_ZERO &&
 	      cplex.getValue(x[i][q][e][k])-1>=NEGATIVE_ZERO) {
	    s.st[i][q]=cplex.getValue(t[e][k]);
	    s.a[i][q]=k;
	    s.ft[i][q]=cplex.getValue(t[e][k])+
	      cplex.getValue(p[i][q][e][k])/LP.speed[i][k];
	    
	  }
	}
      }
    }
  } 
  
  for (k=0;k<m;++k){
    for (e=0;e<n*Q+1;++e){
      if (cplex.getValue(BM[e][k])-1<=POSITIVE_ZERO && 
	  cplex.getValue(BM[e][k])-1>=NEGATIVE_ZERO) 
	s.dm[k]=cplex.getValue(t[e][k]);
    }
  }

  for (k=0;k<m;++k){
    int min=LP.horizon;
    for (i=0;i<n;++i){
      for (q=0;q<Q;++q){
	if (s.a[i][q]==k && s.ft[i][q]<=s.dm[k] && 
	    min>s.dm[k]-s.ft[i][q]) 
	  min=s.dm[k]-s.ft[i][q];
      }
    }
    if (min!=LP.horizon)
      s.dm[k]=s.dm[k]-min;
    else 
      s.dm[k]=0;
  }
  return 0;  
} */
