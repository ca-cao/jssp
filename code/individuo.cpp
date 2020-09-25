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
#include<queue>

using namespace std;

individuo::individuo(){}

individuo::~individuo(){}

ostream& operator<<(ostream& os, const job& dt){
    os <<"\nid:\t" <<dt.id <<"\n"<< "jobdep:\t" <<dt.jobdep;
    os << "\tjobsuc:\t" <<dt.jobsuc<<"\n";
    os <<"start:\t"<<dt.start <<"\tend:\t"<<dt.end<<endl;
    os << "\n";
    return os;
}

void individuo::load(string fname,const vector<vector<pair<int,int>>>& req){
    ifstream infile;
    infile.open(fname);
    string line;
    int time,maq,jid,pid;
    getline(infile,line);
    istringstream lines(line);
    lines>>this->njobs;
    lines>>this->nmaq;

    vector<job> all(this->njobs*this->nmaq);
    plan.resize(this->njobs*this->nmaq);
    
    int countrow=0,countcol=0,j;
    while(getline(infile,line)){
        countcol=0;
        istringstream lines(line);
        for(int i=0;i<njobs;i++){
            lines>>j;
            pid = countrow*njobs + countcol;

            // encontrar su jobid
            jid = find_if(req[j].begin(),req[j].end(),[&countrow](pair<int,int> x){
                        return (x.first == countrow);})-req[j].begin();
            jid = j*nmaq+jid;

            all[jid].id = jid;
             
            // si no es el ultimo
            // avisa a su sucesor
            if( (jid+1)%nmaq != 0 ) 
                all[jid +1].jobdep = pid;
            else
                all[jid].jobsuc = -1;

            // si no es el primero
            // avisa a su depedendencia
            if( jid%nmaq !=0 ) 
                all[jid-1].jobsuc = pid;
            else
                all[jid].jobdep = -1;
           
            // asignacion al plan  
            plan[pid].id = jid;
            plan[pid].is_in_rc = false;

            countcol++;
        }
        countrow++;
    }
    for(int i=0;i<plan.size();i++)
        plan[i] = all[plan[i].id];
    this->cost = -1;

}

// regresa true si tiene menor o igual makespan y menos rutas criticas
bool individuo::operator<(const individuo& other){
    return this->cost<other.cost or 
        (this->cost==other.cost and this->ruta.size()<other.ruta.size());
}

void individuo::create_rand(vector< vector< pair <int,int> > > req){
    int maq,jid,pid;
    this->njobs = req.size();
    this->nmaq = req[0].size();
    
    // columna:=jobs ; fila:=maquina 
    vector<job> all(this->njobs*this->nmaq);
    
    plan.resize(this->njobs*this->nmaq);

    // vector para escojer trabajo
    vector<int> choice(req.size());
    for(int i=0;i<req.size();i++)
        choice[i]=i;
        
    // cuenta en que trabajo va
    vector<int> nop(this->nmaq,0);
    
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //seed = 1;
    
    for(int i=0;i<this->nmaq;i++){
        shuffle(choice.begin(),choice.end(),default_random_engine(seed+1));
        for(int njob :choice){
            // maquina del job
            maq = req[njob][0].first;
            // id del trabajo y del plan
            jid = njob*this->nmaq +(this->nmaq-req[njob].size());
            pid = njobs*maq + nop[maq];

            all[jid].id = jid;
            all[jid].is_in_rc = false;
             
            // si no es el ultimo
            // avisa a su sucesor
            if( (jid+1)%nmaq != 0 ) 
                all[jid +1].jobdep = pid;
            else
                all[jid].jobsuc = -1;

            // si no es el primero
            // avisa a su depedendencia
            if( jid%nmaq !=0 ) 
                plan[all[jid].jobdep].jobsuc = pid;
            else
                all[jid].jobdep= -1;
           
            // asignacion al plan  
            plan[pid] = all[jid];

            // eliminarlo de la cola
            req[njob].erase(req[njob].begin());

            // avanzar a la maquina
            nop[maq]++;
        }
    }
    this->cost = -1;
}

void individuo::show(ostream& os){
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            os<<this->plan[i*njobs+j].id<<"\t";
        }
        os<<"\n";
    }
    // tiempos
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            os<<this->plan[i*njobs+j].start<<"\t";
        }
        os<<"\n";
    }
    
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            os<<this->plan[i*njobs+j].end<<"\t";
        }
        os<<"\n";
    }
}

