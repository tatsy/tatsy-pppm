#ifndef _RANDOM_SEQUENCE_H_
#define _RANDOM_SEQUENCE_H_

#include <vector>

#include "common.h"

class RandomSequence {
private:
    int pos;
    std::vector<double> que;

public:
    RandomSequence()
        : pos(0)
        , que()

    {
    }

    RandomSequence(const RandomSequence& rseq)
        : pos(rseq.pos)
        , que(rseq.que)
    {
    }

    ~RandomSequence()
    {
    }

    RandomSequence& operator=(const RandomSequence& rseq) {
        pos = rseq.pos;
        que = rseq.que;
        return *this;
    }

    void set(int i, double val) {
        Assertion(0 <= i && i < que.size(), "Sample index out of bounds!!");
        que[i] = val;
    }

    void reset() {
        pos = 0;
    }

    void resize(int n) {
        que.resize(n);
    }

    double pop() {
        Assertion(pos < que.size(), "Sequence is empty!!");
        return que[pos++];
    }
};

#endif  // _RANDOM_SEQUENCE_H_
