test: test.cpp
	g++  -o test -std=c++11  test.cpp -l pthread 
	
clean:
	rm -f test
