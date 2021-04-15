#include"prule.hpp"
#include"individuo.hpp"
#include<algorithm>
#include<vector>
#include<numeric>
#include<random>
#include<math.h>
#include<chrono>

using namespace std;

// regresa true si ya probo todas las opciones para una decision
bool prule::make_change(){
    if(changes.back().size()==2){
        double aux=pr[changes.back()[0]];
        pr[changes.back()[0]] = pr[changes.back()[1]];
        pr[changes.back()[1]] = aux; 
        changes.pop_back();
        return true;
    }
    // el cambio de n7 tiene 3 elementos los primeros dos representan el cabio
    // el tercero el orden de u v u---v ; v -- u
    else{
        double epsdbl = numeric_limits<double>::min();
        int u = changes.back()[0];
        int v = changes.back()[1];
        // ---u---v => ---v-u 
        if(changes.back()[2]==1){
            pr[u] = max(0,pr[v]-epsdbl);
        }
        // v---u-- => u-v--- 
        else{
            pr[u] = max(1,pr[v]+epsdbl);
        }
        return true;
    }



    /*
    double minpr=2,maxpr=-1;
    // encuentra el valor maximo y minimo 
    for(int id : changes.back()){
       maxpr = maxpr*(pr[id]<=maxpr) + pr[id]*(pr[id]>maxpr);
       minpr = minpr*(pr[id]>=minpr) + pr[id]*(pr[id]<minpr);
    }
    
    double delta = (maxpr-minpr)/(changes.back().size()-1);
    for(int i=0;i<changes.back().size();i++){
        pr[changes.back()[i]] = minpr +delta*i;
    }
    // si regresa true habia mas permutaciones
    if(next_permutation(changes.back().begin(),changes.back().end())){
        return false;
    }
    else{
        changes.pop_back();
        return true;
    }
    return true;
    */
}

bool prule::operator==(prule other) const{
    if(this->pr.size()!=other.pr.size())
        return false;
    for(int i=0;i<pr.size();i++){
        if(pr[i]!=other.pr[i])
            return false;
    }
    return true;
}

// la pareja tiene (double aleatorio,greedystart)
void prule::rand_init(const instance& req,unsigned seed){
    if(seed==0)
        seed = chrono::system_clock::now().time_since_epoch().count();
    vector<double> priority(req[0].size()*req.size());
    default_random_engine eng(seed);
    uniform_real_distribution<double> dist(0,1);

    for(int i=0;i<req.size();i++){
        for(int j=0;j<req[0].size();j++)
            priority[i*req[0].size()+j] = dist(eng);
    }
    this->pr=priority;
    njobs = req.size();
    nmaq = req[0].size();
}

void prule::perturb(double pflip,unsigned seed){
    if(seed==0)
        seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine eng(seed);
    uniform_real_distribution<double> dist(0,1);
    vector<double> prcopy = pr;
    for(int i=1;i<pr.size()-1;i++){
        if(dist(eng)<pflip)
            pr[i] = 1-pr[i];
            //pr[i] = (prcopy[i+1]+prcopy[i-1])/2;
    }
}

// para usarse con nflips << pr.size()
void prule::perturb(int nflips,unsigned seed){
    if(seed==0)
        seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine eng(seed);
    uniform_int_distribution<int> dist(0,pr.size()-1);
    int count=0;
    int lastsel = dist(eng);
    while(count <nflips){
        int sel = dist(eng);
        if(sel==lastsel) continue;
        pr[sel] = abs(1-pr[sel]);
        count++;
        lastsel = sel;
    }
}

void prule::init(const individuo& x){
    nmaq = x.nmaq;
    njobs = x.njobs;
    pr.resize(nmaq*njobs);
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs;j++){
            pr[x.plan[i*njobs+j].id] = 1-(double)(j)/(double)(njobs);
        }
    }
}

void prule::init(const vector<double>& x,int nmaq,int njobs){
    nmaq = nmaq;
    njobs = njobs;
    pr.resize(nmaq*njobs);
    for(int i=0;i<nmaq*njobs;i++){
        pr[i] = x[i];
    }
}

void prule::show(ostream& fout){
    int count=0; 
    for(auto& p :pr)
        fout<<count++<<" "<<p<<endl;
}

prule prule::operator *(double scale) const{
    prule other;
    other.pr.resize(this->pr.size());
    for(int i=0;i<this->pr.size();i++){
        other.pr[i] = scale * this->pr[i];
    }
    return other;
}

prule prule::operator +(const prule other) const{
    prule ret;
    ret.pr.resize(this->pr.size());
    for(int i=0;i<this->pr.size();i++){
        ret.pr[i] = other.pr[i] + this->pr[i];
    }
    return ret;
}
/*
// selecciona una operacion de la cola
vector<job>::iterator prule::select(vector<job>& mqueue,const instance& req,const vector<job>& plan,int pid){
    vector<job>::iterator best,it = mqueue.begin();
    best = it;
    while(++it!=mqueue.end()){
        // si es mejor que el actual mejor
        if(this->compare(*it,*best,req,plan,pid))
            best = it;
    }
    return best;
}
bool prule::compare(job left,job right,const instance& req,const vector<job>& plan,int pid){
    return true;
}


bool prule::compare(job left,job right,const instance& req,const vector<job>& plan,int pid){
    int njobs = req.size();
    int machend = pid%njobs==0?0:plan[pid-1].end;
    return pr[left.id].first*(max(machend,left.endep(plan))-pr[left.id].second) >=  
        pr[right.id].first*(max(machend,right.endep(plan))-pr[right.id].second);
}

// asume que no estan planificados pero sus dependencias si
// hacer que regrese true si son iguales
bool minstart::compare(job left,job right,const instance& req,const vector<job>& plan,int pid){
    return left.endep(plan)<=right.endep(plan);
}


// elige al que le falta mas por hacer
bool timeleft::compare(job left,job right,const instance& req,const vector<job>& plan,int pid){
    int nmaq = req[0].size();
    int i1 = (left.id)/nmaq, i2 =(right.id)/nmaq;
    int j1 = (left.id)%nmaq, j2 =(right.id)%nmaq;
    int t1 = accumulate(req[i1].begin()+j1,req[i1].end(),0,[](int s,pair<int,int> r){return s+r.second;});
    int t2 = accumulate(req[i2].begin()+j2,req[i2].end(),0,[](int s,pair<int,int> r){return s+r.second;});
    return t1>=t2;
}

bool rand_choose::compare(job left,job right,const instance& req,const vector<job>& plan,int pid){
    if(left.id == right.id)
        return true;
    return random() % 2 ==0;
}

bool timeleft::compare(job left,job right,const instance& req,const vector<job>& plan,int pid){
    // tiempo que le queda a sus trabajos
    int nmaq = req[0].size();
    int i1 = (left.id)/nmaq, i2 =(right.id)/nmaq;
    int j1 = (left.id)%nmaq, j2 =(right.id)%nmaq;
    int t1 = accumulate(req[i1].begin()+j1,req[i1].end(),0,[](int s,pair<int,int> r){return s+r.second;});
    int t2 = accumulate(req[i2].begin()+j2,req[i2].end(),0,[](int s,pair<int,int> r){return s+r.second;});
    // tiempo que le queda a la maquina si la elige
    int 
    
}
    if(left.jobdep==-1) 
        start = nop[i]==0?0:plan[pid-1].end;
    // el tiempo maximo entre su jobdep y su antecesor en la maq 
    else 
        start = max(plan[aux.jobdep].end,nop[i]==0?0:plan[pid-1].end);
    if(start<minst){
        minst = start;
        argmin = j;
    }
    return false;
}
*/
