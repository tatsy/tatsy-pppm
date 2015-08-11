#ifndef _HATLON_H_
#define _HATLON_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef HALTON_EXPORT
        #define HALTON_DLL __declspec(dllexport)
    #else
        #define HALTON_DLL __declspec(dllimport)
    #endif
#else
    #define HALTON_DLL 
#endif

#include <vector>

#include "random_sequence.h"


class HALTON_DLL Halton {
private:
    static const int nPrimes = 1000;
    int  dims;
    long long usedSamples;
    std::vector<int> bases;
    std::vector<long long> permute;

public:
    // Constructor
    // @param[in] dim: dimension of halton sequence
    // @param[in] isPermute: whether or not use permuted Halton
    // @param[in] seed: if permuted Halton is used, it is a seed for random for the permutation
    explicit Halton(int dim = 200, bool isPermute = true, unsigned int seed = -1);
    Halton(const Halton& hal);
    ~Halton();

    Halton& operator=(const Halton& hal);

    void request(RandomSequence& rseq, int n);

private:
    double radicalInverse(long long n, int base, const long long* p) const;
};

#endif  // _HATLON_H_
