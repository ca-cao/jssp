all: main.cpp jsp.o individuo.o nsga2.o
	g++ -c jsp.cpp individuo.cpp nsga2.cpp -O2
	g++ -c main.cpp -O2
	g++ -o main main.o jsp.o individuo.o nsga2.o -O2

profile: main.cpp jsp.o individuo.o
	g++ -c jsp.cpp individuo.cpp -pg 
	g++ -c main.cpp -pg 
	g++ -o main_profile main.o jsp.o individuo.o -pg 

test: test.cpp jsp.o individuo.o
	g++ -c jsp.cpp individuo.cpp -g 
	g++ -c test.cpp -g
	g++ -o test test.o jsp.o individuo.o

best: best.cpp jsp.o individuo.o
	g++ -c jsp.cpp individuo.cpp -g 
	g++ -c best.cpp -g
	g++ -o best best.o jsp.o individuo.o

debug:
	gdb ./main ../instancias/cscmax_20_15_1.txt
clean:
	rm *.o