void individuo::eval(const vector<vector<pair<int,int>>>& req){
    
    // resetear los tiempos
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<this->njobs;j++){
            this->plan[i*njobs+j].start = 0;
            this->plan[i*njobs+j].end = 0;
        }
    }

    // cuenta en que trabajo va
    vector<int> nop(this->nmaq,0);
    
    int op,njob,jid,check,dep,pid;
    
    bool finished = false; 
    
    while(!finished){
        check = accumulate(nop.begin(),nop.end(),0);
    // intentar hacer el siguiente trabajo en la maquina i
        for(int i=0;i<this->nmaq;i++){
            if(nop[i]== this->njobs)
                continue;

            pid = i*njobs+nop[i];
            plan[pid].is_in_rc = false;
            // posicion en req
            jid = this->plan[pid].id;
            op = jid%this->nmaq;
            njob = (jid-op)/this->nmaq;
            
            dep = this->plan[pid].jobdep;
            // si no dependia de otro
            if(op==0){
                this->plan[pid].start = nop[i]==0?0:this->plan[pid-1].end;
                this->plan[pid].end = this->plan[pid].start+req[njob][op].second;
                nop[i] += 1;
            }
            // si ya termino el pasado
            else if(plan[dep].end !=0){
                // le asigna el mayor tiempo entre su dependencia en el job y la maquina
                this->plan[i*njobs+nop[i]].start = nop[i]==0?plan[dep].end:max(this->plan[pid-1].end,plan[dep].end);
                this->plan[i*njobs+nop[i]].end = this->plan[pid].start + req[njob][op].second;
                nop[i] += 1;
            }
        }
        // cuando ya acabo todos
        finished = (accumulate(nop.begin(),nop.end(),0) == this->nmaq*this->njobs) ;
        if(check== accumulate(nop.begin(),nop.end(),0)){
            this->cost = -1;
            return;
        }
    }
    this->cost=0;
    // makespan
    for(int i=0;i<this->nmaq;i++){
        this->cost = this->cost<this->plan[i*njobs+njobs-1].end?this->plan[i*njobs+njobs-1].end:this->cost;
    }
    return;
}

// guardar solo comienzo y final de bloques
vector<vector<int>> individuo::get_rc(){
    this->ruta.clear();
    vector<vector<int>> rc;
    // ruta critica
    for(int i=0;i<this->nmaq;i++)
        if(this->plan[i*njobs+njobs-1].end==this->cost){
            rc.push_back( vector<int>( 1,i*njobs+njobs-1 ) );
            this->plan[i*njobs+njobs-1].is_in_rc = true;
        }

    vector<int> current;
    int jdep;

    while(!rc.empty()){
        // tres casos
        // ambos o solo alguno de los dos
        int last=rc.back().back();
        if(plan[last].start==0){// termino la ruta
            this->ruta.push_back(rc.back());
            rc.pop_back();
            continue;
        }
        // plan[last].jobdep puede ser -1
        jdep = plan[last].jobdep!=-1?plan[last].jobdep:plan[last].jobsuc;
        // el end jobsuc no concuerda con el inicio ni final de el mismo
        // agrega los dos
        if(plan[jdep].end == plan[last-(last%njobs !=0)].end){
            current = rc.back();
            rc.pop_back();
            // agrega el jobdep
            current.push_back(jdep);
            rc.push_back(current);
            current.pop_back();
            plan[jdep].is_in_rc = true;
            // agrega el maqdep
            current.push_back(last-1);
            rc.push_back(current);
            plan[last-1].is_in_rc = true;
        }
        else if(plan[jdep].end==plan[last].start){
            rc.back().push_back(jdep);
            plan[jdep].is_in_rc=true;
        }
        else if(plan[last-(last%njobs !=0)].end==plan[last].start){
            rc.back().push_back(last-1);
            plan[last-1].is_in_rc=true;
        }
    }
    return this->ruta;
}

void individuo::show_rc(ostream& os){
    int m; // maquina
    for(auto rc : ruta){
        for(auto j:rc){
            // maquina inicio
            // maquina fin
            //m = j%njobs;
            //m = (j-m)/njobs  ;
            m = j/njobs;
            os<<m<<" "<<plan[j].end<<endl;
            os<<m<<" "<<plan[j].start<<endl;
        }
    }
}

vector<pair<int,int>> individuo::save_times(){
    vector<pair<int,int>> times(nmaq*njobs);
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs;j++){
            times[plan[i*njobs+j].id].first = plan[i*njobs+j].start;
            times[plan[i*njobs+j].id].second = plan[i*njobs+j].end;
        }
    }
    return times;
}

void individuo::set_times(const vector<pair<int,int>>& times){
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs;j++){
            plan[i*njobs+j].start = times[plan[i*njobs+j].id].first;
            plan[i*njobs+j].end = times[plan[i*njobs+j].id].second;
            plan[i*njobs+j].is_in_rc = false;
        }
    }
}

