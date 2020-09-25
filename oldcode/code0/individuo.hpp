#ifndef INDIVIDUO_HPP
#define INDIVIDUO_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;

struct job{
unsigned int start;
unsigned int end;
unsigned short int id;
};

class individuo{
	public:
	    // constructor
	    individuo();
        ~individuo();
        void create_rand(vector<vector<pair<int,int>>> req);
        void show(ostream& stream=cout);
        void eval(const vector<vector<pair<int,int>>>& req);
   private:
        int nmaq;
        int njobs;
	    unsigned long int cost;
        vector<vector<int>> plan; 
        vector<vector<pair<int,int>>> time; 
        vector<vector<int>> ruta; 

};
    

#endif
