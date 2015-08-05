#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <climits>

class XorShift {
private:
    unsigned int seed[4];
    const float coeff1;
    const float coeff2;

public:
    explicit XorShift(const unsigned int init_seed = 0)
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
};

typedef XorShift Random;

#endif  // _RANDOM_H_
