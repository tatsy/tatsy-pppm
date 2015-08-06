#ifndef _RANDOM_SEQUENCE_H_
#define _RANDOM_SEQUENCE_H_

#include <cassert>
#include <queue>

#include "random.h"

class RandomSequence {
private:
    std::queue<double> que;

public:
    RandomSequence()
        : que()
    {
    }

    RandomSequence(const RandomSequence& rseq)
        : que(rseq.que)
    {
    }

    ~RandomSequence()
    {
    }

    RandomSequence& operator=(const RandomSequence& rseq) {
        que = rseq.que;
        return *this;
    }

    void add(Random& rand, int n) {
        for (int i = 0; i < n; i++) {
            que.push(rand.nextReal());
        }
    }

    double pop() {
        assert(!que.empty(), "Sequence is empty!!");

        double ret = que.front();
        que.pop();
        return ret;
    }
};

#endif  // _RANDOM_SEQUENCE_H_
