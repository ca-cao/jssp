#ifndef INDIVIDUO_HPP
#define INDIVIDUO_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;


typedef vector<vector<pair<int,int>>> instance;

struct job{
public:
    int start;
    int end;
    int id;
    // inidica si esta en la ruta critica
    bool is_in_rc;
    // dependencia en job
    int jobdep;
    // sucesor en job
    int jobsuc;
    // el tiempo minimo en el que puede comenzar
    int greedystart;
    // constructor para que tenga valores default
    job(){
        start=0;
        end = 0;
        id = -1;
        is_in_rc=false;
        jobdep = -1;
        jobsuc = -1;
        greedystart=0;
    }
    // regresa cuando acabo su dependencia
    inline int endep(const vector<job>& plan){return jobdep==-1?0:plan[jobdep].end;}
    void set_gstart(const instance& req);
    // encuentra donde esta en el plan
    int find_pid(const vector<job>& plan);
};

// imprime un job
ostream& operator<<(ostream& os, const job& dt);

class individuo{
	public:
	    // constructor
	    individuo();
	    individuo(int njobs,int nmaq);
        ~individuo();
        void build_dep();
        void load(string fname,const instance& req);
        void create_rand(instance req);
        void show(ostream& os=cout);
        void eval(const instance& req);
        vector<vector<int>> get_rc();
        void show_rc(ostream& stream=cout);
        bool operator<(const individuo& other );
        void swap_op(int op1,int op2);
        void move_op(int op1,int op2);
        vector<pair<int,int>> save_times();
        void set_times(const vector<pair<int,int>>& times);
        vector<pair<job*,job*>> gaps(const instance& req);
        int costo();
        int rc_size();
        bool connected(int u,int v);
        auto fitness() const;
        void perturb(double pm,double pj);

        friend class jsp;
        friend class prule;
   //private:
        int nmaq;
        int njobs;
	    int cost;
        vector<job> plan; 
        vector<vector<int>> ruta;
};
    

#endif
