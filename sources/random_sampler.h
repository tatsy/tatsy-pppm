#ifdef _MSC_VER
#pragma once
#endif

#ifndef _RANDOM_SAMPLER_H_
#define _RANDOM_SAMPLER_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef RANDOM_SAMPLER_EXPORT
        #define RANDOM_SAMPLER_DLL __declspec(dllexport)
    #else
        #define RANDOM_SAMPLER_DLL __declspec(dllimport)
    #endif
#else
    #define RANDOM_SAMPLER_DLL
#endif

#include <memory>
#include "random_interface.h"

enum RandomSamplerType {
    RANDOM_SAMPLER_PSEUDO_RANDOM,
    RANDOM_SAMPLER_QUASI_MONTE_CARLO
};

class RANDOM_SAMPLER_DLL RandomSampler {
private:
    std::unique_ptr<IRandom> rng;

public:
    RandomSampler()
        : rng()
    {
    }

    ~RandomSampler() 
    {
    }

    void request(int n, RandomSequence* rseq) {
        if (rng.get() != NULL) {
            rng->request(n, rseq);
        }
    }

    RandomSampler(RandomSampler&& sampler)
        : rng(std::move(sampler.rng))
    {
    }

    RandomSampler& operator=(RandomSampler&& sampler) {
        this->rng = std::move(sampler.rng);
        return *this;
    }

private:
    RandomSampler(const RandomSampler&) = delete;
    RandomSampler& operator=(const RandomSampler&) = delete;

    friend class XorShift;
    friend class Halton;
};

#endif  // _RANDOM_SAMPLER_H_
