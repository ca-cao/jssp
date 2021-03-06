#ifndef PRULE_HPP
#define PRULE_HPP

#include"individuo.hpp"
#include<vector>

using namespace std;
class prule{
public:
    // regresa true si el izq es mejor que el derecho segun algun criterio
    //virtual bool compare(job left, job right,const instance& req,const vector<job>& plan,int pid);
    vector<job>::iterator select(vector<job>& mqueue,const instance& req,const vector<job>& plan,int pid);
    // si seed = 0 utiliza el tiempo como semilla
    void perturb(double pflip,unsigned seed=0);
    void perturb(int nflips,unsigned seed=0);
    void rand_init(const instance& req,unsigned seed = 0);
    void init(const individuo& x);
    void init(const vector<double>& x,int nmaq,int njobs);
    void updatepr(individuo& sol);
    bool make_change();
    void show(ostream& fout =cout);
    prule operator *(double scale) const;
    prule operator +(const prule other) const;
    bool operator ==(const prule other) const;
    // guarda que operadciones compitieron
    vector<vector<int>> changes;
    vector<double> pr;
    vector<int> pid;
    int nmaq;
    int njobs;
};

/*
class minstart : public prule{
public:
    bool compare(job left, job right,const instance& req,const vector<job>& plan,int pid);
};

class timeleft : public prule{
public:
    bool compare(job left, job right,const instance& req,const vector<job>& plan,int pid);
};

class rand_choose : public prule{
public:
    bool compare(job left, job right,const instance& req,const vector<job>& plan,int pid);
};
*/
#endif
