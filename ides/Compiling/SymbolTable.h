//
//  SymbolTable.h
//  ides
//
//  Created by Sean Edwards on 1/3/14.
//
//

#ifndef __ides__SymbolTable__
#define __ides__SymbolTable__

#include <ides/common.h>

namespace Ides {

class SemGraph;

template<typename T>
class SymbolTable {
    typedef std::unordered_map<Ides::String, T> SymbolMap;
    typedef Graph<SymbolTable<T>>::One Ptr;

    Ptr New() {
        return Ptr(new SymbolTable<T>());
    }

    Ptr New(const Ptr& parent) {
        return Ptr(new SymbolTable<T>(parent));
    }

    const T& Get(Ides::StringRef str) {
        auto i = symbols.find(str);
        if (i != symbols.end()) {
            return i.second;
        }
        else if (i == symbols.end() && parent) {
            return parent->Get(str);
        }
        throw std::runtime_error("No such symbol.");
    }
private:

    SymbolTable() { }
    SymbolTable(SymbolTable<T>::Ptr parent) : parent(parent) { }

    SymbolTable<T>::Ptr parent;
    SymbolMap symbols;
};

}

#endif /* defined(__ides__SymbolTable__) */
