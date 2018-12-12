/*
 This program is intended to generate random files with a mean size,
 specified in bytes, following a Poisson distribution. It will
 generate random data up to the specified limit.
 */

#include <boost/filesystem.hpp>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

double generate(const std::string & dirname, const unsigned & total, const unsigned & mean,
                std::default_random_engine & mtgen, std::minstd_rand & lcgen,
                std::uniform_int_distribution<char> & char_dist, std::poisson_distribution<unsigned> & size_dist)
{
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    unsigned sum = 0;

    while (sum < total) {
        unsigned size = size_dist(mtgen);
        if (sum + size > total) // always hit the bullseye
            size = total - sum;
        sum += size;

        std::string filename = "ffffffff.dat";
        for (int i=0; i<8; i++)
            filename[i] = char_dist(mtgen);
        std::ofstream file((dirname + "/" + filename).c_str());

        char* buffer = new char[size];
        for (unsigned i=0; i<size-1; i++)
            buffer[i] = char_dist(mtgen);
        buffer[size-1] = '\n';
        file.write(buffer, size);
        file.close();
        delete [] buffer;
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    return elapsed.count();
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <total_size> <mean_file_size> <repetitions>" << std::endl;
        std::exit(1);
    }

    const unsigned total = std::atoi(argv[1]);
    const unsigned mean  = std::atoi(argv[2]);
    const unsigned reps  = std::atoi(argv[3]);

    std::default_random_engine mtgen (time(NULL));
    std::minstd_rand lcgen (time(NULL));
    std::uniform_int_distribution<char> char_dist(97, 122);
    std::poisson_distribution<unsigned> size_dist(mean);

    printf("%-12u %-9u", total, mean);

    double* elapsed = new double[reps];
    double sum = 0.;

    for (unsigned i=0; i<reps; i++) {
        std::string dirname = "dddddddd";
        for (int j=0; j<8; j++)
            dirname[j] = char_dist(mtgen);
        boost::filesystem::create_directory(dirname);

        elapsed[i] = generate(dirname, total, mean, mtgen, lcgen, char_dist, size_dist);

        boost::filesystem::remove_all(dirname);

        printf(" %f", elapsed[i]);

        sum += elapsed[i];
    }

    const double avg = sum / reps;
    sum = 0.;
    for (unsigned i=0; i<reps; i++)
        sum += (elapsed[i] - avg) * (elapsed[i] - avg) / reps;

    printf(" %f %f\n", avg, std::sqrt(sum));

    delete [] elapsed;

    return 0;
}
