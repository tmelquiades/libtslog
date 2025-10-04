CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Wall -Wextra

all: server client

server: server.o tslog.o
	$(CXX) $(CXXFLAGS) -o $@ $^

client: client.o
	$(CXX) $(CXXFLAGS) -o $@ $^

server.o: server.cpp tslog.h
	$(CXX) $(CXXFLAGS) -c server.cpp

client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c client.cpp

tslog.o: tslog.cpp tslog.h
	$(CXX) $(CXXFLAGS) -c tslog.cpp

clean:
	rm -f *.o server client server.log

.PHONY: all clean