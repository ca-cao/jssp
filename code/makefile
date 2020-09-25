all: main.cpp jsp.o individuo.o
	@g++ -c jsp.cpp individuo.cpp -g 
	@g++ -c main.cpp -g
	@g++ -o main main.o jsp.o individuo.o

test: test.cpp jsp.o individuo.o
	@g++ -c jsp.cpp individuo.cpp -g 
	@g++ -c test.cpp -g
	@g++ -o test test.o jsp.o individuo.o

best: best.cpp jsp.o individuo.o
	@g++ -c jsp.cpp individuo.cpp -g 
	@g++ -c best.cpp -g
	@g++ -o best best.o jsp.o individuo.o

debug:
	gdb ./main ../instancias/cscmax_20_15_1.txt
clean:
	rm *.o


