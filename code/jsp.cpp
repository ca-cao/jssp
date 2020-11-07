#include"individuo.hpp"
#include<functional>
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

vector<pair<int,int>> make_n7(individuo x){
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

individuo jsp::local_search(individuo x,vector<pair<int,int>> (*vec)(individuo) ){
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


individuo jsp::ILS(individuo inicial,vector<pair<int,int>> (*vec)(individuo),int max_seconds,ostream& fout){
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    int iter=0,nc = 100;
    inicial.eval(req);
    individuo copy = inicial;
    //fout<<"0"<<" "<<inicial.cost<<endl;
    double pm=0.5,pj=0.25;
    while(std::chrono::duration_cast<chrono::seconds>(end-start).count()<max_seconds){
        // shake
        copy.perturb(pm,pj);
        copy = local_search(copy,vec);
        if(copy<inicial){
            inicial=copy;
        } 
        else {
		    copy = inicial;
		}
        //break;
        end = std::chrono::steady_clock::now();
        iter++;
    	cout<<"\titer: "<<iter<<endl;
    }
    inicial.eval(req);
    inicial.get_rc();
    // return best;
    return inicial;
}

