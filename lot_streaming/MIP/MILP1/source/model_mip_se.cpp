#include <algorithm>  
#include <vector>
#include "model_mip_se.h"

int solveReal(LotProblem &LP,Solution &s)
{
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  //int T=LP.nbHP;
  int m=LP.nbLine;
  IloNum start,time_exec;

  IloEnv env;
  IloModel model(env);

  //date evenement
  IloNumVarArray t(env,2*n*Q,0,LP.horizon,ILOFLOAT);
  //X[i][q][e]=1 si t[e] est la date de debut du sous lot q du lot i
  IloNumVar3DMatrix X(env,n);
  //Y[i][q][e]=1 si t[e] est la date de fin du sous lot q du lot i
  IloNumVar3DMatrix Y(env,n);
  //d[i][q][e][k]= X[i][q][e]*a[i][q][k]
  IloNumVar4DMatrix d(env,n);
  //f[i][q][e][k]= Y[i][q][e]*a[i][q][k]
  IloNumVar4DMatrix f(env,n);
  // P[e][t] =1 si l'evenement [t[e],t[e+1]] intersecte l'Hp t
  IloNumVarMatrix P(env,2*n*Q);
  // AD[e][t]=1 si l'evenement e+1 est apres le debut de l'HP t 
  IloNumVarMatrix AD(env,2*n*Q);
  //BF[e][t]=1 si e est avant la fin de l'HP t
  IloNumVarMatrix BF(env,2*n*Q);
  // production du sous lot q de i
  IloNumVarMatrix p(env,n);
  // O[e][k]=1 si la machine k est "on" a l'evenement e
  IloNumVarMatrix O(env,2*n*Q);
  //a[i][q][k]=1 si le sous lot q du lot i est sur la machine k
  IloNumVar3DMatrix a(env,n);
  //pic de conso en HP
  IloNumVar Pmax(env,0,m*LP.delta,ILOFLOAT);
  // BM[e][k]=1 si l'evenement e est le debut de la maintenance sur k
  IloNumVarMatrix BM(env,2*n*Q);
  IloNumVarMatrix W(env,n);

  
  createModel(env,model,LP,X,Y,d,f,a,p,P,AD,BF,O,BM,t,Pmax,W);

  IloCplex cplex(model);
  
  cplex.setParam(IloCplex::EpInt, 0.0);
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads, 1);
  //cplex.setOut(env.getNullStream());
  start= cplex.getCplexTime();

  if (cplex.solve())
    {
      time_exec=cplex.getCplexTime()-start;
      std::cout << "status: "<< cplex.getStatus() <<   " en " << time_exec <<  std::endl;
      //afficher solution
      std::cout << "obj model real (conso+peak)=" << cplex.getObjValue() << std::endl;
      modelToSolution(LP,s,cplex,t,X,Y,a,BM);
 
      env.end();
      return 0;
    }
  else
    {
      env.end();
      std::cout << "probleme" << std::endl;
      return 1;
    }
}

