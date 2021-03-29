#include"individuo.hpp"
#include<algorithm>
#include<iostream>
#include<fstream>
#include<sstream>
#include<numeric>
#include<climits>
#include<vector>
#include<random>
#include<chrono>
#include<tuple>
#include<cmath>
#include<queue>
#include<set>

using namespace std;

individuo::individuo(){}
individuo::individuo(int njobs,int nmaq){
    this->njobs = njobs;
    this->nmaq = nmaq;
    this->plan.resize(njobs*nmaq);
    this->cost = -1;
}

// construir uno usando heuristicas o reglas de prioridad 

individuo::~individuo(){}

ostream& operator<<(ostream& os, const job& dt){
    os <<"\nid:\t" <<dt.id <<"\n"<< "jobdep:\t" <<dt.jobdep;
    os << "\tjobsuc:\t" <<dt.jobsuc<<"\n";
    os <<"start:\t"<<dt.start <<"\tend:\t"<<dt.end<<endl;
    os << "\n";
    return os;
}

// asume que su jobdep ya esta planificado
//inline int job::endep(const vector<job>& plan){
//    return jobdep==-1?0:plan[jobdep].end;
//}


void job::set_gstart(const instance& req){
    if(this->id==-1)
        return;
    int nmaq = req[0].size();
    int i = (this->id)/nmaq;
    int j = (this->id)%nmaq;
    int gs = accumulate(req[i].begin(),req[i].begin()+j,0,[](int s,pair<int,int> r){return s+r.second;});
    this->greedystart = gs;
    return;

}

//asume que ya esta planificado y su dep y suc tambien
int job::find_pid(const vector<job>& plan){
    if(jobdep!=-1)
        return plan[jobdep].jobsuc;
    if(jobsuc!=-1)
        return plan[jobsuc].jobdep;
    cout<<"error in findpid\n";
    return -1;
}

// construye las dependencias en los trabajos
// esto funciona cuando estan asignados todos los trabajos
void individuo::build_dep(){
    vector<job> all(this->njobs*this->nmaq);
    int pid,jid;
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs;j++){
            pid = i*njobs+j;
            jid = plan[pid].id;
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
        }
    }
    for(int i=0;i<plan.size();i++)
        plan[i] = all[plan[i].id];
    this->cost = -1;
}

