#ifndef INDIVIDUO_HPP
#define INDIVIDUO_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;

typedef vector<vector<pair<int,int>>> instance;

struct job{
    int start;
    int end;
    int id;
    // inidica si esta en la ruta critica
    bool is_in_rc;
    // dependencia en job
    int jobdep;
    // sucesor en job
    int jobsuc;
    // constructor para que tenga valores default
    job(){
        start=0;
        end = 0;
        id = -1;
        is_in_rc=false;
        jobdep = -1;
        jobsuc = -1;
    }
};

// imprime un job
ostream& operator<<(ostream& os, const job& dt);

class individuo{
	public:
	    // constructor
	    individuo();
        ~individuo();
        void load(string fname,const instance& req);
        void create_rand(instance req);
        void show(ostream& os=cout);
        void eval(const instance& req);
        vector<vector<int>> get_rc();
        void show_rc(ostream& stream=cout);
        bool operator<(const individuo& other );
        void swap_op(int op1,int op2);
        void move_op(int op1,int op2);
        vector<pair<int,int>> make_vec(const instance& req);
        vector<pair<int,int>> save_times();
        void set_times(const vector<pair<int,int>>& times);
        vector<pair<job*,job*>> gaps(const instance& req);
        unsigned long int costo();
        void set_cost(unsigned long int c);
        int rc_size();
        bool connected(int u,int v);
        void perturb(double pm=.85,double pj=.5);
        vector<pair<int,int>> make_vec2(const instance& req);
        void bnb(const instance& req);
	    vector < pair<int, int> > create_all_vec();
        auto fitness() const;
   //private:
        int nmaq;
        int njobs;
	    unsigned long int cost;
        vector<job> plan; 
        vector<vector<int>> ruta;

};
    

#endif
