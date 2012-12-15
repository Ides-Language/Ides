//
//  ASTConstantExpression.h
//  ides
//
//  Created by Sean Edwards on 12/14/12.
//
//

#ifndef ides_ASTConstantExpression_h
#define ides_ASTConstantExpression_h

#include <ides/AST/ASTExpression.h>

namespace Ides {
namespace AST {

    class ASTConstantExpression : public AST {
    public:
        virtual ~ASTConstantExpression() { }
    };
    
    
    
    /* Integral constant expression types.
     *
     * Note: constant ints store an unsigned value, because negative constants
     * are handled automatically by the unary - operator.
     */
    
    class ASTConstantIntExpression : public ASTConstantExpression {
    public:
        ASTConstantIntExpression(uint64_t v) : val(v) { }
        virtual ~ASTConstantIntExpression() { }

        virtual Ides::String GetDOT() const;
    private:
        uint64_t val;
    };
    
    class ASTConstantCharExpression : public ASTConstantExpression {
    public:
        ASTConstantCharExpression(uint8_t v) : val(v) { }
        virtual ~ASTConstantCharExpression() { }
        
        virtual Ides::String GetDOT() const;
    private:
        uint8_t val;
    };

    
    
    /* String constant expression types */
    
    class ASTConstantStringExpression : public ASTConstantExpression {
    public:
        ASTConstantStringExpression(const Ides::String& str) : val(str) { }
        virtual ~ASTConstantStringExpression() { }
        
        virtual Ides::String GetDOT() const;
        
    protected:
        Ides::String val;
    };
    
    class ASTConstantCStringExpression : public ASTConstantStringExpression {
    public:
        ASTConstantCStringExpression(const Ides::String& str) : ASTConstantStringExpression(str) { }
        virtual ~ASTConstantCStringExpression() { }
        
        virtual Ides::String GetDOT() const;
    };
    
    class ASTConstantWCStringExpression : public ASTConstantStringExpression {
    public:
        ASTConstantWCStringExpression(const Ides::String& str) : ASTConstantStringExpression(str) { }
        virtual ~ASTConstantWCStringExpression() { }
        
        virtual Ides::String GetDOT() const;
    };
    
    class ASTConstantLCStringExpression : public ASTConstantStringExpression {
    public:
        ASTConstantLCStringExpression(const Ides::String& str) : ASTConstantStringExpression(str) { }
        virtual ~ASTConstantLCStringExpression() { }
        
        virtual Ides::String GetDOT() const;
    };
    
    
    
}; // namespace AST
}; // namespace Ides

#endif
