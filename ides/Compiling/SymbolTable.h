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
#include <ides/Lang.h>

namespace Ides {

    class SemGraph;

    template<typename T>
    class SymbolTable {
    public:
        typedef std::unordered_map<Ides::String, T> SymbolMap;
        typedef SymbolTable<T>* Ptr;

        SymbolTable() : parent(NULL) { }
        SymbolTable(SymbolTable<T>::Ptr parent) : parent(parent) { }

        void InsertSymbol(const Ides::StringRef name, const T& val) {
            symbols.insert(std::make_pair(name, val));
        }

        T& GetSymbol(const llvm::StringRef str) {
            auto i = symbols.find(str);
            if (i != symbols.end()) {
                return i->second;
            }
            else if (i == symbols.end() && parent) {
                return parent->GetSymbol(str);
            }
            throw std::runtime_error("No such symbol.");
        }
    protected:
        void SetParentSymbolTable(SymbolTable<T>::Ptr parent) {
            this->parent = parent;
        }
    private:

        SymbolTable<T>::Ptr parent;
        SymbolMap symbols;
    };

    template<typename T>
    class VisibilitySymbolTable {
    public:
        VisibilitySymbolTable() : pubSyms(), privSyms(pubSyms) { }
        virtual ~VisibilitySymbolTable() {}

        virtual void InsertSymbol(const Ides::StringRef name, const T& val) {
        }

        virtual T& GetSymbol(const llvm::StringRef str) {
        }
    protected:
        void SetParentSymbolTable(typename SymbolTable<T>::Ptr parent) {
            this->pubSyms->SetParentSymbolTable(parent);
        }
    private:
        SymbolTable<T> pubSyms;
        SymbolTable<T> privSyms;
    };

}

#endif /* defined(__ides__SymbolTable__) */
