#include"individuo.hpp"
#include<algorithm>
#include<iostream>
#include<fstream>
#include<sstream>
#include<numeric>
#include<vector>
#include<random>
#include<chrono>
#include<cmath>
#include<map>

using namespace std;

individuo::individuo(){}

individuo::~individuo(){}

void individuo::create_rand(vector< vector< pair <int,int> > > req){
    int job,maq,jobid;
    this->njobs = req.size();
    this->nmaq = req[0].size();
    // columna:=jobs ; fila:=maquina 
    vector<vector <int>> plan(req[0].size());

    // vector para escojer trabajo
    vector<int> choice(req.size());
    for(int i=0;i<req.size();i++)
        choice[i]=i;
        
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    
    for(int i=0;i<this->nmaq;i++){
        shuffle(choice.begin(),choice.end(),default_random_engine(seed+1));
        for(int job :choice){
            // maquina del job
            maq = req[job][0].first;
            // id del trabajo
            jobid = job*this->nmaq +(this->nmaq-req[job].size());
            // agregarlo a la maquina correspondiente
            plan[maq].push_back(jobid);
            // eliminarlo de la cola
            req[job].erase(req[job].begin());
        }
    }
    this->plan = plan;
    this->cost = -1;
}


void individuo::show(ostream& stream){
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            stream<<this->plan[i][j]<<"\t";
        }
        stream<<"\n";
    }

    // tiempos
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            stream<<this->time[i][j].first<<"\t";
        }
        stream<<"\n";
    }
    
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            stream<<this->time[i][j].second<<"\t";
        }
        stream<<"\n";
    }
    //stream<<"costo\t"<<this->cost<<endl;
}

void individuo::eval(const vector<vector<pair<int,int>>>& req){
    
    // tiempos de inicio y final 
    vector<vector<pair<int,int>>> time(this->nmaq, vector<pair<int,int>>(this->njobs));

    // cuenta en que trabajo va
    vector<int> nop(this->nmaq,0);

    // conjunto de tareas terminadas y tiempo en que acaban
    map<int,int> done;

    int maq,job,jobid;
    
    bool finished = false; 
    
    while(!finished){
    // intentar hacer el siguiente trabajo en la maquina i
        for(int i=0;i<this->nmaq;i++){
            if(nop[i]>(this->njobs-1))
                continue;
            jobid = this->plan[i][nop[i]];
            maq = jobid%this->nmaq;
            job = (jobid-maq)/this->nmaq;
            auto dep = done.find(jobid-1);
            // si no dependia de otro
            if(maq==0){
                time[i][nop[i]].first = nop[i]==0?0:time[i][nop[i]-1].second;
                time[i][nop[i]].second = time[i][nop[i]].first+req[job][maq].second;
                done.insert(pair<int,int>(jobid,time[i][nop[i]].second));
                nop[i] += 1;
            }
            // si ya termino el pasado
            else if(dep!=done.end()){
                //time[i][nop[i]].first = nop[i]==0?0:time[i][nop[i]-1].second;
                //cout<<nop[i]<<"\t"<<prevmaq<<endl;
                // le asigna el mayor tiempo entre su dependencia en el job y la maquina
                time[i][nop[i]].first = max(time[i][nop[i]-1].second,dep->second);
                time[i][nop[i]].second = time[i][nop[i]].first + req[job][maq].second;
                done.insert(pair<int,int>(jobid,time[i][nop[i]].second));
                nop[i] += 1;
            }
        }
        //cout<<accumulate(nop.begin(),nop.end(),0)<<endl;
        // cuando ya acabo todos
        if(accumulate(nop.begin(),nop.end(),0) == this->nmaq*this->njobs)
            finished = true;
    }
    this->time = time;
    
    this->cost=0;
    for(int i=0;i<this->nmaq;i++){
        this->cost = this->cost<time[i].back().second?time[i].back().second:this->cost;
    }

    vector<vector<int>> rc;
    // ruta critica
    for(int i=0;i<this->nmaq;i++)
        if(time[i].back().second==this->cost){
            rc.push_back(vector<int>(1,time[i].back().first));
            nop[i] -=1; 
        }

    // iterar en cada uno de los trabajos finales
}

