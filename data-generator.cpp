/*
 This program is intended to generate random files with a mean size,
 specified in bytes, following a Poisson distribution. It will
 generate random data up to the specified limit.
 */

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <queue>
#include <random>
#include <string>

void generate(const std::string & dirname, const uint64_t & total, const uint64_t & mean,
              std::default_random_engine & mtgen, std::minstd_rand & lcgen,
              std::uniform_int_distribution<char> & char_dist, std::poisson_distribution<uint64_t> & size_dist,
              uint64_t & total_actual, double & mean_actual, double & time_create, std::queue<std::string> & files)
{
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    uint64_t sum = 0;
    uint64_t n = 0;

    while (sum < total) {
        uint64_t size = size_dist(mtgen);
        if (sum + size > total) // always hit the bullseye
            size = total - sum;
        sum += size;
        n++;

        std::string filename = "ffffffff.dat";
        for (int i = 0; i < 8; i++)
            filename[i] = char_dist(mtgen);
        std::ofstream file((dirname + "/" + filename).c_str());
        files.push(filename);

        char* buffer = new char[size];
        for (uint64_t i = 0; i < size-1; i++)
            buffer[i] = char_dist(mtgen);
        buffer[size-1] = '\n';
        file.write(buffer, size);
        file.close();
        delete [] buffer;
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> delta = end - start;
    time_create = delta.count();

    total_actual = sum;
    mean_actual = double(sum) / n;
}

int main(int argc, char* argv[])
{
    const char* valid_opts = "t:m:r:s:hd";
    int o;

    uint64_t tval = 1000000;
    uint64_t sval = time(NULL);
    uint64_t mval = 10000;
    uint64_t rval = 1;
    int      dval = 0;

    while( ( o = getopt (argc, argv, valid_opts) ) != -1 )
    {
        switch(o)
        {
        case 't':
            if (optarg)
                tval = std::atol(optarg);
            break;
        case 'm':
            if (optarg)
                mval = std::atol(optarg) ;
            break;
        case 'r':
            if (optarg)
                rval = std::atol(optarg) ;
            break;
        case 's':
            if (optarg)
                sval = std::atol(optarg) ;
            break;
        case 'h':
            printf("Usage: %s [OPTIONS]\n", argv[0]);
            printf("Options:\n");
            printf("         -t total       total size of dataset (B)\n");
            printf("         -m mean        mean file size (B)\n");
            printf("         -r reps        number of repetitions\n");
            printf("         -s seed        random number generator seed\n");
            printf("         -d             delete data before exiting\n");
            printf("         -h             print this message and exit\n");
            std::exit(0);
        case 'd':
            dval = 1;
            break;
        }
    }

    if (argc == 1) {
        printf("Usage: %s [OPTIONS]\n", argv[0]);
        printf("Options:\n");
        printf("         -t total       total size of dataset (B)\n");
        printf("         -m mean        mean file size (B)\n");
        printf("         -r reps        number of repetitions\n");
        printf("         -s seed        random number generator seed\n");
        printf("         -d             delete data before exiting\n");
        printf("         -h             print this message and exit\n");
        std::exit(1);
    }

    const uint64_t total(tval);
    const uint64_t seed (sval);
    const uint64_t mean (mval);
    const uint8_t  reps (rval);
    const bool     clean(dval);
    std::queue<std::string> paths;
    std::queue<std::string> files;

    std::default_random_engine mtgen (seed);
    std::minstd_rand lcgen (seed);
    std::uniform_int_distribution<char> char_dist(97, 122);
    std::poisson_distribution<uint64_t> size_dist(mean);

    double* means       = new double[reps];
    double* time_create = new double[reps];
    uint64_t* totals    = new uint64_t[reps];

    std::string dirname = "dddddddd";
    for (int j=0; j<8; j++)
        dirname[j] = char_dist(mtgen);
    boost::filesystem::create_directory(dirname);

    for (uint8_t i = 0; i < reps; i++) {
        std::string subname = dirname + "/dddddddd";
        for (uint8_t j=subname.length(); j>subname.length()-8; j--)
            subname[j] = char_dist(mtgen);
        boost::filesystem::create_directory(subname);
        paths.push(subname);

        generate(subname, total, mean,
                 mtgen, lcgen, char_dist, size_dist,
                 totals[i], means[i], time_create[i], files);
    }

    double cavg = 0., mavg = 0.;
    uint64_t tavg = 0;
    for (uint8_t i = 0; i < reps; i++) {
        cavg += time_create[i];
        mavg += means[i];
        tavg += totals[i];
    }
    cavg /= reps;
    mavg /= reps;
    tavg /= reps;

    std::string logname = dirname + ".log";
    printf("Writing summary to %s\n", logname.c_str());
    FILE* logfile = fopen(logname.c_str(), "w");

    fprintf(logfile, "%-12lu %-15f", tavg, mavg);

    double sumsq = 0.;
    for (uint8_t i = 0; i < reps; i++) {
        fprintf(logfile, " %-12f", time_create[i]);
        sumsq += (time_create[i] - cavg) * (time_create[i] - cavg) / reps;
    }

    fprintf(logfile, " %-12f %-12f\n", cavg, std::sqrt(sumsq));
    fflush(logfile);

    if (clean) {
        while (files.size() != 0) {
            std::string file = files.front();
            boost::filesystem::remove_all(file);
            files.pop();
        }
        while (paths.size() != 0) {
            std::string path = paths.front();
            boost::filesystem::remove_all(path);
            paths.pop();
        }
        boost::filesystem::remove_all(dirname);
    }

    delete [] means;
    delete [] time_create;
    delete [] totals;
    fclose(logfile);

    return 0;
}
