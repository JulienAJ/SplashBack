CXX=c++
CXXFLAGS=-O2 -Wall -fvisibility=hidden
LDFLAGS=-lIrrlicht
TARGET=Splash
HOST=$(shell uname)

ifeq ($(HOST),Darwin)
LDFLAGS+=-framework OpenGL -framework Cocoa -framework IOKit
else
LDFLAGS+=-lGL -lX11 -lXxf86vm -lXext
endif

all: main.o Game.o Splash.o
	$(CXX) $^ -o $(TARGET) $(LDFLAGS)

main.o: main.cpp
	$(CXX) -c $^ $(CXXFLAGS)

Game.o: Game.cpp Game.hpp EventReceiver.hpp
	$(CXX) -c $< $(CXXFLAGS)

Splash.o: Splash.cpp Splash.hpp
	$(CXX) -c $< $(CXXFLAGS)

.PHONY: clean mrproper rebuild

clean:
	@rm -f *.o

mrproper: clean
	@rm -rf $(TARGET)

rebuild: clean all
