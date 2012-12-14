#ifndef _IDES_TYPES_H_
#define _IDES_TYPES_H_

#include <llvm/Type.h>

namespace Ides {
namespace Types {

    
    class Type {
    public:
        typedef boost::shared_ptr<Type> Ptr;
        
        virtual llvm::Type GetLLVMType() const = 0;
    };
    
    class ReferenceType : public Type {
        
    };
    
    template<typename T>
    class NumericType : public Type {
        
    };
    
} // namespace Types
} // namespace Ides
#endif // _IDES_TYPES_H_

