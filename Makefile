CXX ?= clang++
CXXFLAGS ?= -O3

dumpnar: *.cc *.hh
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -std=c++17 main.cc -o dumpnar

fmt:
	clang-format -i *.cc *.hh

clean:
	rm -f dumpnar

.PHONY: fmt clean
