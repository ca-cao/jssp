#ifndef INDIVIDUO_HPP
#define INDIVIDUO_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;

struct job{
// cambiar los punteros por enteros
// tener un jobid que viene del archivo
// tener un planid que se asigna en la construccion
// poner todo en un solo arreglo lineal
int start;
int end;
int id;
bool is_in_rc;
// dependencia en maquina
job* maqdep;
// dependencia en job
job* jobdep;
job* jobsuc;
};

ostream& operator<<(ostream& os, const job& dt);

void swap_op(job* op1,job* op2);
void move_op(job* op1,job* op2);

class individuo{
	public:
        // definir un operador de comparacion
	    // constructor
	    individuo();
        ~individuo();
        void create_rand(vector<vector<pair<int,int>>> req);
        void show(ostream& stream=cout);
        void eval(const vector<vector<pair<int,int>>>& req);
        vector<vector<job*>> get_rc();
        void N7climb(const vector<vector<pair<int,int>>>& req,bool add_gaps = false);
        vector<pair<job*,job*>> make_vec(const vector<vector<pair<int,int>>>& req,bool add_gaps = false);
        int get_maq(const vector<vector<pair<int,int>>>& req,int id);
        vector<pair<int,int>> save_times();
        void set_times(const vector<pair<int,int>>& times);
        vector<pair<job*,job*>> gaps(const vector<vector<pair<int,int>>>& req);
        unsigned long int costo();
        bool operator<(const individuo& other );
   private:
        int nmaq;
        int njobs;
	    unsigned long int cost;
        vector<vector<job>> plan; 
        vector<vector<job*>> ruta; 

};
    

#endif
