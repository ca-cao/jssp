all: main.cpp jsp.o individuo.o nsga2.o
	g++ -c jsp.cpp individuo.cpp nsga2.cpp -O2
	g++ -c main.cpp -O2
	g++ -o main main.o jsp.o individuo.o nsga2.o -O2

