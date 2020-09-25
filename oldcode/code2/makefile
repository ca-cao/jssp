all: main.cpp jsp.o individuo.o
	@g++ -c jsp.cpp individuo.cpp -g 
	@g++ -c main.cpp -g
	@g++ -o main main.o jsp.o individuo.o

test:
	./main ../instancias/cscmax_20_15_1.txt test.txt

debug:
	gdb ./main ../instancias/cscmax_20_15_1.txt
clean:
	rm *.o


