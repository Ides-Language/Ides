#ifndef _IDES_AST_NODE_H_
#define _IDES_AST_NODE_H_

#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/unordered_map.hpp>

#include <ides/common.h>
#include <ides/Types/Type.h>

#include <ides/Parsing/ParseContext.h>
#include <ides/ASTVisitor/ASTVisitor.h>

#include <ides/AST/ASTContext.h>

namespace Ides {
namespace AST {
    class AST;
    class ASTIdentifier;
    class ASTVariableDeclaration;
    
    typedef Ides::Parsing::ParseContext ParseContext;
    
    enum Specifier {
        // Low order bits = visibility
        PUBLIC = 0,
        PROTECTED = 1,
        INTERNAL = 2,
        PRIVATE = 3,
        
        MASK_VISIBILITY = 7,
        
        EXTERN = 8,
        CONST = 16,
    };
    
    class AST {
    public:
        
        AST() { }
        virtual ~AST() { }
        
        virtual void Accept(Visitor* v) = 0;
        
        clang::SourceRange exprloc;
    };
    
    class Token : public AST {
    public:
        Token(llvm::StringRef name) : name(name) {}
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        const Ides::String& operator*() { return name; }
        
    private:
        const Ides::String name;
    };
    
    typedef std::list<AST*> ASTList;
    
    class Type : public AST {
    public:
        Type() : isConst(false) { }
        virtual ~Type() { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) = 0;
        
        void SetConst(bool constness) { isConst = constness; }
        bool isConst;
    };
    
    template<typename T>
    class BuiltinType : public Type {
    public:
        virtual void Accept(Visitor* v) { v->Visit(this); }
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return T::GetSingletonPtr();
        }
        
        const Ides::Types::Type& operator*() { return T::GetSingleton(); }
        const Ides::Types::Type& operator->() { return T::GetSingleton(); }
    };
    
    typedef BuiltinType<Ides::Types::VoidType>       VoidType;
    typedef BuiltinType<Ides::Types::UnitType>       UnitType;
    
    typedef BuiltinType<Ides::Types::Integer1Type>   BoolType;
    
    typedef BuiltinType<Ides::Types::Integer8Type>   Integer8Type;
    typedef BuiltinType<Ides::Types::UInteger8Type>  UInteger8Type;
    typedef BuiltinType<Ides::Types::Integer16Type>  Integer16Type;
    typedef BuiltinType<Ides::Types::UInteger16Type> UInteger16Type;
    typedef BuiltinType<Ides::Types::Integer32Type>  Integer32Type;
    typedef BuiltinType<Ides::Types::UInteger32Type> UInteger32Type;
    typedef BuiltinType<Ides::Types::Integer64Type>  Integer64Type;
    typedef BuiltinType<Ides::Types::UInteger64Type> UInteger64Type;
    
    typedef BuiltinType<Ides::Types::Float32Type>    Float32Type;
    typedef BuiltinType<Ides::Types::Float64Type>    Float64Type;
    
    class PtrType : public Type {
    public:
        PtrType(Type* type) : basetype(type) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) {
            return Ides::Types::PointerType::Get(basetype->GetType(ctx));
        }
        
    private:
        boost::scoped_ptr<Type> basetype;
    };
    
    typedef std::list<boost::shared_ptr<Type> > TypeList;
    
    class FunctionType : public Type {
    public:
        FunctionType(TypeList* argtypes, Type* rettype) : rettype(rettype)
        {
            if (argtypes != NULL) {
                std::copy(argtypes->begin(), argtypes->end(), std::back_inserter(this->argtypes));
                delete argtypes;
            }
            
        }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx);
        
        Type* rettype;
        TypeList argtypes;
    };
    
    class TypeName : public Type {
    public:
        TypeName (Token* name) : name(name) { }
        virtual void Accept(Visitor* v) { v->Visit(this); }
        
        virtual const Ides::Types::Type* GetType(ASTContext& ctx) { return NULL; }
        
        boost::scoped_ptr<Token> name;
        
    };
    
} // namespace AST
} // namespace Ides


#endif // _IDES_AST_NODE_H_