void individuo::load(string fname,const instance& req){
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

// const porque no modifica nada
auto individuo::fitness() const {
    // caracteristicas o metricas
    // 1. Cmax (makespan)
    // 2. totime2 = \sum C_i^2 (suma del tiempo total cuadrado)
    // 3. flowtime = \sum J_i (flowtime)
    // 4. Icmax = \sum I(C_i=Cmax) (numero de maquinas que tardan igual al makespan)
    // 5. nrc=rc.size() (numero de rutas criticas)
    // 6. dist2=\sigma(C_i) (desviacion estandar de tiempos de finalizacion)
    vector<int> finishtimes;
    // 4. maquinas que tardan lo mismo que Cmax
    int Icmax;
    for(int i=0;i<this->nmaq;i++){
        finishtimes.push_back(plan[i*njobs+njobs-1].end);
        if(plan[i*njobs+njobs-1].end==cost)
            Icmax+=1;
    }

    // 2. suma del tiempo cuadrado total de las maquinas
    int totime2 = inner_product(finishtimes.begin(),finishtimes.end(),finishtimes.begin(),0);

    // 3. flowtime 
    int flowtime=0;
    for(int i=0;i<this->nmaq;i++){
        for(int j=0;j<njobs;j++)
            if((plan[i*njobs+j].id +1)%nmaq ==0)
                flowtime +=plan[i*njobs+j].end;
    }
    // 6. desviacion estandar <x^2>-<x>^2
    double mean2 = accumulate(finishtimes.begin(),finishtimes.end(),0)/(1.0*nmaq); 
    mean2*=mean2;
    double sqrmean = totime2 / (1.0*nmaq);
    double dist2= sqrmean-mean2; 
    
     
    // tupla 
    // 1.
    //return make_tuple(cost);
    // 2. 
    //return make_tuple(cost,totime2);
    // 3. 
    //return make_tuple(cost,flowtime);
    // 4. 
    //return make_tuple(cost,Icmax);
    // 5.
    //return make_tuple(cost,ruta.size());
    // 6.
    //return make_tuple(cost,dist2);
    //return vector<int>({cost,totime2,flowtime,ruta.size()});
    vector<double> feat({1.0*cost,1.0*totime2,1.0*flowtime,1.0*ruta.size()});
    return  inner_product(feat.begin(),feat.end(),weights.begin(),0);
    /*
    // tiempos ordenados
    vector<int> finishtimes;
    for(int i=0;i<this->nmaq;i++)
        finishtimes.push_back(plan[i*njobs+njobs-1].end);
    // reverse iterators para que quede de mayor a menor
    sort(finishtimes.rbegin(),finishtimes.rend());
    return finishtimes;
    */
}

// regresa true si el j-esimo mayor tiempo de i1 es menor que el i2
// si son iguales regresa false
/*
bool lex_vec_cmpr(const individuo& i1, const individuo& i2){
    vector<int> v1 = i1.fitness();
    vector<int> v2= i2.fitness();
    if(v1.size()!=v2.size())
        cout<<"dim error\n"<<"v1.size=: "<<v1.size()<<"\tv2.size: "<<v2.size();
    for(int i = 0;i<v1.size();i++){
        if(v1[i]<v2[i])
            return true;
        if(v1[i]>v2[i])
            return false;
    }
    return false;
}*/

// regresa true si tiene mejor fitness
bool individuo::operator<(const individuo& other){
    // construir la tupla 
    return this->fitness()<other.fitness();
    //return lex_vec_cmpr(*this,other);
}

void individuo::create_rand(instance req){
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

void individuo::eval(const instance& req){
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
        // esto repara la solucion
        if(check== accumulate(nop.begin(),nop.end(),0)){
			//cout << "Intenta reperacion" << endl;
			bool found = false;
			while(!found){
				int i = random() % nmaq;
				if (nop[i] == this->njobs) continue;
				for (int j = nop[i]; j < this->njobs; j++){
	                pid = i*njobs+j;
   	                // posicion en req
	                jid = this->plan[pid].id;
                    op = jid%this->nmaq;
                    njob = (jid-op)/this->nmaq;
                    dep = this->plan[pid].jobdep;
                    // si no dependia de otro
                    if(op==0){
                    	found = true;
			        	move_op(pid, i*njobs+nop[i]);
			        } 
                    else if(plan[dep].end !=0){
			        	found = true;
			        	move_op(pid, i*njobs+nop[i]);
                    }
				}
			}//while !found
        }//if check
    }// while !finished
    this->cost=0;
    // makespan
    for(int i=0;i<this->nmaq;i++){
        this->cost = this->cost<this->plan[i*njobs+njobs-1].end?this->plan[i*njobs+njobs-1].end:this->cost;
    }
    // evaluar el tardiness
    // la fecha limite es el final de la dependencia de su jobsuc
     /*
    int tardiness = 0;
    for(const auto& op : plan){
        // si su jobsuc es el primero en procesarse
        if( op.jobsuc%njobs ==0 ){
            tardiness+=plan[op.jobsuc].start;
        }
        // si no es el ultimo  
        else if(op.jobsuc!=-1){
           tardiness+= plan[op.jobsuc].start - plan[op.jobsuc-1].end ;
        }
    }*/
    int idletime=0;
    for(int i=0;i<nmaq;i++){
        idletime = plan[i*njobs].start;
        for(int j=0;j<njobs-1;j++)
            // sumar el tiempo que esta parada
            idletime += plan[i*njobs+j+1].start-plan[i*njobs+j].end;
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

int individuo::costo(){
    return this->cost;
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

// los elementos impares van despues de los pares
void individuo::perturb(double pm,double pj){
    // revolver la mitad de los trabajos de la maquina que esta mas tiempo hacer nada
    vector<int> ftimes,shuf(njobs),change,stimes;
    int pmaq=nmaq*pm,pjobs=njobs*pj;

    for(int i=0;i<this->nmaq;i++){
        ftimes.push_back(plan[i*njobs+njobs-1].end);
        stimes.push_back(plan[i*njobs].start);
    }
    
    for(int i =0;i<pmaq/2;i++){
        // las maquina que empieza al ultimo
        change.push_back(max_element(stimes.begin(),stimes.end())-stimes.begin());
        *max_element(stimes.begin(),stimes.end())=0;

        // las maquina que acaban primero
        change.push_back(min_element(ftimes.begin(),ftimes.end())-ftimes.begin()); 
        *min_element(ftimes.begin(),ftimes.end())=0;
    }

    //cout << "perturb\t machines:\t" << change.size()<<"\tjobs:\t"<<pjobs <<endl;
    for(int i= 0;i<njobs-1;i++)
        shuf[i]=i;
    // mezclar
    vector<int> aux;
    for(int m : change){
        random_shuffle(shuf.begin(),shuf.end());
        for(int i=0;i<pjobs/2;i++){
            swap_op(m*njobs+shuf[i],m*njobs+shuf[i+1]);
        }

    }
}

void individuo::move_vec(vector<pair<int,int>> (*vec)(const individuo&)){
    pair<int,int> move = vec(*this).back();
    move_op(move.first,move.second);
}
