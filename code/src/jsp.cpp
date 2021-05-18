#include"individuo.hpp"
#include<functional>
#include"nsga2.hpp"
#include<algorithm>
#include<fstream>
#include"jsp.hpp"
#include<random>
#include<vector>
#include<chrono>
#include<cmath>

using namespace std;

template<class ForwardIt, class Compare>
ForwardIt maxelement(ForwardIt first, ForwardIt last,Compare comp){
    if (first == last) return last;
    ForwardIt largest = first;
    ++first;
    for (; first != last; ++first)
        if (comp(*largest, *first)) 
            largest = first;
    return largest;
}

instance getData(string fname){ 
    ifstream infile;
    infile.open(fname);
    string line;
    int nmaq,njobs,time,maq;
    getline(infile,line);
    istringstream lines(line);
    lines>>njobs;
    lines>>nmaq;

    instance req(njobs, vector<pair<int,int>>(nmaq));
    int countrow=0,countcol=0;
    while(getline(infile,line)){
        countcol=0;
        istringstream lines(line);
        for(int i=0;i<nmaq;i++){
            lines>>maq;
            lines>>time;
            req[countrow][countcol]=make_pair(maq,time);
            countcol+=1;
        }
        countrow+=1;
    }
    return req;
}

// constructor
jsp::jsp(string fname){
    req = getData(fname);
}

// destructor
jsp::~jsp(){};


// escala todos los tiempos a un factor gamma del maximo
instance jsp::scale_req(double gamma){
    int maxtime=0,aux;

    // encontrar el máximo
    for(auto v: req){
    aux = maxelement(v.begin(),v.end(),[](pair<int,int> p1,pair<int,int> p2){
            return (p1.second<p2.second);})->second;
    maxtime = maxtime<aux?aux:maxtime; 
    }
    for(int i=0;i<req.size();i++){
        for(int j=0;j<req[i].size();j++){
            req[i][j].second=ceil(gamma*req[i][j].second);
        }
    }
    return req;

} 

vector<pair<int,int>> make_n7(const individuo& x){
    vector<pair<int,int>> n7;
    int i,j;
    // generar la vecindad
    for(auto rc :x.ruta){
        i = 0;
        j = 0;
        while(i != rc.size()){
            // i es el inicio del bloque
            while(true){
                j++;
                // si ya llego al final de la ruta o no estan en la misma maquina
                if(j == rc.size() or rc[i]/x.njobs != rc[j]/x.njobs)
                    break;
                // si el predecesor de i esta en la ruta critica
                //if(plan[plan[rc[i]].jobdep].is_in_rc){
                    n7.push_back(pair<int,int>(rc[i],rc[j]));
                    n7.push_back(pair<int,int>(rc[j],rc[i]));
                //}
            }
            j--;
            // j es el final del bloque
            while(true){
                i++;
                // si ya llego al final de la ruta o no estan en la misma maquina
                if(i == rc.size() or rc[i]/x.njobs != rc[j]/x.njobs)
                    break;
                // si el sucesor de j esta en la ruta critica
                //if(plan[plan[rc[j]].jobsuc].is_in_rc){
                    n7.push_back(pair<int,int>(rc[i],rc[j]));
                    n7.push_back(pair<int,int>(rc[j],rc[i]));
                //}
            }
        }
    }
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //seed = 1;
    shuffle(n7.begin(),n7.end(),default_random_engine(seed));
    return n7;
}

