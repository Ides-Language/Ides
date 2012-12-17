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
#include <llvm/Support/IRBuilder.h>
#include <llvm/Value.h>
#include <llvm/Type.h>
#include <llvm/Module.H>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

namespace Ides {
    typedef std::string String;
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
        
        
        class Graph {
        public:
            virtual ~Graph() { }
        
            virtual Ides::String GetDOT() const = 0;
            virtual Ides::String GetCHeader() const = 0;
        };
    }
} // namespace Ides

#endif // _IDES_COMMON_H_

