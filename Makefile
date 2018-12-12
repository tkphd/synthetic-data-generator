data-generator: data-generator.cpp
	g++ -O2 -Wall $< -o $@ -lboost_system -lboost_filesystem

.PHONY: clean
clean:
	rm data-generator
