#ifndef INDIVIDUO_HPP
#define INDIVIDUO_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;


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
        void load(string fname,const vector<vector<pair<int,int>>>& req);
        void create_rand(vector<vector<pair<int,int>>> req);
        void show(ostream& os=cout);
        void eval(const vector<vector<pair<int,int>>>& req);
        vector<vector<int>> get_rc();
        void show_rc(ostream& stream=cout);
        bool operator<(const individuo& other );
        void swap_op(int op1,int op2);
        void move_op(int op1,int op2);
        vector<pair<int,int>> make_vec(const vector<vector<pair<int,int>>>& req);
        void N7climb(const vector<vector<pair<int,int>>>& req);
        void N8climb(const vector<vector<pair<int,int>>>& req);
        vector<pair<int,int>> save_times();
        void set_times(const vector<pair<int,int>>& times);
        vector<pair<job*,job*>> gaps(const vector<vector<pair<int,int>>>& req);
        unsigned long int costo();
        void set_cost(unsigned long int c);
        int rc_size();
        bool connected(int u,int v);
        void perturb(const vector<vector<pair<int,int>>>& req,double pm=.85,double pj=.5);
        vector<pair<int,int>> make_vec2(const vector<vector<pair<int,int>>>& req);
        void bnb(const vector<vector<pair<int,int>>>& req);
	    vector < pair<int, int> > create_all_vec();
		void all_shift_climb(const vector<vector<pair<int,int>>>& req);
		void fitnessclimb(const vector<vector<pair<int,int>>>& req);
        auto fitness() const;
   private:
        int nmaq;
        int njobs;
	    unsigned long int cost;
        vector<job> plan; 
        vector<vector<int>> ruta;

};
    

#endif