int solveRealSimple(LotProblem &LP,Solution &s,Solution &s2)
{
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  int m=LP.nbLine;
  IloNum start,time_exec;

  IloEnv env;
  IloModel model(env);

  IloNumVarArray t(env,2*n*Q,0,LP.horizon,ILOFLOAT);
  IloNumVar3DMatrix X(env,n);
  IloNumVar3DMatrix Y(env,n);
  IloNumVar4DMatrix d(env,n);
  IloNumVar4DMatrix f(env,n);
  IloNumVarMatrix P(env,2*n*Q);
  IloNumVarMatrix AD(env,2*n*Q);
  IloNumVarMatrix BF(env,2*n*Q);
  IloNumVarMatrix p(env,n);
  IloNumVarMatrix O(env,2*n*Q);
  IloNumVar3DMatrix a(env,n);
  IloNumVar Pmax(env,0,m*LP.delta,ILOFLOAT);
  IloNumVarMatrix BM(env,2*n*Q);
  IloNumVarMatrix W(env,n);
  std::cout << "loading model" << std::endl;
  createModel(env,model,LP,X,Y,d,f,a,p,P,AD,BF,O,BM,t,Pmax,W);
  if (s.isValid(LP))
    solutionToModel(env,model,LP,s,X,Y,BM);
  IloCplex cplex(model);
  
  cplex.setParam(IloCplex::EpInt, 0.0);
  cplex.setParam(IloCplex::TiLim, time_limit);
  cplex.setParam(IloCplex::Threads, 1);
  //cplex.exportModel("model.lp");
  //cplex.setOut(env.getNullStream());
  start= cplex.getCplexTime();

  if (cplex.solve())
    {
      time_exec=cplex.getCplexTime()-start;
      std::cout << "status: "<< cplex.getStatus() << " en " << time_exec<<  std::endl;
      //afficher solution
      std::cout << "obj model real (conso+peak)=" << cplex.getObjValue() << std::endl;
      modelToSolution(LP,s2,cplex,t,X,Y,a,BM);
      /*IloNumArray v(env);
      cplex.getValues(t,v);
      std::cout << "t =" << v << std::endl;

      for (int i=0;i<n;++i)
	{
	  for (int q=0;q<Q;q++)
	    {
	      cplex.getValues(X[i][q],v);
	      std::cout << "X["<<i<<"]["<<q<<"]="<<v<<std::endl;
	      cplex.getValues(Y[i][q],v);
	      std::cout << "Y["<<i<<"]["<<q<<"]="<<v<<std::endl;
	      cplex.getValues(a[i][q],v);
	      std::cout << "a["<<i<<"]["<<q<<"]="<<v<<std::endl;
	    }
	  cplex.getValues(p[i],v);
	  std::cout << "p["<<i<<"]="<<v<<std::endl;
	}
      for (int e=0;e<2*n*Q;++e)
	{
	  cplex.getValues(O[e],v);
	  std::cout << "O["<<e<<"]="<<v<<std::endl;
	  //cplex.getValues(P[e],v);
	  //std::cout << "P["<<e<<"]="<<v<<std::endl;
	  cplex.getValues(BM[e],v);
	  std::cout << "BM["<<e<<"]="<<v<<std::endl;
	}*/
      env.end();
      return 0;
    }
  else
    {
      env.end();
      std::cout << "probleme" << std::endl;
      return 1;
    }
}

 int createModel(IloEnv& env,IloModel & m, LotProblem &LP,IloNumVar3DMatrix &X, IloNumVar3DMatrix &Y,IloNumVar4DMatrix &d, IloNumVar4DMatrix &f,IloNumVar3DMatrix &a, IloNumVarMatrix &p, IloNumVarMatrix &P, IloNumVarMatrix &AD,  IloNumVarMatrix &BF,IloNumVarMatrix &O, IloNumVarMatrix &BM,IloNumVarArray &t,IloNumVar &Pmax,IloNumVarMatrix &W) {
  return createVar(env,LP,X,Y,d,f,a,p,P,AD,BF,O,BM,W) || 
    createConstraint(env,m,LP,X,Y,d,f,a,p,P,AD,BF,O,BM,t,Pmax,W);
}

