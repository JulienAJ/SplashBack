Splash: main.o Splash.o
	g++ $^ -o $@ 

main.o: main.cpp
	g++ -c $^

Splash.o: Splash.cpp Splash.hpp
	g++ -c $<

clean:
	@rm -fr *.o

mrproper: clean
	@rm Splash
