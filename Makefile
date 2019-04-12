CXXOPT = -O3 -Wall

data-generator: data-generator.cpp
	g++  $(CXXOPT) $< -o $@ -lboost_system -lboost_filesystem

profile: CXXOPT = -O1 -g
profile: data-generator

.PHONY: clean
clean:
	rm data-generator
