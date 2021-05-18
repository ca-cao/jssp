#include"individuo.hpp"
#include<algorithm>
#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<numeric>
#include<vector>
#include<random>
#include<chrono>
#include<cmath>
#include<map>



using namespace std;

individuo::individuo(){}

individuo::~individuo(){}

ostream& operator<<(ostream& os, const job& dt){
    os <<"\nid:\t" <<dt.id <<"\n"<< "jobdep:\t" <<(dt.jobdep)->id << "\nmaqdep:\t" << (dt.maqdep)->id <<"\n";
    os << "jobsuc:\t" <<(dt.jobsuc)->id<<"\n";
    os <<"start:\t"<<dt.start <<"\nend:\t"<<dt.end<<endl;
    os << "\n";
    return os;
}

// deberia llamarse makespan
bool individuo::operator<(const individuo& other){
    vector<int> thisftimes,otherftimes;
    for(int i=0;i<nmaq;i++){
        thisftimes[i] = plan[i].back().end;
        otherftimes[i] = other.plan[i].back().end;
    }
    sort(thisftimes.rbegin(),thisftimes.rend());
    sort(otherftimes.rbegin(),otherftimes.rend());

    for(int i=0;i<nmaq;i++){
        if(thisftimes[i]>otherftimes[i])
            return false;
        if(thisftimes[i]<otherftimes[i])
            return true;
    }
    return false;
}

//void individuo::load()

void individuo::create_rand(vector< vector< pair <int,int> > > req){
    int njob,maq,jobid;
    this->njobs = req.size();
    this->nmaq = req[0].size();
    // columna:=jobs ; fila:=maquina 
    // este deberia ser el plan un arreglo 1D
    vector<job> all(this->njobs*this->nmaq);
    //static vector<vector <job>> this->plan(req[0].size());
    this->plan.clear();
    this->plan.resize(req[0].size());

    for(int i=0;i<this->plan.size();i++) this->plan[i].reserve(this->njobs);

    // vector para escojer trabajo
    vector<int> choice(req.size());
    for(int i=0;i<req.size();i++)
        choice[i]=i;
        
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    job current_job;
    job* dep;
    
    for(int i=0;i<this->nmaq;i++){
        shuffle(choice.begin(),choice.end(),default_random_engine(seed+1));
        for(int njob :choice){
            // maquina del job
            maq = req[njob][0].first;
            // id del trabajo y dependencia
            jobid = njob*this->nmaq +(this->nmaq-req[njob].size());
            all[jobid].id = jobid;
            all[jobid].is_in_rc = false;
            if(this->plan[maq].empty()){
            // agregarlo a la maquina correspondiente
               this->plan[maq].push_back(all[jobid]);
               this->plan[maq].back().maqdep = &(this->plan[maq].back());
            }
            else{
                all[jobid].maqdep = &(this->plan[maq].back());
                this->plan[maq].push_back(all[jobid]);

            }
            if((jobid+1)%this->nmaq!=0){// el ultimo job
                all[jobid+1].jobdep = &(this->plan[maq].back());
            }
            if(jobid%this->nmaq==0){// el primer job
                this->plan[maq].back().jobdep = &(this->plan[maq].back());
            }
            this->plan[maq].back().jobsuc = &(this->plan[maq].back());
            // avisar que es el sucesor
            this->plan[maq].back().jobdep->jobsuc = &(this->plan[maq].back());
            
            // eliminarlo de la cola
            req[njob].erase(req[njob].begin());
        }
    }
    this->cost = -1;
}


void individuo::show(ostream& stream){
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            stream<<this->plan[i][j].id<<"\t";
        }
        stream<<"\n";
    }
    // tiempos
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            stream<<this->plan[i][j].start<<"\t";
        }
        stream<<"\n";
    }
    
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            stream<<this->plan[i][j].end<<"\t";
        }
        stream<<"\n";
    }

}

