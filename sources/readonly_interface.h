#ifndef _READONLY_H_
#define _READONLY_H_

class IReadOnly {
public:
    IReadOnly() {}
    ~IReadOnly() {}

private:
    IReadOnly(const IReadOnly&) {}
    IReadOnly& operator=(const IReadOnly&) {}
};

#endif  // _READONLY_H_
