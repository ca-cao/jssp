#include "individuo.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>
#include "jsp.hpp"
#include <random>
#include <string>
#include <chrono>
#include <time.h>
#define INF INT_MAX
#define EPS 1.0e-14

using namespace std;

bool lex_vec_cmpr(const individuo& i1, const individuo& i2){
    auto v1 = i1.obj;
    auto v2 = i2.obj;
    if(v1.size()!=v2.size())
        cout<<"dim error\n"<<"v1.size=: "<<v1.size()<<"\tv2.size: "<<v2.size();
   
    return (v1[0]<v2[0]) or (v1[0]==v2[0] and (v1[1]<v2[1]));
    
    for(int i = 0;i<v1.size();i++){
        if(v1[i]<v2[i])
            return true;
        if(v1[i]>v2[i])
            return false;
    }
    return false;

}

vector<individuo> initpop(int popsize,instance req){
    vector<individuo> pop(popsize);
    for(int i=0;i<popsize;i++){
        pop[i].create_rand(req);
        pop[i].eval(req);
    }
    return pop;
}

// Evalua la poblacion 
void eval(vector<individuo>& pop,instance req){
    for(int i=0;i<pop.size();i++){
        // evaluar 
        pop[i].eval(req);
    }
}

// hace el nondominated sorting 
void ndsort(vector<individuo>& pop){
    // ordenamiento lexicografico
    sort(pop.begin(),pop.end(),lex_vec_cmpr);
    // asignar frentes
    // se fija cual de los anteriores lo domina
    pop[0].front = 0;
    vector<double>ymin;
    ymin.push_back(pop[0].obj[1]);
    bool found;
    for(int i=1;i<pop.size();i++){
        found = false;
        // encontrar el primer frente que no lo domine
        for(int j =0;j<ymin.size();j++){
            if(pop[i].obj[1]<ymin[j]){
                pop[i].front = j;
                ymin[j] = pop[i].obj[1];
                found = true;
                break;
            }
        }
        // no encontro ninguno
        // asigna un nuevo frente
        if(!found){
            pop[i].front = ymin.size();
            ymin.push_back(pop[i].obj[1]);
        }
    }
    // los ordena conforme a su frente
    sort(pop.begin(),pop.end(),[](const individuo& left,const individuo& right)
            {return left.front<right.front;});
}

// recibe la poblacion y el objetvo por el cual ordenar
// regresa el orden de los indices que lo ordenan del indice start al end
vector<int> argsort(const vector<individuo>& pop,int start,int end,int kobj){
    vector<int> idx(end-start+1);
    // crea el vector de indices
    iota(idx.begin(), idx.end(),start);
    sort(idx.begin(),idx.end(),[&pop,kobj](int left,int right)->bool{
            return pop[left].obj[kobj]<pop[right].obj[kobj];});
    return idx;
}

// asume que ya estan ordenados en frentes 
// asigna los numeros de crowding
void crowdsort(vector<individuo>& pop){
    vector<int> sorted_idx;
    int idx,idxp,idxm,start,end;
    double imax,imin;
    // encontrar los indices de los frentes
    vector<int> fridx;
    fridx.push_back(0);
    for(int i=1;i<pop.size();i++){
        if(pop[i].front!=pop[i-1].front)
            fridx.push_back(i);
    }
    fridx.push_back(pop.size());
    for(int k=1;k<fridx.size();k++){
        start = fridx[k-1];
        end = fridx[k]-1;
    for(int i=0;i<pop[0].obj.size();i++){
        // ordenar respecto al i-esimo objetivo
        sorted_idx = argsort(pop,start,end,i);
        // hallar el rango de los valores para normalizar
        imax = pop[sorted_idx.back()].obj[i];
        imin = pop[sorted_idx.front()].obj[i];
        // ya que estan ordenados asignar la crowddist
        for(int j=0;j<sorted_idx.size();j++){
            idx = sorted_idx[j];
            // a los extremos les pone una distancia grande
            if(j == 0 or j ==sorted_idx.size()-1){
                pop[idx].crowddist = 1e10;
                continue;
            }
            idxp =sorted_idx[j+1];
            idxm =sorted_idx[j-1];
            pop[idx].crowddist += (pop[idxp].obj[i]-pop[idxm].obj[i])/(imax-imin);
        }
    }
    }//for fridx
}

