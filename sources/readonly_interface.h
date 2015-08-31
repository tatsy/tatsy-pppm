#ifndef _READONLY_H_
#define _READONLY_H_

class IReadOnly {
public:
    IReadOnly() {}
    ~IReadOnly() {}

private:
    IReadOnly(const IReadOnly&) = delete;
    IReadOnly& operator=(const IReadOnly&) = delete;
};

#endif  // _READONLY_H_