// genera todos los cambios para la vecindad N7
vector<pair<int,int>> individuo::make_vec(const vector<vector<pair<int,int>>>& req){
    vector<pair<int,int>> n7;
    int i,j;
    // generar la vecindad
    for(auto rc :ruta){
        i = 0;
        j = 0;
        while(i != rc.size()){
            // i es el inicio del bloque
            while(true){
                j++;
                // si ya llego al final de la ruta o no estan en la misma maquina
                if(j == rc.size() or rc[i]/njobs != rc[j]/njobs)
                    break;
                // si el predecesor de i esta en la ruta critica
                if(plan[plan[rc[i]].jobdep].is_in_rc){
                    n7.push_back(pair<int,int>(rc[i],rc[j]));
                    n7.push_back(pair<int,int>(rc[j],rc[i]));
                }
            }
            j--;
            // j es el final del bloque
            while(true){
                i++;
                // si ya llego al final de la ruta o no estan en la misma maquina
                if(i == rc.size() or rc[i]/njobs != rc[j]/njobs)
                    break;
                // si el sucesor de j esta en la ruta critica
                if(plan[plan[rc[j]].jobsuc].is_in_rc){
                    n7.push_back(pair<int,int>(rc[i],rc[j]));
                    n7.push_back(pair<int,int>(rc[j],rc[i]));
                }
            }
        }
    }
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //seed = 1;
    shuffle(n7.begin(),n7.end(),default_random_engine(seed));
    return n7;
}


void individuo::swap_op(int a,int b){
    job aux = plan[a];
    plan[a] = plan[b];
    plan[b] = aux;
    
    if(plan[a].jobdep != -1)
        plan[plan[a].jobdep].jobsuc = a;
    if(plan[a].jobsuc != -1)
        plan[plan[a].jobsuc].jobdep = a;
    
    if(plan[b].jobdep != -1)
        plan[plan[b].jobdep].jobsuc = b;
    if(plan[b].jobsuc != -1)
        plan[plan[b].jobsuc].jobdep = b;
}

// recorre a al lugar de b
void individuo::move_op(int a,int b){
    if(a>b){
        while(a>b){
            swap_op(a,a-1);
            a = a-1;
        }
    }
    else if(a<b){
        while(a<b){
            swap_op(a,a+1);
            a= a+1;
        }
    }
}

unsigned long int individuo::costo(){
    return this->cost;
}

void individuo::N7climb(const vector<vector<pair<int,int>>>& req){
    int u,v;
    unsigned long int cost0;
    vector<pair<int,int>> times;
    int rc_save;
    this->eval(req);
    this->get_rc();

    vector<pair<int,int>> n7 = make_vec(req);
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
            n7 = make_vec(req);
        }
        else if(this->cost == cost0){
            rc_save = this->ruta.size();
            this->get_rc();
            // cambiar si tiene menos rutas criticas
            if(this->ruta.size() < rc_save )
                n7 = make_vec(req);
            else{
            // regresar al individuo a su estado
                move_op(v,u);
                this->cost = cost0;
                this->set_times(times);
                this->get_rc();
            }
        }
        // regresar al individuo a su estado
        else{
            move_op(v,u);
            this->cost = cost0;
            this->set_times(times);
            this->get_rc();
        }
    }
}

// regresa si hay un camino de u a v
// BFS no recursivo
bool individuo::connected(int u,const int v){
    if(u ==-1 or v ==-1)
        return false;
    vector<bool> visited(nmaq*njobs);
    queue<int> qiu;
    visited[u] = true;
    qiu.emplace(u);
    while(!qiu.empty()){
        u = qiu.front();
        qiu.pop();
        if(u==v){
            return true;
        }
        // si no esta al final del job
        if(plan[u].jobsuc !=-1 and !visited[plan[u].jobsuc]){
            visited[plan[u].jobsuc]=true;
            qiu.emplace(plan[u].jobsuc);
        }
        // si no esta al final de la maquina
        if((u+1)%njobs !=0 and !visited[u+1]){
            visited[u+1] = true;
            qiu.emplace(u+1);
        }
    }
    return false;
}

void individuo::perturb(){
    // escojer un numero al azar de cambios
    default_random_engine generator;
    uniform_int_distribution<int> dist(nmaq,njobs*nmaq-1);
    int npert = dist(generator);
    vector<int> choice(njobs*nmaq);
    
    for(int i=0;i<njobs*nmaq;i++)
        choice[i]=i;
    
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //seed = 1;
    shuffle(choice.begin(),choice.end(),default_random_engine(seed));

    job u,v;
    // ---v-u---  -> ---u-v---
    /*
    for(int j=1;j<njobs;j++){
        for(int i=0;i<nmaq;i++){
            v = plan[i*njobs+j-1];
            u = plan[i*njobs+j];
            // cambiarlo con el anterior si no genera un ciclo
            if(!connected(v.jobsuc,u.jobdep)){
                swap_op(i*njobs+j-1,i*njobs+j);
                break;
            }
        }
    }
    */
    for(int i=0;i<npert;i++){
        if(choice[i]%njobs==0)
            continue;
        u = plan[choice[i]];
        v = plan[choice[i]-1];
        if(!connected(v.jobsuc,u.jobdep)){
            swap_op(choice[i],choice[i]-1);
            cout<<"swap!\n";
        }
    }

}


