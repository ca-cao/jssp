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
        prule auxrule;
        auxrule.init(inputs,problem.nmaq,problem.njobs);
        return problem.ASGA(&auxrule).costo()*1.0;
        //return problem.VNS(rule,max_seconds,inputs).costo()*1.0;
    }

    unsigned int NumberOfParameters() const override{
        return problem.njobs*problem.nmaq;
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
    ofstream d1,d1_2,d0;
    d1.open("delta1.ind");
    d1_2.open("delta1_2.ind");
    d0.open("delta0.ind");
    // leer instancia y obtener parametros
    for(int i=78;i<=78;i++){
        string file("../../instancias/dmu");
        if(i<10)
            file = file + "0" +to_string(i)+".txt";
        else
            file =file +to_string(i)+".txt";
        jsp problem(file);
        prule rule,update,avg;
        problem.VNS(rule,300);
    }
    return 0;
}
/*
        rule.rand_init(problem.req,123);
        cout << problem.local_search(&rule,0).costo()<<endl;
        rule.show(d0);
        cout << problem.local_search(&rule,.5).costo()<<endl;
        rule.show(d1_2);
        cout << problem.local_search(&rule,1).costo()<<endl;
        rule.show(d1);
        vnsx obj(problem,1);
        de::DifferentialEvolution de(obj, 300, std::time(nullptr));
        de.Optimize(100);
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
