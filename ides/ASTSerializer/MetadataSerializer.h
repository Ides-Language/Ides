//
//  MetadataSerializer.h
//  ides
//
//  Created by Sean Edwards on 1/8/13.
//
//

#ifndef __ides__MetadataSerializer__
#define __ides__MetadataSerializer__

#include <ides/ASTVisitor/ASTVisitor.h>
#include <ides/Types/TypeVisitor.h>

namespace Ides {
namespace AST {
    
    class MetadataSerializer : public Visitor, public Ides::Types::TypeVisitor {
    public:
        
        MetadataSerializer(llvm::LLVMContext& lctx) : lctx(lctx) { }
        virtual ~MetadataSerializer() { }
        
        virtual void Visit(Ides::AST::FunctionDeclaration* ast);
        
        llvm::MDNode* GetMDValue(AST* ast);
    private:
        llvm::MDNode* last;
        
        boost::unordered_map<AST*, llvm::MDNode*> mdnodes;
        
        llvm::LLVMContext& lctx;
    };
    
}
}

#endif /* defined(__ides__MetadataSerializer__) */