void individuo::eval(const vector<vector<pair<int,int>>>& req){
    
    // resetear los tiempos
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            this->plan[i][j].start = 0;
            this->plan[i][j].end = 0;
        }
    }

    // cuenta en que trabajo va
    vector<int> nop(this->nmaq,0);
    
    // conjunto de tareas terminadas y tiempo en que acaban
    // esto ya no es necesario
    map<int,int> done;

    int maq,njob,jobid,check;
    
    bool finished = false; 
    
    while(!finished){
        check = accumulate(nop.begin(),nop.end(),0);
    // intentar hacer el siguiente trabajo en la maquina i
        for(int i=0;i<this->nmaq;i++){
            if(nop[i]>(this->njobs-1))
                continue;
            jobid = this->plan[i][nop[i]].id;
            maq = jobid%this->nmaq;
            njob = (jobid-maq)/this->nmaq;
            auto dep = done.find(jobid-1);
            // si no dependia de otro
            if(maq==0){
                this->plan[i][nop[i]].start = nop[i]==0?0:this->plan[i][nop[i]-1].end;
                this->plan[i][nop[i]].end = this->plan[i][nop[i]].start+req[njob][maq].second;
                done.insert(pair<int,int>(jobid,this->plan[i][nop[i]].end));
                nop[i] += 1;
            }
            // si ya termino el pasado
            else if(dep!=done.end()){
                // le asigna el mayor tiempo entre su dependencia en el job y la maquina
                this->plan[i][nop[i]].start = nop[i]==0?dep->second:max(this->plan[i][nop[i]-1].end,dep->second);
                this->plan[i][nop[i]].end = this->plan[i][nop[i]].start + req[njob][maq].second;
                done.insert(pair<int,int>(jobid,this->plan[i][nop[i]].end));
                nop[i] += 1;
            }
        }
        // cuando ya acabo todos
        finished = (accumulate(nop.begin(),nop.end(),0) == this->nmaq*this->njobs) ;
        if(check== accumulate(nop.begin(),nop.end(),0)){
            this->cost = -1;
            return;
        }
            //finished = true;
    }
    
    this->cost=0;
    for(int i=0;i<this->nmaq;i++){
        this->cost = this->cost<this->plan[i].back().end?this->plan[i].back().end:this->cost;
    }
    return;
}

// guardar solo comienzo y final de bloques
vector<vector<job*>> individuo::get_rc(){
    this->ruta.clear();
    vector<vector<job*>> rc;
    // ruta critica
    for(int i=0;i<this->nmaq;i++)
        if(this->plan[i].back().end==this->cost){
            rc.push_back( vector<job*>( 1,&( this->plan[i].back() ) ) );
            this->plan[i].back().is_in_rc = true;
        }
    vector<job*> current;

    while(!rc.empty()){
        // tres casos
        // ambos o solo alguno de los dos
        job* last=rc.back().back();
        if(last->start==0){// termino la ruta
            this->ruta.push_back(rc.back());
            rc.pop_back();
            continue;
        }
        // agrega los dos
        if(last->jobdep->end == last->maqdep->end){
            current = rc.back();
            rc.pop_back();
            // agrega el jobdep
            current.push_back(last->jobdep);
            rc.push_back(current);
            // agrega el maqdep
            current.pop_back();
            current.push_back(last->maqdep);
        }
        else if(last->jobdep->end==last->start){
            rc.back().push_back(last->jobdep);
            last->jobdep->is_in_rc=true;
        }
        else if(last->maqdep->end==last->start){
            rc.back().push_back(last->maqdep);
            last->maqdep->is_in_rc=true;
        }
    }
    return this->ruta;
}

int individuo::get_maq(const vector<vector<pair<int,int>>>& req,int id){
    int col = id%this->nmaq;
    int row = (id-col)/this->nmaq;
    return req[row][col].first;
}

vector<pair<int,int>> individuo::save_times(){
    vector<pair<int,int>> times(nmaq*njobs);
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs;j++){
            times[plan[i][j].id].first = plan[i][j].start;
            times[plan[i][j].id].second = plan[i][j].end;
        }
    }
    return times;
}

void individuo::set_times(const vector<pair<int,int>>& times){
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs;j++){
            plan[i][j].start = times[plan[i][j].id].first;
            plan[i][j].end = times[plan[i][j].id].second;
        }
    }
}

