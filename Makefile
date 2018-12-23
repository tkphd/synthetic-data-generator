data-generator: data-generator.cpp
	g++ -O3 -Wall $< -o $@ -lboost_system -lboost_filesystem

.PHONY: clean
clean:
	rm data-generator