// encontrar los huecos y tratar de meter jobs de la rc ahi
vector<pair<int,int>> individuo::make_vec2(const vector<vector<pair<int,int>>>& req){
    vector<pair<int,int>> vec;
    vector<vector<pair<int,int>>> gaps;
    gaps.resize(nmaq);
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs-1;j++){
            // si hay un gap
            if(plan[i*njobs+j].end!=plan[i*njobs+j+1].start)
                gaps[i].push_back(make_pair<int,int>(i*njobs+j,i*njobs+j+1));
        }
    }
    int aux;
    for(auto rc : ruta){
        for(int j=1;j<rc.size()-1;j++){
            aux = rc[j];
            for(auto k: gaps[aux/njobs]){
                while(k.second<aux){
                    // mover a la izq
                    if(connected(plan[aux-1].jobsuc,plan[aux].jobdep))
                        break;
                    swap_op(aux,aux-1);
                    aux-=1;
                }
                while(k.first>aux){
                    // mover a la der
                    if(connected(plan[aux].jobsuc,plan[aux+1].jobdep))
                        break;
                    swap_op(aux,aux+1);
                    aux++;
                }
                if(aux == k.second)
                    vec.push_back(pair<int,int>(rc[j],k.second));
                if(aux == k.first)
                    vec.push_back(pair<int,int>(rc[j],k.first));
            }
            // regresarla a su lugar original
            move_op(aux,rc[j]);
        }
    }
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //seed = 1;
    shuffle(vec.begin(),vec.end(),default_random_engine(seed));
    return vec;

}

void individuo::N8climb(const vector<vector<pair<int,int>>>& req){
    int u,v;
    unsigned long int cost0;
    vector<pair<int,int>> times;
    int rc_save;
    this->eval(req);
    this->get_rc();
    vector<pair<int,int>> n8 = make_vec2(req);
    while(!n8.empty()){
        cost0=this->cost;
        u = n8.back().first; v = n8.back().second;
        n8.pop_back();

        // guarda el schedule
        times = this->save_times();

        // moverlo 
        move_op(u,v);

        // evaluar
        this->eval(req);

        // ver si es mejor
        if(this->cost<cost0){
            this->get_rc();
            n8 = make_vec2(req);
        }
        else if(this->cost == cost0){
            rc_save = this->ruta.size();
            this->get_rc();
            // cambiar si tiene menos rutas criticas
            if(this->ruta.size() < rc_save )
                n8 = make_vec2(req);
            else{
            // regresar al individuo a su estado
                move_op(v,u);
                this->cost = cost0;
                this->set_times(times);
                this->get_rc();
            }
        }
        // regresar al individuo a su estado
        else{
            move_op(v,u);
            this->cost = cost0;
            this->set_times(times);
            this->get_rc();
        }
    }
}

// branch and bound
void individuo::bnb(const vector<vector<pair<int,int>>>& req){
    // evaluar y conseguir su rc
    eval(req);
    get_rc();
     
    // se olvidan todos los ordenes de la ruta critica
    for(auto rc : ruta){
        for(int i=0;i<rc.size();i++){
        // definir los candidatos para cada bloque

        }
    }

}

/*
void individuo::replan_machine(){
    // escoger maquina aleatoria
    default_random_engine generator;
    uniform_int_distribution<int> distribution(0,nmaq-1);
    int m = distribution(generator);
    vector<jobs> trial(nmaq*njobs);

    int dep,check;
    bool finshed = true;
    // guarda el numero de trbajo en el que va
    vector<int> nop(nmaq,0);
    // empezar a planificar hasta que ya no se pueda
    while(!finished){
        check = accumulate(nop.begin(),nop.end(),0);
        for(int i=0 ;i<nmaq;i++){
            if(i == m) continue;
            dep = plan[nop[i]].jobdep;
            // planificarlo si se puede
            // si no tiene dependencia en job
            if(dep==-1){
                trial[nop[i]].id = plan[nop[i]].id;
                trial[nop[i]].is_in_rc = true; 
                nop[i]++;
            }
            // si su dependencia ya se planifico
            else if(trial[dep].is_in_rc){
                trial[nop[i]].id = plan[nop[i]].id;
                trial[nop[i]].is_in_rc = true; 
                nop[i]++;
            }
        }
        // si ya no logro poner ninguno
        if(check == accumulate(nop.begin(),nop.end(),0)){
            // poner uno de la maquina m

        }
    }
}
*/