/*

// evaluar al vecino
int individuo::eval_vec(pair<int,int> change,const vector<pair<int,int>>& rest){
    int vec_cost=this->cost;    
    for(int i=0;i<this->nant;i++){
        if(change.first==i)
            continue;
    // restar si se habia sumado
        if(abs(this->genT[change.first]-this->genT[i])
                <=rest[change.first*this->nant+i].first)
            vec_cost-=rest[change.first*this->nant+i].second;
    // sumar si hay restriccion
        if(abs(change.second-this->genT[i])
                <=rest[change.first*this->nant+i].first)
            vec_cost+=rest[change.first*this->nant+i].second;

    }
    return vec_cost;
}

// evaluar al vecino y regresa true si hizo el cambio
bool individuo::change_vec(pair<int,int> change,const vector<pair<int,int>>& rest){
    int nchan=this->nchan;
    int nant=this->nant;
    unsigned int cost0=0,cost1=0;
    int trx=change.first,chan=change.second;
    // calcular el costo base
    for(int i=0;i<nant;i++){
        // si la distancia es menor
        if(abs(this->genT[trx]-this->genT[i])<=rest[trx*nant+i].first)
            cost0+=rest[trx*nant+i].second;
        if(abs(chan-this->genT[i])<=rest[trx*nant+i].first)
            cost1+=rest[trx*nant+i].second;
    }
    if(cost0>cost1){
        this->genT[trx]=chan;
        return true;
    }
    else
        return false;
}

// recibe 2 trx regresa true o false si encontro una asignacion mejor y hace la asignacion
bool individuo::trx_pair(pair<int,int> change,const vector<pair<int,int>>& rest){
    int nchan=this->nchan;
    int nant=this->nant;
    unsigned int cost=0,c1=0,c2=0;
    int trx1=change.first,trx2=change.second;
    // calcular el costo base
    for(int i=0;i<nant;i++){
            // si la distancia es menor
            if(abs(this->genT[trx1]-this->genT[i])
                    <=rest[trx1*nant+i].first)
                cost+=rest[trx1*nant+i].second;
            if(abs(this->genT[trx2]-this->genT[i])
                    <=rest[trx2*nant+i].first)
                cost+=rest[trx2*nant+i].second;
    }
    if(abs(this->genT[trx1]-this->genT[trx2])
            <=rest[trx1*nant+trx2].first)
        cost-=rest[trx1*nant+trx2].second;

    // construir la tabla con deltas y canales
    vector<pair<unsigned int,int>> t1,t2;
    for(int i=0;i<nchan;i++){
        for(int j=0;j<nant;j++){
                // si la distancia es menor
                if(abs(i-this->genT[j])<=rest[trx1*nant+j].first 
                        and j!=trx2 and j!=trx1)
                    c1+=rest[trx1*nant+j].second;
                if(abs(i-this->genT[j])<=rest[trx2*nant+j].first 
                        and j!=trx1 and j!=trx2)
                    c2+=rest[trx2*nant+j].second;
            }
        t1.push_back(make_pair(c1,i));
        t2.push_back(make_pair(c2,i));
        c1=0;
        c2=0;
    }
    // ordenar de menor a mayor por el costo
    sort(t1.begin(),t1.end());
    sort(t2.begin(),t2.end());
    // canales originales
    int chan1=this->genT[trx1],chan2=this->genT[trx2];
    unsigned int best=cost,ncost;
    // encontrar el minimo
    for(int i=0;i<nchan;i++){
        if(t1[i].first+t2[0].first>=best)
            break;
        for(int j=0;j<nchan;j++){
        // hallar el costo del nuevo
            ncost=t1[i].first+t2[j].first;
            if(ncost>=best)
                break;
            if(rest[trx1*this->nant+trx2].first>=abs(t1[i].second-t2[j].second))
                ncost+=rest[trx1*this->nant+trx2].second;
            if(ncost<best){
                best=ncost;
                chan1=t1[i].second;
                chan2=t2[j].second;
            }
        }
    }
    // si la solucion nueva es diferente a la anterior
    // cambia los canales y regresa true
    if(best<cost){
        this->genT[trx1]=chan1;
        this->genT[trx2]=chan2;
        return true;
    }
    else
        return false;
}



void individuo::create_greedy(const vector<pair<int,int>>& rest,int nchan_){
    this->nant=sqrt(rest.size());
    this->nchan=nchan_;
    random_device rd;
    mt19937 gen(rd());
    int maxr=0,mchan=0;
    // generar el primero de forma aleatoria
    uniform_int_distribution<> dist(0,nchan_-1);
    this->genT.push_back(dist(gen));
    // agregar uno que no tenga restricciones con el anterior
    for(int i=0;i<this->nant-1;i++){
        maxr=0;
        mchan=0;
        for(int j=0;j<i+1;j++){
            if(maxr<rest[j*this->nant+i+1].first)

                maxr=rest[j*this->nant+i+1].first;
                mchan=j;
        }
        this->genT.push_back((this->genT[i]+maxr)%nchan_);
    }
}

void individuo::load(string fname){
    fstream fin;
    fin.open(fname);
    string line;
    int chan;
    this->nchan=0;
    while(getline(fin,line)){
        istringstream channel(line);
        channel>>chan;
        this->genT.push_back(chan);
        this->nchan=this->nchan<chan?chan:this->nchan;
        
    }
    this->nant=this->genT.size();
}

    
void individuo::shake3trx(){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dchan(0,this->nchan-1);
    uniform_int_distribution<> dtrx(0,this->genT.size()-1);
    int t1=-1,t2=-1;
    // cambia 3 trx tal vez repetidos 
    for(int i=0;i<3;i++){
        this->genT[dtrx(gen)]=dchan(gen);
    }
}*/
