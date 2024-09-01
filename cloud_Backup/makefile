.PHONY:test
test:test.cc fileTool.hpp config.hpp
	g++ -o $@ $^ -g -std=c++11 -lpthread -lbundle -lstdc++fs -ljsoncpp
.PHONY:clean
clean:
	rm -f test