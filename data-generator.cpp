/*
 This program is intended to generate random files with a mean size,
 specified in bytes, following a Poisson distribution. It will
 generate random data up to the specified limit.
 */

#include <boost/filesystem.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
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

    std::cout << "Generating " << total << " B of data with files of mean size " << mean << " B." << std::endl;

    std::default_random_engine mtgen (time(NULL));
    std::minstd_rand lcgen (time(NULL));

    std::uniform_int_distribution<char> char_dist(97, 122);
    std::poisson_distribution<unsigned int> size_dist(mean);

    std::ofstream summary("summary.log");
    boost::filesystem::create_directory("data");

    unsigned int sum = 0;
    unsigned int count = 0;
    std::string dirname = "data/dddddddd/";

    while (sum < total) {
        unsigned int size = size_dist(mtgen);
        size = std::max(unity, size);

        if (sum + size > total) // always hit the bullseye
            size = std::max(unity, total - sum);
        sum += size;
        summary << size << '\t' << sum << '\n';

        if (count % 10000 == 0)
            for (int i=5; i<13; i++)
                dirname[i] = char_dist(mtgen);
        boost::filesystem::create_directory(dirname);

        std::string filename = dirname + "ffffffff.dat";
        for (int i=14; i<22; i++)
            filename[i] = char_dist(mtgen);
        std::ofstream file(filename.c_str());

        for (unsigned int i=0; i<size; i++)
            file << char_dist(mtgen);
        file << std::endl;
        file.close();

        count++;
    }

    summary.close();

    std::cout << "Generated " << sum << " B." << std::endl;

    return 0;
}
