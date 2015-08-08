#ifndef _PATH_H_
#define _PATH_H_

#include <string>

namespace path {

    inline std::string getExtention(const std::string& filename) {
        const int pos = filename.find_last_of(".");
        return filename.substr(pos);
    }

}  // namespace path

#endif  // _PATH_H_
