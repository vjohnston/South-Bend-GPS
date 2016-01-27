CXX=g++
CC=gcc
CXXFLAGS=-std=c++11 -O3

all: route test1 test3 test4 test5a test5b measure

route: route.cpp street_map.cpp street_map.hpp
	$(CXX) $(CXXFLAGS) $< street_map.cpp -o $@

test1: test1.cpp street_map.cpp street_map.hpp
	$(CXX) $(CXXFLAGS) $< street_map.cpp -o $@
test3: test3.cpp street_map.cpp street_map.hpp
	$(CXX) $(CXXFLAGS) $< street_map.cpp -o $@
test4: test4.cpp street_map.cpp street_map.hpp
	$(CXX) $(CXXFLAGS) $< street_map.cpp -o $@
test5a: test5a.cpp street_map.cpp street_map.hpp
	$(CXX) $(CXXFLAGS) $< street_map.cpp -o $@
test5b: test5b.cpp street_map.cpp street_map.hpp
	$(CXX) $(CXXFLAGS) $< street_map.cpp -o $@

measure: measure.c
	$(CC) $< -o $@
clean:
	rm -rf route test1 test3 test4 test5a test5b measure *.exe *.o *.dSYM gmon.out *~

