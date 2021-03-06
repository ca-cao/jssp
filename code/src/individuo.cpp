#include"individuo.hpp"
#include<algorithm>
#include<iostream>
#include<fstream>
#include<sstream>
#include<numeric>
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
    this->obj = {0,0};
}

individuo::~individuo(){}

ostream& operator<<(ostream& os, const job& dt){
    os <<"\nid:\t" <<dt.id <<"\n"<< "jobdep:\t" <<dt.jobdep;
    os << "\tjobsuc:\t" <<dt.jobsuc<<"\n";
    os <<"start:\t"<<dt.start <<"\tend:\t"<<dt.end<<endl;
    os << "\n";
    return os;
}

// construye las dependencias en los trabajos
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
    // consta de una tupla con:
    // (makespan,numero de rutasc,tiempo total de las maquinas,trabajos diferentes en las rc)
    vector<int> finishtimes;
    for(int i=0;i<this->nmaq;i++)
        finishtimes.push_back(plan[i*njobs+njobs-1].end);

    // tiempo total de las maquinas
    int totime = accumulate(finishtimes.begin(),finishtimes.end(),0);

    // numero de trabajos diferentes en rc
    set<int> rcjobs;
    int rc_size=0;
    for(auto rc :ruta){
        rc_size += rc.size();
        for(int i:rc)
            rcjobs.insert(i);
    }
    // mide cuantos repetidos hay es 0 si no hay repetidos y <0 si hay
    rc_size = rcjobs.size()-rc_size;

    // distancia cuadrada promedio a la maquina que mas tarda
    double mean = totime/nmaq; 
    double sqrmean = inner_product(finishtimes.begin(),finishtimes.end(),finishtimes.begin(),0)/nmaq;
    double dist2= sqrmean - 2*cost*mean + mean*mean; 
    dist2 =  -*min_element(finishtimes.begin(),finishtimes.end());
    
    // espacios
    int idletime=0;
    for(int i=0;i<nmaq;i++){
        idletime = plan[i*njobs].start;
        for(int j=0;j<njobs-1;j++)
            // sumar el tiempo que esta parada
            idletime += plan[i*njobs+j+1].start-plan[i*njobs+j].end;
    }
    // tupla 
    return  make_tuple(cost,idletime,totime,dist2,rc_size);

}

// regresa true si tiene menor o igual makespan y menos rutas criticas
bool individuo::operator<(const individuo& other){
    // construir la tupla 
    return this->fitness()<other.fitness();
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
    this->obj = {1e15,1e15};
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
    this->obj = {cost*1.0,idletime*1.0};
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
/*
// genera todos los cambios para la vecindad N7
vector<pair<int,int>> individuo::make_vec(const instance& req){
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
                if(i == rc.size() or rc[i]/njobs != rc[j]/njobs)
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
*/


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

vector < pair<int, int> > individuo::create_all_vec(){
	vector< pair<int,int> > all_vec;
	for (int i = 0; i < nmaq; i++){
		for (int j = 0; j < njobs; j++){
			for (int k = 0; k < njobs; k++){
				if (j == k) continue;
				all_vec.push_back(make_pair(i*njobs + j, i*njobs+k));
			}
		}
	}
	return all_vec;
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

// regresa un vector con los cambios que se hiceron
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
   /* 
    
    // intento estilo montecarlo que no funciona
    random_device rd;
    individuo copy=(*this);
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);//uniform distribution between 0 and 1
    for(int i=0;i<nmaq;i++){
        for(int j=0;j<njobs;j++){
            // mover a la derecha izq o no hacer nada
            if(dis(gen)<min(pm,pj) and j!=njobs-1){
                copy.swap_op(i*njobs+j,i*njobs+j+1);
                copy.eval(req);
                if(copy<(*this))
                    *this=copy;
                else
                    copy.swap_op(i*njobs+j,i*njobs+j+1);
            }
            else if(dis(gen)<(pm+pj) and j!=0){
                copy.swap_op(i*njobs+j,i*njobs+j-1);
                copy.eval(req);
                if(copy<(*this))
                    *this=copy;
                else
                    copy.swap_op(i*njobs+j,i*njobs+j-1);
            }
        }
    }

    // escojer un numero al azar de cambios
    default_random_engine generator;
    //uniform_int_distribution<int> dist(nmaq,njobs*nmaq-1);
    uniform_int_distribution<int> dist(1,njobs/2);
    int npert = dist(generator);
    vector<int> choice(njobs*nmaq);
    
    for(int i=0;i<njobs*nmaq;i++)
        choice[i]=i;
    
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    //seed = 1;
    shuffle(choice.begin(),choice.end(),default_random_engine(seed));

    job u,v;
    // ---v-u---  -> ---u-v---
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
    for(int i=0;i<npert;i++){
        if(choice[i]%njobs==0)
            continue;
        u = plan[choice[i]];
        v = plan[choice[i]-1];
        //if(!connected(v.jobsuc,u.jobdep)){
        swap_op(choice[i],choice[i]-1);
            //cout<<"swap!\n";
        //}
    }
    */
}

/*
*/

// branch and bound
void individuo::bnb(const instance& req){
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

