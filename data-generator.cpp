/*
 This program is intended to generate random files with a mean size,
 specified in bytes, following a Poisson distribution. It will
 generate random data up to the specified limit.
 */

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <total_size> <mean_file_size>" << std::endl;
        std::exit(1);
    }

    const unsigned int total = std::atoi(argv[1]);
    const unsigned int mean = std::atoi(argv[2]);
    const unsigned int unity = 1;

    std::minstd_rand0 easy_gen;
    std::default_random_engine hard_gen;

    std::uniform_int_distribution<char> name_dist(97,122);
    std::uniform_int_distribution<char> data_dist(32,126);
    std::poisson_distribution<unsigned int> size_dist(mean);

    std::ofstream summary("summary.log");

    unsigned int sum = 0;
    while (sum < total) {
        std::string name = "aaaaaaaa.dat";
        for (int i=0; i<8; i++)
            name[i] = name_dist(easy_gen);
        std::ofstream file(name.c_str());

        unsigned int size = sum + 1;
        while (size > sum) // make sure we don't overstep
            size = size_dist(hard_gen);
        size = std::max(unity, size);

        if (sum + size >= total) // always hit the bullseye
            size = std::max(unity, total - sum);
        sum += size;

        summary << size << '\t' << sum << '\n';

        for (unsigned int i=0; i<size; i++)
            file << data_dist(easy_gen);
        file << std::endl;
        file.close();
    }

    summary.close();

    std::cout << "Generated " << sum << " B." << std::endl;

    return 0;
}
