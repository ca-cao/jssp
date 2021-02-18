#ifndef NSGA2_HPP
#define NSGA2_HPP
#include"individuo.hpp"
#include<iostream>
#include<fstream>
#include<sstream>
#include"jsp.hpp"
#include<string>
#include<vector>

using namespace std;


// compara lexicograficamente
bool lex_vec_cmpr(const individuo& i1, const individuo& i2);

// inicializa una poblacion al azar
vector<individuo> initpop(int popsize,instance req);

// Evalua la poblacion
void eval(vector<individuo>& pop,instance req);

// hace el nondominated sorting 
void ndsort(vector<individuo>& pop);

// recibe la poblacion y el objetvo por el cual ordenar
// regresa el orden de los indices que lo ordenan
vector<int> argsort(const vector<individuo>& pop,int kobj);

// asigna los numeros de crowding
void crowdsort(vector<individuo>& pop);


// cruce por orden parcial
individuo porderx(const individuo& x,const individuo& y);

// mutacion por busqueda local iterada
void mutate(vector<individuo>& pop,jsp* problem);

// regresa un individuo elegido por torneo binario
int crtournament(const vector<individuo>& parents);

// cruza la poblacion
vector<individuo> cross(const vector<individuo>& parents);

// selecciona de acuerdo con el crowd y con el frente
// asume que ya estan asigandos los frentes y la crowddist
vector<individuo> select(const vector<individuo>& pop);


#endif