// regresa  un numero aleatorio entre 0 y 1
double randomperc(){
    srand((int)clock());
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

// hacer funciones de cruza y mutacion

// regresa un individuo elegido por torneo binario
int crtournament(const vector<individuo>& parents){
    // seleccionar dos al azar 
    srand((int)clock());
    int comp1=random()%parents.size(),comp2=random()%parents.size();
    // asegurarse que no sean iguales
    while(comp1==comp2)
        comp2=random()%parents.size();

    // elegir al ganador
    if(parents[comp1].front<parents[comp2].front)
        return comp1;
    if(parents[comp2].front<parents[comp1].front)
        return comp2;
    if(parents[comp2].crowddist<parents[comp1].crowddist)
        return comp2;
    return comp1;
    
}

// cruce por orden parcial
individuo porderx(const individuo& x,const individuo& y){
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(0,1);
    int xcount,ycount,rand;
    int jobid,xid,yid;
    individuo hijo(x.njobs,x.nmaq);
    for(int i=0;i<x.nmaq;i++){
        // marca si el trabajo ya fue asignado
        vector<bool> assigned(x.njobs,false);
        xcount = 0,ycount =0;
        for(int k = 0;k<x.njobs;k++){
            // genera los posibles trabajos
            // mientras alguno de los dos este asignado
            do{
                xid = x.plan[i*x.njobs+xcount].id;
                yid = y.plan[i*y.njobs+ycount].id;
                // avanza si ya esta asignado
                xcount += assigned[xid/x.nmaq];
                ycount += assigned[yid/y.nmaq];
            }while(assigned[xid/x.nmaq] or assigned[yid/y.nmaq]);
            // elegir alguno
            rand = dist(gen);
            jobid = rand*xid + (1-rand)*yid;
            hijo.plan[i*hijo.njobs+k].id = jobid;
            assigned[jobid/x.nmaq] = true;
            xcount += (rand); 
            ycount += (1-rand);
        }
    }
    // hay que asignar las jobsuc y jobdep
    hijo.build_dep();
    return hijo;
}

// mutacion por busqueda local iterada
void mutate(vector<individuo>& pop,jsp* problem){
    for(auto& x: pop){
        if(randomperc()<.2){
            x.perturb();
            //x = problem->ILS(x,make_n7,0);
            x = problem->local_search(x,make_n7);
        }
    }
}

// cruza la poblacion
vector<individuo> cross(const vector<individuo>& parents){
    vector<individuo> children(parents.size());
    int pidx1,pidx2;
    // escoger a los padres por crowding y eso
    for(int i=0;i<parents.size()/2;i++){
        pidx1 = crtournament(parents);
        pidx2 = crtournament(parents);
       // realcross(parents[pidx1],parents[pidx2],children[2*i],children[2*i+1]);
        children[2*i]= porderx(parents[pidx1],parents[pidx2]);
        children[2*i+1]= porderx(parents[pidx1],parents[pidx2]);
    }
    return children;
}

// selecciona de acuerdo con el crowd y con el frente
// asume que ya estan asigandos los frentes y la crowddist
vector<individuo> select(const vector<individuo>& pop){
    vector<individuo> parents;
    parents.reserve(pop.size()/2);
    vector<individuo> current_front;
    int upcut = pop.size()/2-1;
    int lowcut = pop.size()/2-1;
    int front = pop[upcut].front;
    // encontrar los limites del ultimo frente que cabe
    while(pop[upcut+1].front == front)
        upcut+=1;
    while(pop[lowcut-1].front == front)
        lowcut-=1;

    current_front.insert(current_front.end(),pop.begin()+lowcut,pop.begin()+upcut+1);
    sort(current_front.begin(),current_front.end(),[](const individuo& a, const individuo& b)->bool
            {
            return a.crowddist > b.crowddist;
            });
    
    parents.insert(parents.end(),pop.begin(),pop.begin()+lowcut);
    parents.insert(parents.end(),current_front.begin(),current_front.begin()+(pop.size()/2 - parents.size()));
    
    return parents;
}

/*
//  NSGA II
vector<individuo> nsga2(int popsize,int max_iter, int problem, int dim, int nobj){
    // genera a los hjos y padres juntos en pop
    vector<individuo> pop ;
    pop.reserve(popsize*2);
    vector<individuo> parents=initpop(dim,popsize) ,hijos= initpop(dim,popsize);
    for(int i= 0;i<max_iter;i++){
        // 1.0  combinar padres e hijos
        pop.insert(pop.end(),parents.begin(),parents.end());
        pop.insert(pop.end(),hijos.begin(),hijos.end());
        // 1.1  evaluar 
        eval(pop,problem);
        // 2 ordenar en frentes
        ndsort(pop);
        // 3.0 hallar la distancia de crowding
        crowdsort(pop);
        // 3.1 escoger padres
        parents = select(pop);
        // 4 generar hijos
        hijos = cross(parents);
        pop.clear();
    }
    // hallar a los no dominados
    int cut=parents.size()/2;
    int delta = cut/2;
    while(delta>0){
        cut = parents[cut].front==0?cut+delta:cut-delta;
        delta/=2;
    }
    parents.resize(cut);
    return parents;
}
*/

