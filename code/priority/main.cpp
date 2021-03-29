#include"individuo.hpp"
#include<algorithm>
#include"prule.hpp"
#include<iostream>
#include"jsp.hpp"
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<chrono>
#include<cmath>
#include<ctime>
#include "de/DifferentialEvolution.h"

using namespace std;
class vnsx: public de::IOptimizable{
public:
    vnsx(jsp problem_m,int max_seconds_m):problem(problem_m),max_seconds(max_seconds_m){
    }
    double EvaluteCost(std::vector<double> inputs) const override{
        return problem.VNS(rule,max_seconds,inputs).costo()*1.0;
    }

    unsigned int NumberOfParameters() const override{
        return 4;
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(NumberOfParameters());
        for (auto& c : constr)
        {
            c = Constraints(0, 1, true);
        }
        return constr;
    }
private:
    jsp problem;
    prule rule;
    individuo best;
    int max_seconds;
};

int main(int argc, char** argv){
    ofstream reprule,repin;
    reprule.open("rule.ind");
    repin.open("best.ind");
    // leer instancia y obtener parametros
    for(int i=79;i<=79;i++){
        string file("../../instancias/dmu");
        if(i<10)
            file = file + "0" +to_string(i)+".txt";
        else
            file =file +to_string(i)+".txt";
        jsp problem(file);
        prule rule,update,avg;
        vnsx obj(problem,10);
        de::DifferentialEvolution de(obj, 4, std::time(nullptr));
        de.Optimize(2, reprule, true);
    }
    return 0;
}
/*
        best.load("best80.txt",problem.req);
        best.eval(problem.req);
        rule.init(best);
        locopt = problem.ASGA(&rule);
        update.init(locopt);

        best = problem.local_search(&rule);
        locopt = problem.local_search(&update);

        cout << best.costo()<<" "<<locopt.costo()<<endl;
        cout <<"0 "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.local_search(locopt,make_n7);
        cout <<"n7 "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.local_search(&rule);
        cout <<"pr "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.VNS(rule,60,.01,make_n7);
        cout <<"3 "<< i<<" "<< locopt.costo()<<endl;
        locopt = problem.VNS(rule,120,.01,make_n7);
        cout <<"4 "<< i<<" "<< locopt.costo()<<endl;
    }
*/
