//
//  Header.h
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#ifndef _IDES_UTIL_H_
#define _IDES_UTIL_H_

#ifndef LOG
#define LOG(x)
#endif

#define xppstr(s) #s
#define ppstr(s) xppstr(s)

#include "Singleton.h"
#include "Match.h"

namespace Ides {
    typedef std::string String;
    typedef boost::filesystem::path Path;
    typedef const Ides::String& StringRef;
    typedef Ides::String::const_iterator SourceIterator;

    namespace Util {

        template<typename T>
        struct Tree {
            typedef std::unique_ptr<T> One;
            typedef std::vector<One> Many;
        };

        class StringBuilder {
        public:
            StringBuilder() : buffer(new std::stringstream()) {}

            std::stringstream& GetBuffer() {
                return *buffer;
            }

            operator std::string() const {
                return buffer->str();
            }

        private:
            boost::shared_ptr<std::stringstream> buffer;
        };
    }
} // namespace Ides


template<typename T>
Ides::Util::StringBuilder operator<<(Ides::Util::StringBuilder builder, const T& output) {
    builder.GetBuffer() << output;
    return builder;
}

#endif