int createVar(IloEnv& env,LotProblem &LP,IloNumVar3DMatrix &X, IloNumVar3DMatrix &Y, IloNumVar4DMatrix &d, IloNumVar4DMatrix &f,IloNumVar3DMatrix &a, IloNumVarMatrix &p, IloNumVarMatrix &P, IloNumVarMatrix &AD,  IloNumVarMatrix &BF, IloNumVarMatrix &O, IloNumVarMatrix &BM,IloNumVarMatrix &W)
{
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  int T=LP.nbHP;
  int m=LP.nbLine;

  int i,q,e,k;
  for (i=0;i<n;++i) {
    X[i]=IloNumVarMatrix(env,Q);
    Y[i]=IloNumVarMatrix(env,Q);
    d[i]=IloNumVar3DMatrix(env,Q);
    f[i]=IloNumVar3DMatrix(env,Q);
    a[i]=IloNumVarMatrix(env,Q);
    p[i]=IloNumVarArray(env,Q,0,LP.demand[i],ILOFLOAT);
    W[i]=IloNumVarArray(env,Q,0,LP.demand[i]*LP.delta,ILOFLOAT);
    for (q=0;q<Q;++q) {
      d[i][q]=IloNumVarMatrix(env,2*n*Q);
      f[i][q]=IloNumVarMatrix(env,2*n*Q);
      a[i][q]=IloNumVarArray(env,m,0,1,ILOINT);      
      X[i][q]=IloNumVarArray(env,2*n*Q,0,1,ILOINT);
      Y[i][q]=IloNumVarArray(env,2*n*Q,0,1,ILOINT);
      for (e=0;e<2*n*Q;++e) {
	d[i][q][e]=IloNumVarArray(env,m,0,1,ILOFLOAT);
	f[i][q][e]=IloNumVarArray(env,m,0,1,ILOFLOAT);
      }
    }
  }
  for (i=0;i<2*n*Q;++i) {
    P[i]=IloNumVarArray(env,m,0,1,ILOFLOAT);
    AD[i]=IloNumVarArray(env,T,0,1,ILOINT);
    BF[i]=IloNumVarArray(env,T,0,1,ILOINT);
    O[i]=IloNumVarArray(env,m,0,1,ILOFLOAT); 
    BM[i]=IloNumVarArray(env,m,0,1,ILOINT);
  }
  
  /*
  char namevar[24];
  for (i=0;i<n;i++){
    for (q=0;q<Q;q++) {
      for (e=0;e<2*n*Q;++e){
	sprintf(namevar,"X_%d_%d_%d",i,q,e);
	X[i][q][e].setName(namevar);
	sprintf(namevar,"Y_%d_%d_%d",i,q,e);
	Y[i][q][e].setName(namevar);
	for (k=0;k<m;++k){
	  sprintf(namevar,"d_%d_%d_%d_%d",i,q,e,k);
	d[i][q][e][k].setName(namevar);
	sprintf(namevar,"f_%d_%d_%d_%d",i,q,e,k);
	f[i][q][e][k].setName(namevar);
	}
      }
      sprintf(namevar,"p_%d_%d",i,q);
      p[i][q].setName(namevar);
      for (k=0;k<m;++k) {
	sprintf(namevar,"a_%d_%d_%d",i,q,k);
	a[i][q][k].setName(namevar);
      }
    }
  }

  for (e=0;e<2*n*Q;++e) {
    for (i=0;i<T;++i) {
      sprintf(namevar,"AD_%d_%d",e,i);
      AD[e][i].setName(namevar);
    }
  }

  for (e=0;e<2*n*Q;++e) {
    for (i=0;i<m;++i) {
      sprintf(namevar,"P_%d_%d",e,i);
      P[e][i].setName(namevar);
      sprintf(namevar,"O_%d_%d",e,i);
      O[e][i].setName(namevar);
      sprintf(namevar,"BM_%d_%d",e,i);
      BM[e][i].setName(namevar);
    }
    }*/

  return 0;
}

 int createConstraint(IloEnv& env, IloModel &model, LotProblem &LP,IloNumVar3DMatrix &X, IloNumVar3DMatrix &Y, IloNumVar4DMatrix &d, IloNumVar4DMatrix &f,IloNumVar3DMatrix &a, IloNumVarMatrix &p, IloNumVarMatrix &P, IloNumVarMatrix &AD,IloNumVarMatrix &BF,  IloNumVarMatrix &O, IloNumVarMatrix &BM,IloNumVarArray &t,IloNumVar &Pmax,IloNumVarMatrix &W)
{
  uint n=LP.nbLot;
  uint Q=LP.nbSublot;
  uint T=LP.nbHP;
  uint m=LP.nbLine;

  uint i,e,q,j,q2,e2,k,h;

  IloExpr expr(env);
  for (i=0;i<n;++i){
    for (q=0;q<Q;++q)
      expr+= W[i][q];
  }
  model.add(IloMinimize(env,ALPHA*expr+BETA*Pmax));
  expr.end();

  //ordonne event
  for (e=0;e<2*n*Q-1;e++)
    model.add(t[e]<=t[e+1]); 
 

  //souslot ordonnancer une fois max (Q nb max de sous lot mais il peut y en avoir moins)
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q)	{
      IloExpr expr1(env);
      IloExpr expr2(env);
      for (e=0;e<2*n*Q;++e) {
	expr1+=X[i][q][e];
	expr2+=Y[i][q][e];
      }
      model.add(expr1 == 1);
      model.add(expr2 == 1);
      expr1.end();
      expr2.end();
    }
  }    
  

  //un lot ne peut finir avant d'avoir commencer
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q){
      for (e=0;e<2*n*Q;++e) {
	IloExpr expr(env);
	for (e2=0;e2<=e;++e2)
	  expr+=X[i][q][e2]-Y[i][q][e2];
	model.add(expr >= 0);
	expr.end();
      }
    }
  }


  //un sous lot doit etre affecté pour produire
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {  
      IloExpr expr(env);
      for (k=0;k<m;++k)
	expr+=a[i][q][k];
      model.add(LP.demand[i]*expr>=p[i][q]);
      expr.end();
    }
  }
    
  //minimum size batch
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {  
      IloExpr expr(env);
      for (k=0;k<m;++k)
	expr+=a[i][q][k];
      model.add(p[i][q] >= minSize*expr);
      expr.end();
    }
  }

  //un sous lot doit etre affecté a une seule machine 
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      IloExpr expr(env);
      for (k=0;k<m;k++)
	expr+=a[i][q][k];
      model.add(expr<=1);
      expr.end();
    }
  }


  //linéarisation variable *
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      for (e=0;e<2*n*Q;++e) {
	for (k=0;k<m;++k) {
	  model.add(d[i][q][e][k] >= X[i][q][e] + a[i][q][k] -1);
	  model.add(d[i][q][e][k] <= X[i][q][e]);
	  model.add(d[i][q][e][k] <= a[i][q][k]);
	  model.add(f[i][q][e][k] >= Y[i][q][e] + a[i][q][k] -1);
	  model.add(f[i][q][e][k] <= Y[i][q][e]);
	  model.add(f[i][q][e][k] <= a[i][q][k]);
	}
      }
    }
  }
    
  //une machine n'est pas utilisé en meme temps pour deux sous lot
  for (e=1;e<2*n*Q;++e) {
    for (k=0;k<m;++k) {
      IloExpr expr(env);
      for (i=0;i<n;++i) {
	for (q=0;q<Q;++q)
	  expr+=d[i][q][e][k]-f[i][q][e][k];
      }
      model.add(O[e][k]==O[e-1][k]+expr);
      expr.end();
    }
  }

  for (k=0;k<m;++k) {
    IloExpr expr(env);
    for (i=0;i<n;++i) {
      for (q=0;q<Q;++q)
	expr+=d[i][q][0][k];
    }
    model.add(O[0][k]==expr);
    expr.end();
  }

   
    
  //DEADLINE GENERALE DU PROJET
  model.add(t[2*n*Q-1]<=LP.horizon);
    
  //setup
  for (i=0;i<n;++i) {
    for (j=0;j<n;++j) {
      for (q=0;q<Q;++q) {
	for (q2=0;q2<Q;q2++) {
	  for (e2=0;e2<2*n*Q;++e2) {
	    for (e=e2;e<2*n*Q;++e)  {
	      for (k=0;k<m;++k)
		model.add(t[e]>=t[e2]+
			  LP.setup[i][j]*(d[j][q][e][k]+f[i][q2][e2][k]-1));
	    }
	  }
	}
      }
    }
  }
    

  //machine autorisée
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      for (k=0;k<LP.subset[i].size();k++)
	model.add(a[i][q][LP.subset[i][k]]==0);
    }
  }
   


  //durée sous lot en fonction de la machine 
  for (i=0;i<n;++i)  {
    for (q=0;q<Q;++q)	 {
      for (k=0;k<m;++k)  {
	for (e=0;e<2*n*Q;++e)  {
	  for (e2=e;e2<2*n*Q;++e2)  {
	    if (LP.speed[i][k]!=0) 
	      model.add(t[e2]>=t[e]+p[i][q]*1/LP.speed[i][k]
			-M*(2-d[i][q][e][k]-f[i][q][e2][k]));
	  }
	}
      }
    }
  }

  for (i=0;i<n;++i)  {
    for (q=0;q<Q;++q)	 {
      for (k=0;k<m;++k)  {
	for (e=0;e<2*n*Q;++e)  {
	  for (e2=e;e2<2*n*Q;++e2)  {
	    if (LP.speed[i][k]!=0) 
	      model.add(t[e2]<=t[e]+p[i][q]*1/LP.speed[i][k]
			+M*(2-d[i][q][e][k]-f[i][q][e2][k]));
	  }
	}
      }
    }
  }
  
  //la demande de chaque lot est respectée
  for (i=0;i<n;++i){
    IloExpr expr(env);
    for (q=0;q<Q;++q)
      expr+=p[i][q];
    model.add(expr==LP.demand[i]);
    expr.end();
  }


  //contrainte l'evenement e est dans la periode d'heure pleine t
  //séparée en deux: e est avant la fin de l'heure pleine t 
  //                 et e est apres le debut de l'HP t

  for (e=0;e<2*n*Q-1;++e) {
    for (h=0;h<T;++h) {
      model.add(AD[e][h] >= (t[e+1] - LP.beginHP[h])/LP.horizon);
      model.add(LP.endHP[h]*BF[e][h] >= LP.endHP[h] - t[e]);
	
      for (k=0;k<m;++k)
	model.add(P[e][k] >= AD[e][h] + BF[e][h] + O[e][k] - 2);
    }
  }

   
  //pic de conso max
  for (e=0;e<2*n*Q;++e) {
    IloExpr expr(env);
    for (k=0;k<m;++k)
      expr+=P[e][k];
    expr*=LP.delta;
    model.add(Pmax>=expr);
    expr.end();
  }

  for (i=0;i<n;++i){
    for (q=0;q<Q;++q){
      for (k=0;k<m;++k){
	if (LP.speed[i][k]!=0)
	  model.add(W[i][q] >= LP.delta*p[i][q]/LP.speed[i][k]-M*(1-a[i][q][k]));
      }
    }
  }
  
   
  //OPERATION DE MAINTENANCE
  //si il y a une maintenance à faire sur la machine k alors un et un seul début
  for (k=0;k<m;++k){
    if (LP.durMaint[k]!=0){
      IloExpr expr(env);
      for (e=0;e<2*n*Q;++e){
	expr+=BM[e][k];
      }
      model.add(expr==1);
      expr.end();
    }
  }
    
  //l'operation doit etre avant son "start max"
  for (e=0;e<2*n*Q;++e)
    {
      for (k=0;k<m;++k)
	model.add(t[e]<=LP.horizon-LP.durMaint[k]*BM[e][k]);
    }
    
  //si on commence une operation de maintenance sur la machine k, 
  //aucune tache ne peut commencer sur k avant la fin de cette operation
  //et aucune tache ne peut etre pendant la maintenance
  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      for (e=0;e<2*n*Q;++e) {
	for (e2=e;e2<2*n*Q;++e2) {
	  for (k=0;k<m;++k)
	    model.add(t[e2] >= t[e] + 
		      LP.durMaint[k]*(BM[e][k]+d[i][q][e2][k]-1));
	}
      }
    }
  }

  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      for (e=0;e<2*n*Q;++e) {
	for (e2=e;e2<2*n*Q;++e2) {
	  for (k=0;k<m;++k){
	    IloExpr expr(env);
	    for (uint e3=0;e3<=e;++e3)
	      expr+=(d[i][q][e3][k]-f[i][q][e3][k]);
	    model.add(t[e2] != t[e] -
		      LP.horizon*(2-BM[e2][k]-expr));
	    expr.end();
	  }
	}
      }
    }
  }

 
    
    
    
  return 0;
}


