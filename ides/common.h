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

#define BOOST_PP_VARIADICS 1

#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <utility>
extern "C" {
#include <stdint.h>
}
#include <llvm/IR/IRBuilder.h>
#include <llvm/DIBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/system_error.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/preprocessor.hpp>

#include <boost/variant.hpp>

#include <yaml-cpp/yaml.h>

#include <ides/Util/Util.h>
#include <ides/Diagnostics/Diagnostics.h>



#endif // _IDES_COMMON_H_

