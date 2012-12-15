#ifndef _IDES_COMMON_H_
#define _IDES_COMMON_H_

#ifdef WIN32
#ifdef LIBIDES_EXPORTS
#define IDES_EXPORTS __declspec(dllexport)
#else
#define IDES_EXPORTS __declspec(dllimport)
#endif
#else
#define IDES_EXPORTS
#endif

namespace Ides {
    typedef std::string String;
} // namespace Ides

#endif // _IDES_COMMON_H_