// encontrar los huecos y tratar de meter jobs de la rc ahi
vector<pair<int,int>> make_vec2(const individuo& x){
    int nmaq = x.nmaq;
    int njobs = x.njobs;
    vector<pair<int,int>> vec;
    vector<vector<int>> gaps(nmaq);
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs-1;j++){
            // si hay un gap
            if(x.plan[i*njobs+j].end!=x.plan[i*njobs+j+1].start)
                gaps[i].push_back(i*njobs+j);
        }
    }
    for(auto rc : x.ruta){
        for(int j=1;j<rc.size()-1;j++){
            // para cada op de la ruta critica agrega el mov
            // puede que el gap este a la izq o derecha
            // aux/njobs es la maquina en la que esta aux
            for(auto k: gaps[rc[j]/njobs]){
               if(x.plan[k].start>x.plan[rc[j]].end) 
                   vec.push_back(pair<int,int>(rc[j],k+1));
               else
                   vec.push_back(pair<int,int>(rc[j],k));
            }
        }
    }
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //seed = 1;
    shuffle(vec.begin(),vec.end(),default_random_engine(seed));
    return vec;

}

individuo jsp::local_search(individuo x,vector<pair<int,int>> (*vec)(const individuo&) ){
    int u,v;
    unsigned long int cost0;
    vector<pair<int,int>> times;
    int rc_save=0;
    x.eval(req);
    x.get_rc();
    individuo trial;
    vector<pair<int,int>> n7 = (*vec)(x);
    while(!n7.empty()){
        // crear un individuo de prueba
        trial = x;
        u = n7.back().first; v = n7.back().second;
        n7.pop_back();

        // moverlo 
        trial.move_op(u,v);

        // evaluar y obtener su ruta critica
        trial.eval(req);
        trial.get_rc();

        // ver si es mejor, cambiarlo y hacer la nueva vecindad
        if(trial<x){
            x = trial;
            n7 = (*vec)(x);
        }
    }
    return x;
}


// en el fout guarda la red de cambios
individuo jsp::ILS(individuo inicial,vector<pair<int,int>> (*vec)(const individuo& ),int max_seconds,ostream& fout){
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    int iter=0,current_best=0;
    double maxnano = max_seconds*1e9;

    // empezar con algun optimo local
    inicial.eval(req);
    inicial = local_search(inicial,vec);
    individuo copy = inicial;

    // definir cuantos trabajo se van a cambiar
    double pm=.9,pj=.9;
    int weight;

    while(chrono::duration_cast<chrono::nanoseconds>(end-start).count()<=maxnano){
        if((iter-current_best)==0){
            pm =min(.9,pm+.05);
            pj =min(.9,pj+.05);
        }
        else{
            pm =max(.1,pm-.05);
            pj =max(.1,pj-.05);
        }

        // perturbacion
        copy.perturb(pm,pj);
        // busqueda local
        copy = local_search(copy,vec);
        iter++;
        if(copy<inicial){
            inicial=copy;
            current_best = iter;
        } 
        else {
		    copy = inicial;
		}
        // datos para construir la red
        // nodo vecino peso mejor_nodo 
        weight = pm*inicial.nmaq + pj*inicial.njobs;
        //fout << iter<<" " <<current_best-(current_best == iter)<<" "<<weight<<" "<<(current_best == iter)<<endl;


        //break;
        end = std::chrono::steady_clock::now();
    }
    inicial.eval(req);
    inicial.get_rc();
    // return best;
    return inicial;
}

//  NSGA II
vector<individuo> jsp::nsga2(int popsize,int max_iter){
    // genera a los hjos y padres juntos en pop
    vector<individuo> pop ;
    pop.reserve(popsize*2);
    vector<individuo> parents=initpop(popsize,req) ,hijos= initpop(popsize,req);
    for(int i= 0;i<max_iter;i++){
        // 1.0  combinar padres e hijos
        pop.insert(pop.end(),parents.begin(),parents.end());
        pop.insert(pop.end(),hijos.begin(),hijos.end());
        // 1.1  evaluar 
        eval(pop,req);
        // 2 ordenar en frentes
        ndsort(pop);
        // 3.0 hallar la distancia de crowding
        crowdsort(pop);
        // 3.1 escoger padres
        parents = select(pop);
        // 4 generar hijos
        hijos = cross(parents);
        mutate(hijos,this);
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
