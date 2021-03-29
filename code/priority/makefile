all: main.cpp jsp.o individuo.o prule.o
	g++ -c  individuo.cpp jsp.cpp main.cpp prule.cpp -O2
	g++ -o main main.o -fopenmp jsp.o individuo.o prule.o -O2 
profile: main.cpp jsp.o individuo.o prule.o
	g++ -c  individuo.cpp jsp.cpp main.cpp prule.cpp -O2 -pg
	g++ -o main main.o jsp.o individuo.o prule.o -O2 -pg

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


