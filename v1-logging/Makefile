CXX = g++
CXXFLAGS = -std=c++17 -02 -pthread -Wall -Wextra

all: test_logger

test_logger: tslog.o main_logger_test.O
        $(CXX) $(CXXFLAGS) -o $@ $^

tslog.o: tslog.cpp tslog.h
        $(CXX) $(CXXFLAGS) -c tslog.cpp

main_logger_test.o: main_logger_test.cpp tslog.h
        $(CXX) $(CXXFLAGS) -c main_logger_test.cpp

clean:
        rm -f *.o test_logger test_log.txt

.PHONY: all clean