int modelToSolution(LotProblem& LP,Solution &s,IloCplex& cplex,IloNumVarArray& t,IloNumVar3DMatrix& X,IloNumVar3DMatrix& Y, IloNumVar3DMatrix &a, IloNumVarMatrix &BM){
  int i,q,j,e;
  int affect=0;
  int n=LP.nbLot;
  int Q=LP.nbSublot;
  int m=LP.nbLine;

  for (i=0;i<n;++i){
    for (q=0;q<Q;++q){
      for (e=0;e<2*n*Q;++e){
	if (cplex.getValue(X[i][q][e])-1<=POSITIVE_ZERO && 
	    cplex.getValue(X[i][q][e])-1>=NEGATIVE_ZERO)
	  s.st[i][q]=cplex.getValue(t[e]);
	if (cplex.getValue(Y[i][q][e])-1<=POSITIVE_ZERO && 
	    cplex.getValue(Y[i][q][e])-1>=NEGATIVE_ZERO)
	  s.ft[i][q]=cplex.getValue(t[e]);
      }
      for (j=0;j<m;++j){
	if (cplex.getValue(a[i][q][j])-1<=POSITIVE_ZERO && 
	    cplex.getValue(a[i][q][j])-1>=NEGATIVE_ZERO) 
	  s.a[i][q]=j;
      }
    }
  }
  
  for (j=0;j<m;++j){
    for (e=0;e<2*n*Q;++e){
      if (cplex.getValue(BM[e][j])-1<=POSITIVE_ZERO && 
	  cplex.getValue(BM[e][j])-1>=NEGATIVE_ZERO)
	s.dm[j]=cplex.getValue(t[e]);

    }
  }
  return 0;  
} 