// genera todos los cambios para la vecindad N7
vector<pair<job*,job*>> individuo::make_vec(const vector<vector<pair<int,int>>>& req,bool add_gaps){
    vector<pair<job*,job*>> n7;
    int i,j;
    // generar la vecindad
    for(auto rc :ruta){
        i = 0;
        j = 0;
        while(i != rc.size()){
            while(true){
                j += 1;
                if(j == rc.size())
                    break;
                get_maq(req,rc[i]->id)!=get_maq(req,rc[j]->id);
                if(get_maq(req,rc[i]->id)!=get_maq(req,rc[j]->id))
                    break;
                n7.push_back(pair<job*,job*>(rc[i],rc[j]));
                n7.push_back(pair<job*,job*>(rc[j],rc[i]));
            }
            while(true){
                i += 1;
                if(i == rc.size())
                    break;
                if(get_maq(req,rc[i]->id)!=get_maq(req,rc[j-1]->id))
                    break;
                n7.push_back(pair<job*,job*>(rc[i],rc[j-1]));
                n7.push_back(pair<job*,job*>(rc[j-1],rc[i]));
            }
        }
    }

    // agrega nuevos cambios
    if(add_gaps){
        for(auto rc :ruta){
            for(int i=0;i<rc.size()-1;i++){
                // si su jobdep lo detiene no puede moverse
                // cuando es el inicio de un bloque 
                if(rc[i]->jobdep->end == rc[i]->start)
                    continue;
                // buscar los posibles gaps
                for(auto x = rc[i];;x-- ){
                    // si empieza antes que su dependencia
                    // o es el primer job en la maquina
                    if(x->start <= rc[i]->jobdep->end or x->maqdep == x) 
                        break;
                    if(x->is_in_rc)
                        continue;
                    n7.push_back(pair<job*,job*>(rc[i],x));
                }
            }
        }
    }

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(n7.begin(),n7.end(),default_random_engine(seed));
    return n7;
}


unsigned long int individuo::costo(){
    return this->cost;
}

void individuo::N7climb(const vector<vector<pair<int,int>>>& req,bool add_gaps){
    job *u,*v;
    unsigned long int cost0;
    vector<pair<int,int>> times;
    int rc_save;
    this->eval(req);
    this->get_rc();

    vector<pair<job*,job*>> n7 = make_vec(req,add_gaps);
    while(!n7.empty()){
        cost0=this->cost;
        u = n7.back().first; v = n7.back().second;
        n7.pop_back();

        // guarda el schedule
        times = this->save_times();

        // moverlo 
        move_op(u,v);

        // evaluar
        this->eval(req);

        // ver si es mejor
        if(this->cost<cost0){
            this->get_rc();
            n7 = make_vec(req,add_gaps);
        }
        else if(this->cost == cost0){
            rc_save = this->ruta.size();
            this->get_rc();
            // cambiar si tiene menos rutas criticas
            if(this->ruta.size() < rc_save )
                n7 = make_vec(req,add_gaps);
            else{
                // regresarlo a su edo
                move_op(v,u);
                this->cost = cost0;
                this->set_times(times);
                // obtener la rc
                this->get_rc();
            }

        }
        // regresar al individuo a su estado
        else{
            move_op(v,u);
            this->cost = cost0;
            this->set_times(times);
        }
    }
}

// los jobs estan en un bloque critico
// cambia jobs de lugar
void swap_op( job* op1, job* op2){
    job aux = *op1;
    op1->id = op2->id;
    op2->id = aux.id;

    // si es la ultima op
    if(op2->jobsuc == op2)
        op1->jobsuc = op1;
    else{
        op1->jobsuc = op2->jobsuc;
        op1->jobsuc->jobdep = op1;
    }

    // si es la primera op
    if(op2->jobdep == op2)
        op1->jobdep = op1;
    else{
        op1->jobdep = op2->jobdep;
        op1->jobdep->jobsuc = op1;
    }

    // si es la ultima op
    if(aux.jobsuc == op1)
        op2->jobsuc = op2;
    else{
        op2->jobsuc = aux.jobsuc;
        op2->jobsuc->jobdep = op2;
    }

    // si es la primera op
    if(aux.jobdep == op1)
        op2->jobdep = op2;
    else{
        op2->jobdep = aux.jobdep;
        op2->jobdep->jobsuc = op2;
    }
}

// recorre a al lugar de b
void move_op(job* a,job* b){
    if(a>b){
        while(a>b){
            swap_op(a,a-1);
            a= a-1;
        }
    }
    else if(a<b){
        while(a<b){
            swap_op(a,a+1);
            a= a+1;
        }
    }
}

/*
void show_rc(vector<vector<job*>> rc){
    for(auto ruta : rc){
        for(auto j:ruta){
            col = j->id%nmaq;
            row = j->id/nmaq;
            cout<<req[row][col].first<<" "<<j->end<<endl;
            cout<<req[row][col].first<<" "<<j->start<<endl;
        }
    }
}
*/
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
