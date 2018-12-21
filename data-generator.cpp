/*
 This program is intended to generate random files with a mean size,
 specified in bytes, following a Poisson distribution. It will
 generate random data up to the specified limit.
 */

#include <boost/filesystem.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <string>

void generate(const std::string & dirname, const uint64_t & total, const uint64_t & mean,
              std::default_random_engine & mtgen, std::minstd_rand & lcgen,
              std::uniform_int_distribution<char> & char_dist, std::poisson_distribution<uint64_t> & size_dist,
              uint64_t & total_actual, double & mean_actual, double & elapsed)
{
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    uint64_t sum = 0;
    uint64_t n = 0;
    std::queue<std::string> filenames;

    while (sum < total) {
        uint64_t size = size_dist(mtgen);
        if (sum + size > total) // always hit the bullseye
            size = total - sum;
        sum += size;
        n++;

        std::string filename = "ffffffff.dat";
        for (int i=0; i<8; i++)
            filename[i] = char_dist(mtgen);
        std::ofstream file((dirname + "/" + filename).c_str());
        filenames.push(filename);

        char* buffer = new char[size];
        for (uint64_t i=0; i<size-1; i++)
            buffer[i] = char_dist(mtgen);
        buffer[size-1] = '\n';
        file.write(buffer, size);
        file.close();
        delete [] buffer;
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> delta = end - start;

    total_actual = sum;
    mean_actual = double(sum) / n;
    elapsed = delta.count();

    while (filenames.size() != 0) {
        std::string filename = filenames.front();
        boost::filesystem::remove((dirname + "/" + filename).c_str());
        filenames.pop();
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <total_size> <mean_file_size> <repetitions>" << std::endl;
        std::exit(1);
    }

    const uint64_t total = std::atol(argv[1]);
    const uint64_t mean  = std::atol(argv[2]);
    const uint8_t  reps  = std::atoi(argv[3]);

    std::default_random_engine mtgen (time(NULL));
    std::minstd_rand lcgen (time(NULL));
    std::uniform_int_distribution<char> char_dist(97, 122);
    std::poisson_distribution<uint64_t> size_dist(mean);

    double* elapsed  = new double[reps];
    double* means    = new double[reps];
    uint64_t* totals = new uint64_t[reps];

    for (uint8_t i=0; i<reps; i++) {
        std::string dirname = "dddddddd";
        for (int j=0; j<8; j++)
            dirname[j] = char_dist(mtgen);
        boost::filesystem::create_directory(dirname);

        generate(dirname, total, mean, mtgen, lcgen, char_dist, size_dist, totals[i], means[i], elapsed[i]);

        boost::filesystem::remove(dirname);
    }

    double eavg = 0., mavg = 0.;
    uint64_t tavg = 0;
    for (uint8_t i=0; i<reps; i++) {
        eavg += elapsed[i];
        mavg += means[i];
        tavg += totals[i];
    }
    eavg /= reps;
    mavg /= reps;
    tavg /= reps;

    printf("%-12lu %-15f", tavg, mavg);

    double sumsq = 0.;
    for (uint8_t i=0; i<reps; i++) {
        printf(" %-12f", elapsed[i]);
        sumsq += (elapsed[i] - eavg) * (elapsed[i] - eavg) / reps;
    }

    printf(" %-12f %-12f\n", eavg, std::sqrt(sumsq));

    delete [] elapsed;
    delete [] means;
    delete [] totals;

    return 0;
}
