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

#include <string>
#include <cassert>
#include <iostream>
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
#include <llvm/Support/PrettyStackTrace.h>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <clang/Basic/SourceLocation.h>

#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>

#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/DiagnosticIDs.h>

#include <ides/Diagnostics/Diagnostics.h>

//#define LOG(x) std::cerr << "Debug: " << x << std::endl;

#ifndef LOG
#define LOG(x)
#endif

#define xppstr(s) #s
#define ppstr(s) xppstr(s)

#define SETTRACE(str) llvm::PrettyStackTraceString __stack_t(__FILE__ ":" ppstr(__LINE__) " - " str);

namespace Ides {
    typedef std::string String;
    typedef const Ides::String& StringRef;
    typedef Ides::String::const_iterator SourceIterator;
    
    namespace Util {
        
        // Singleton implementation from Ogre3D project.
        // http://ogre3d.org
        template <typename T>
        class Singleton
        {
        private:
            /** \brief Explicit private copy constructor. This is a forbidden operation.*/
            Singleton(const Singleton<T> &);
            
            /** \brief Private operator= . This is a forbidden operation. */
            Singleton& operator=(const Singleton<T> &);
            
        protected:
            
            static T* msSingleton;
            
        public:
            Singleton( void )
            {
                assert( !msSingleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200
                int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
                msSingleton = (T*)((int)this + offset);
#else
                msSingleton = static_cast< T* >( this );
#endif
            }
            ~Singleton( void )
            {  assert( msSingleton );  msSingleton = 0;  }
            static T& GetSingleton( void )
            {       assert( msSingleton );  return ( *msSingleton ); }
            static T* GetSingletonPtr( void )
            { return msSingleton; }
        };
        
        class DiagnosticsError : public std::runtime_error {
        public:
            DiagnosticsError(clang::DiagnosticsEngine& diags, Ides::Diagnostics::DiagIDs diagid, const clang::SourceRange& loc) :
            std::runtime_error("code error"), loc(loc), innerException(),
            builder(new clang::DiagnosticBuilder(Ides::Diagnostics::Diag(diags, diagid, loc.getBegin()))) { }
            
            DiagnosticsError(clang::DiagnosticsEngine& diags, const clang::SourceRange& loc, const DiagnosticsError& inner) :
            std::runtime_error("code error"), innerException(new DiagnosticsError(inner)),
            builder(new clang::DiagnosticBuilder(Ides::Diagnostics::Diag(diags, Ides::Diagnostics::NOTE_FROM, loc.getBegin()))) { }
            
            ~DiagnosticsError() throw() {}
            
            const clang::SourceRange& GetLocation() const { return loc; }
            const DiagnosticsError& GetInnerException() const { return *innerException; }
            
            bool HasInnerException() const { return innerException != NULL; }
            
            clang::DiagnosticBuilder& GetBuilder() { return *builder; }
        private:
            const clang::SourceRange loc;
            boost::shared_ptr<DiagnosticsError> innerException;
            
            boost::shared_ptr<clang::DiagnosticBuilder> builder;
        };
    }
} // namespace Ides



template<typename T>
Ides::Util::DiagnosticsError operator<<(Ides::Util::DiagnosticsError err, const T& output) {
    err.GetBuilder() << output;
    return err;
}

#endif // _IDES_COMMON_H_

