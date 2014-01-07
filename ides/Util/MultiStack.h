//
//  MultiStack.h
//  ides
//
//  Created by Sean Edwards on 1/6/14.
//
//

#ifndef ides_MultiStack_h
#define ides_MultiStack_h

#include <stack>

namespace Ides {

    template<typename... Args>
    class MultiContext : std::stack<Args>... {
        // MultiContextItem handles all stack operations for us, to ensure the stack stays sane.
        template<typename CT, typename CS>
        friend class MultiContextItem;
    public:
        template<typename A>
        A& Top() const { return ((std::stack<A>*)this)->top(); }
    protected:
        template<typename A> void Pop() { ((std::stack<A>*)this)->pop(); }
        template<typename A> void Push(A& t) { ((std::stack<A>*)this)->push(t); }
    };

    template<typename T, typename S>
    class MultiContextItem {
    public:
        MultiContextItem(T& val, S& stack)
        : stack(stack), val(val)
        {
            stack.template Push<T>(this->val);
        }

        ~MultiContextItem() {
            // Sanity check.
            // We should be the only one with write access to the stack.
            // But evil exists.
            assert(stack.template Top<T>() == val);

            stack.template Pop<T>();
        }

    private:
        MultiContextItem( const MultiContextItem<T, S>& );
        const MultiContextItem<T, S>& operator=( const MultiContextItem<T, S>& );
        S& stack;
        T& val;
    };
}

#define SETCTX_OTHER(arg, self) Ides::MultiContextItem<decltype(arg), decltype(self)> __stack_ctx##__COUNTER__(arg, self);
#define SETCTX(arg) SETCTX_OTHER(arg, *this)

#endif
