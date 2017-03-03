all:	pi_pool pi

pi_pool:	pi_pool.o
	g++ -o pi_pool pi_pool.o -lpthread

pi_pool.o:	pi_pool.cpp
	g++ -c pi_pool.cpp -lpthread

pi:	pi.o
	g++ -o pi pi.o -lpthread

pi.o:	pi.cpp
	g++ -c pi.cpp -lpthread

clean:
	rm -f pi_pool pi *.o
