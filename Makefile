data-generator: data-generator.cpp
	g++ -pg -Wall $< -o $@ -lboost_system -lboost_filesystem

.PHONY: clean
clean:
	rm data-generator
