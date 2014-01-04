//
//  Match.h
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#ifndef ides_Match_h
#define ides_Match_h

#include <functional>
#include <iostream>

namespace {
    template<typename T>
    bool TrueFunc(const T& arg) {
        return true;
    }
}

namespace Ides {
namespace Util {

template<typename Ret, typename... Args>
class MatchCase;

template<typename Ret, typename T, typename... Args>
class MatchCase<Ret, T, Args...> {

    friend class MatchCase<Ret, Args...>;

public:
    template<typename A>
    MatchCase<Ret, A, T, Args...> on(const A& test, const std::function<Ret(A*)>& func) {
        return on<Ret, A, T, Args...>(std::bind(std::equal_to<A>(), test, std::placeholders::_1), func);
    }

    template<typename A>
    MatchCase<Ret, A, T, Args...> on(const std::function<bool(const A&)>& test, const std::function<Ret(A*)>& func) {
        return MatchCase<Ret, A, T, Args...>(test, func, std::move(*this));
    }

    template<typename A>
    MatchCase<Ret, A, T, Args...> on(const std::function<Ret(A*)>& func) {
        return MatchCase<Ret, A, T, Args...>(TrueFunc<A>, func, std::move(*this));
    }

    template<typename A>
    Ret match(A* val) const {
        T* v = dynamic_cast<T*>(val);
        if (v != NULL && test(*v)) {
            return func(v);
        }
        return parent.match(val);
    }

private:

    MatchCase(const std::function<bool(const T&)>& test, const std::function<Ret(T*)>& func, const MatchCase<Ret, Args...>& super)
    : parent(std::move(super)), func(func), test(test) { }

    const MatchCase<Ret, Args...> parent;
    const std::function<Ret(T*)> func;
    const std::function<bool(const T&)> test;
};


template<typename Ret, typename... Args>
class MatchCase {
public:
    template<typename A>
    MatchCase<Ret, A, Args...> on(const std::function<bool(const A&)>& test, const std::function<Ret(A*)>& func) {
        return MatchCase<Ret, A, Args...>(test, func, std::move(*this));
    }

    template<typename A>
    MatchCase<Ret, A, Args...> on(const std::function<Ret(A*)>& func) {
        return MatchCase<Ret, A, Args...>(TrueFunc<A>, func, std::move(*this));
    }

    template<typename A>
    Ret match(A& val) const {
        throw std::runtime_error("No such pattern.");
    }
};

}
}

template<typename Ret = void>
Ides::Util::MatchCase<Ret> Match() {
    return Ides::Util::MatchCase<Ret>();
}

#endif
