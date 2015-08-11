#ifdef _MSC_VER
#pragma once
#endif

#ifndef _XORSHIFT_H_
#define _XORSHIFT_H_

#include <cmath>
#include <climits>
#include <memory>

#include "random_sampler.h"
#include "random_interface.h"
#include "random_sequence.h"

class XorShift : public IRandom {
private:
    unsigned int seed[4];
    const float coeff1;
    const float coeff2;

public:
    explicit XorShift(unsigned int init_seed = 0)
        : coeff1(1.0f / UINT_MAX)
        , coeff2(1.0f / 16777216.0f)
    {
        unsigned int s = init_seed;
        for (int i = 1; i <= 4; i++) {
            seed[i - 1] = s = 1812433253U * (s ^ (s >> 30)) + i;
        }
    }

    unsigned int next() {
        const unsigned int t = seed[0] ^ (seed[0] << 11);
        seed[0] = seed[1];
        seed[1] = seed[2];
        seed[2] = seed[3];
        return seed[3] = (seed[3] ^ (seed[3] >> 19)) ^ (t ^ (t >> 8));
    }

    int nextInt(int n) {
        return std::abs((int)next()) % n;
    }

    double nextReal() {
        return (double)next() * coeff1;
    }

    void request(int n, RandomSequence* rseq) override {
        rseq->resize(n);
        for (int i = 0; i < n; i++) {
            rseq->set(i, nextReal());
        }
        rseq->reset();
    }

    static RandomSampler generateSampler(unsigned int init_seed = 0) {
        RandomSampler rand;
        rand.rng = std::unique_ptr<IRandom>(new XorShift(init_seed));
        return std::move(rand);
    }
};

typedef XorShift Random;

#endif  // _XORSHIFT_H_
