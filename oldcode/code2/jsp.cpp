#include<algorithm>
#include<iostream>
#include<fstream>
#include"jsp.hpp"
#include<random>
#include<vector>
#include<chrono>
#include<cmath>

using namespace std;
vector< vector< pair <int,int> > > getData(string fname){ 
    ifstream infile;
    infile.open(fname);
    string line;
    int nmaq,njobs,time,maq;
    getline(infile,line);
    istringstream lines(line);
    lines>>njobs;
    lines>>nmaq;

    vector< vector<pair<int,int>> > req(njobs, vector<pair<int,int>>(nmaq));
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

void show_rc(vector<vector<job*>> rc,const vector<vector<pair<int,int>>>& req,ostream& fout){
    int col,row;
    int nmaq = req[0].size();
    for(auto ruta : rc){
        for(auto j:ruta){
            col = j->id%nmaq;
            row = j->id/nmaq;
            fout<<req[row][col].first<<" "<<j->end<<endl;
            fout<<req[row][col].first<<" "<<j->start<<endl;
        }
    }
}

/*
// generar vecindad
vector<pair<int,int>> make_vec(individuo inicial){
    vector<pair<int,int>> vecindad;
    for(int i=0;i<inicial.nant;i++){
        for(int j=0;j<inicial.nchan;j++){
            if(inicial.genT[i]!=j)
                vecindad.push_back(make_pair(i,j));
        }
    }
    // shuffle 
    random_device rd;
    mt19937 g(rd());
    shuffle(vecindad.begin(), vecindad.end(), g);
    return vecindad;
}


// generar vecindad para 2 trx
vector<pair<int,int>> make_vec2(const vector<pair<int,int>>& rest){
    int ntrx=sqrt(rest.size());
    vector<pair<int,int>> vecindad;
    for(int i=0;i<ntrx;i++){
        for(int j=i+1;j<ntrx;j++){
            // si la penalizacion exixte
            if(rest[i*ntrx+j].second>0)
                vecindad.push_back(make_pair(i,j));
        }
    }
    // shuffle 
    random_device rd;
    mt19937 g(rd());
    shuffle(vecindad.begin(), vecindad.end(), g);
    return vecindad;
}

// escalada estocastica
individuo stoch_climb(individuo inicial,const vector<pair<int,int>>& rest){
    vector<pair<int,int>> vecindad = make_vec(inicial);
    while(!vecindad.empty()){
        if(inicial.change_vec(vecindad.back(),rest)){
            vecindad = make_vec(inicial);
            continue;
        }
        else{
            vecindad.pop_back();
        }
    }
    return inicial;
}

// escalada estocastica para nueva vecindad
individuo stoch_climb2(individuo inicial,const vector<pair<int,int>>& rest){
    // shuffle 
    random_device rd;
    mt19937 g(rd());
    //vecindad
    vector<pair<int,int>> vecindad = make_vec2(rest);
    uniform_int_distribution<> dist(0,vecindad.size()-1);
    // pares ya optimizados
    vector<pair<int,int>> optpairs;
    pair<int,int> aux;
    int count=0;
    while(!vecindad.empty()){
        swap(vecindad.back(),vecindad[dist(g)]);
        //shuffle(vecindad.begin(), vecindad.end(), g);
        // si encontro una asignacion mejor
        if(inicial.trx_pair(vecindad.back(),rest)){
            // quitarlo de la vecindad
            aux=vecindad.back();
            vecindad.pop_back();
            // revisar cuales de los optimizados estan conectados
            // y regresarlos a la vecindad
            for(int i=optpairs.size()-1;i>=0;i--){
                if(optpairs[i].first==aux.first or optpairs[i].first==aux.second or 
                        optpairs[i].second==aux.first or optpairs[i].second==aux.second){
                    vecindad.push_back(optpairs[i]);
                    swap(optpairs[i],optpairs.back());
                    optpairs.pop_back();
                }
            }
            //pasar el par a los optimizados 
            optpairs.push_back(aux);
        }
        else{
            aux=vecindad.back();
            vecindad.pop_back();
            optpairs.push_back(aux);
        }
        count+=1;
    }
    return inicial;
}


individuo VNS(individuo inicial,const vector<pair<int,int>>& rest,int max_seconds,ostream& fout){
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    inicial.eval(rest);
    fout<<"0"<<" "<<inicial.cost<<endl;
    individuo best=inicial;
    while(std::chrono::duration_cast<chrono::seconds>(end-start).count()<max_seconds){
        // shake
        inicial.shake3trx();
        // busqueda en vecindad chica
        inicial =stoch_climb(inicial,rest);
        // busqueda en vecindad grande
        inicial =stoch_climb2(inicial,rest);
        end = std::chrono::steady_clock::now();
        inicial.eval(rest);
        fout<<chrono::duration_cast<chrono::milliseconds>(end-start).count()<<" "<<inicial.cost<<endl;
        if(inicial.cost<best.cost)
            best=inicial;
    }
    return best;
}*/
