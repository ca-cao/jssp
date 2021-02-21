#include"individuo.hpp"
#include<functional>
#include<climits>
#include"prule.hpp"
#include<algorithm>
#include<fstream>
#include"jsp.hpp"
#include<random>
#include<vector>
#include<chrono>
#include<cmath>
#include<list>
#include<map>

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

// construye un individuo con las prioridades de rule
// guarda las operaciones que compitieron en rule->changes
individuo jsp::ASGA(prule* rule,unsigned seed){
    if(seed ==0)
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    int njobs,nmaq;
    vector<job>plan;
    njobs = req.size();
    nmaq = req[0].size();
    plan.resize(njobs*nmaq);

    if(rule->pr.size()!=plan.size())
        rule->rand_init(req,seed);
    if(!rule->changes.empty())
        rule->changes.clear();
    rule->changes.reserve(njobs*nmaq);

    // STEP 1
    vector<list<job>> mqueue(nmaq);
    vector<int> jobop(njobs),nop(nmaq,0),mintimes(nmaq,INT_MAX);
    multimap<int,int> mtimes;
    // poner a los que ya estan planificables
    // los trabajos que ya estan disponibles para cada maquina
    for(int i=0;i<njobs;i++){
        job aux;
        // id del trabajo 
        aux.id = i*nmaq;
        aux.jobdep = -1;
        // almacenar la duracion 
        aux.end = req[i][0].second;
        // calcular su greedystart
        //aux.set_gstart(req);
        // ponerlo en la cola de la maquina 
        mqueue[req[i].front().first].push_back(aux);
        mintimes[req[i].front().first] = mintimes[req[i].front().first]>aux.end?
            aux.end:mintimes[req[i].front().first];
        // avanzar el contador
        jobop[i]=1;
    }
    // construye el mapa
    for(int i=0;i<nmaq;i++) mtimes.emplace(mintimes[i],i);
    bool finished = false;
    while(!finished){
        // STEP 2
        // encontrar maquina de la op que acaba primero
        //int minmaq = min_element(mintimes.begin(),mintimes.end())-mintimes.begin();
        int minmaq,minfinish;
        do{
            minmaq = mtimes.begin()->second;
            minfinish = mtimes.begin()->first;
            mtimes.erase(mtimes.begin());
            // corregir si esta desactualizado
        }while(mintimes[minmaq]!=minfinish);

        // STEP 3 encontrar trabajos elegibles y seleccionar (prule)
        list<job>::iterator selected;
        int pid = minmaq*njobs+nop[minmaq],start;
        int machend =  nop[minmaq]==0?0:plan[pid-1].end;
        double selpr=-1;
        // operaciones que compiten
        for(auto current=mqueue[minmaq].begin();current!=mqueue[minmaq].end();current++){
            start = max(current->endep(plan),machend);
            // si empieza antes que el que acaba primero es elegible y si mqueue[minmaq][j] es que selected 
            if(start<minfinish and (rule->pr[current->id]>selpr)){
                    //selected = mqueue[minmaq].begin()+j;
                    selected = current;
                    selected->start = start;
                    selpr = rule->pr[selected->id];
            }
        }
        // planificarlo
        plan[pid].id = selected->id;
        plan[pid].start = selected->start;
        plan[pid].end = plan[pid].start+selected->end;
        plan[pid].jobdep = selected->jobdep;
        plan[pid].is_in_rc = false;
        nop[minmaq]+=1;
        if(selected->jobdep!=-1)
            plan[selected->jobdep].jobsuc = pid;
        
        // STEP 4 actualizar todo
        // poner a su jobsuc en la cola
        if((selected->id+1)%nmaq !=0){
            int jid = selected->id/nmaq;
            job aux;
            aux.id = selected->id+1;
            aux.jobdep = pid;
            aux.end = req[jid][jobop[jid]].second;
            int maqsuc = req[jid][jobop[jid]].first;
            jobop[jid]+=1;
            // agregar al nuevo a ka cola
            mqueue[maqsuc].push_back(aux);
            // actualizar mintimes
            int machend = nop[maqsuc]==0?0:plan[maqsuc*njobs+nop[maqsuc]-1].end;
            int finish = max(aux.endep(plan),machend) + aux.end;
            mintimes[maqsuc] = mintimes[maqsuc]>finish?finish:mintimes[maqsuc];
            mtimes.emplace(mintimes[maqsuc],maqsuc);
        }
        // eliminar al seleccionado
        mqueue[minmaq].erase(selected);
        // agregar las operaciones que compitieron
        // actualizar el minmaq  
        mintimes[minmaq] = INT_MAX;
        for(auto j:mqueue[minmaq]){
            rule->changes.push_back(vector<int>({plan[pid].id,j.id}));
            int finish = max(j.endep(plan),plan[pid].end) + j.end;
            mintimes[minmaq] = mintimes[minmaq]>finish?finish:mintimes[minmaq];
        }
        mtimes.emplace(mintimes[minmaq],minmaq);
        
        finished = (accumulate(nop.begin(),nop.end(),0) == nmaq*njobs);
    }// while !finished
    int cost=0;
    // makespan
    for(int i=0;i<nmaq;i++){
        cost = cost<plan[i*njobs+njobs-1].end?plan[i*njobs+njobs-1].end:cost;
    }
    individuo active(njobs,nmaq);
    active.plan = plan;
    active.cost = cost;
    
    //seed = 1;
    shuffle(rule->changes.begin(),rule->changes.end(),default_random_engine(seed));
    return active;
}


individuo jsp::local_search(prule* rule,unsigned seed){
    if(seed==0)
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    prule* trialrule = new prule();
    individuo x = ASGA(rule,seed);
    *trialrule = *rule;
    individuo trial;
    bool sort_ch=true;
    int count=0;
    while(!rule->changes.empty()){
        // hacer los cambios en prioridades
        trialrule->make_change();
        rule->changes = trialrule->changes;
        
        // crear un individuo optimo local de prueba 
        trial = ASGA(trialrule,seed);
        trial.get_rc();
        // ver si es mejor, cambiarlo y hacer la nueva vecindad
        if(trial<x){
            x = trial;
            *rule = *trialrule;
            sort_ch=true;
            count++;
        }
        else{
            *trialrule= *rule;
        }
        
    }
    return x;
}

// en el fout guarda la red de cambios
individuo jsp::ILS(prule* rule,int max_seconds,double pflip,unsigned seed,ostream& fout){
    auto start = std::chrono::steady_clock::now();
    auto end = start;
    int iter=0,current_best=0;
    double maxnano = max_seconds*1e9;
    if(seed==0)
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();

    // empezar con algun optimo local
    individuo inicial = local_search(rule,seed);
    individuo trial;
    prule* trialrule = new prule();
    *trialrule = *rule;

    int weight;

    while(chrono::duration_cast<chrono::nanoseconds>(end-start).count()<=maxnano){
        // perturbacion
        trialrule->perturb(pflip);
        
        // busqueda local
        trial = local_search(trialrule,seed);
        
        // ver si mejoro o no
        if(trial<inicial){
            inicial=trial;
            *rule = *trialrule;
            current_best = iter;
        } 
        else{
		    trial = inicial;
            *trialrule = *rule;
		}
        // datos para construir la red
        // nodo vecino peso mejor_nodo 
        //weight = pm*inicial.nmaq + pj*inicial.njobs;
        //fout << iter<<" " <<current_best-(current_best == iter)<<" "<<weight<<" "<<(current_best == iter)<<endl;
        //break;
        end = std::chrono::steady_clock::now();
    }
    inicial.eval(req);
    inicial.get_rc();
    // return best;
    return inicial;
}