int solutionToModel(IloEnv& env,IloModel & model,LotProblem& LP,Solution& s,IloNumVar3DMatrix& X, IloNumVar3DMatrix& Y, IloNumVarMatrix& BM){
  std::vector <mytuple > event_idx;

  int n=LP.nbLot;
  int m=LP.nbLine;
  int Q=LP.nbSublot;
  int i, k,q;

  for (i=0;i<n;++i) {
    for (q=0;q<Q;++q) {
      if (s.a[i][q]!=-1){
	event_idx.push_back(std::make_tuple(s.st[i][q],i,q,s.a[i][q],0));
	event_idx.push_back(std::make_tuple(s.ft[i][q],i,q,s.a[i][q],1));
      }
    }
  }
  
  std::sort(event_idx.begin(), event_idx.end(),comparator);
  
  for (i=0;i<event_idx.size();++i) {
    if (std::get<4>(event_idx[i])==0)
      model.add(X[std::get<1>(event_idx[i])][std::get<2>(event_idx[i])][i]==1);
    else 
      model.add(Y[std::get<1>(event_idx[i])][std::get<2>(event_idx[i])][i]==1);
  }
  int affect=0;
  for (k=0;k<m;++k){
    affect=0;
    for (i=0;i<event_idx.size();++i){
      if (s.dm[k]==std::get<0>(event_idx[i]) && !affect){
	model.add(BM[i][k]==1);
	affect=1;
      }
    }
  }
  return 0;
  
}


bool comparator(const mytuple& l, const mytuple& r)  {   
  if (std::get<0>(l)-std::get<0>(r)>NEGATIVE_ZERO && std::get<0>(l)-std::get<0>(r)<POSITIVE_ZERO) {
    if (std::get<3>(l)==std::get<3>(r))
      return (std::get<4>(l)>std::get<4>(r));
    else return (std::get<4>(l)<std::get<4>(r));
  }
  else return (std::get<0>(l) < std::get<0>(r));
}

