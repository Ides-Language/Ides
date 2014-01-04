//
//  SourceLocation.h
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#ifndef __ides__SourceLocation__
#define __ides__SourceLocation__

#include <ides/common.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <boost/filesystem.hpp>

namespace Ides {
    using namespace Ides::Util;
    
    struct Source {
    public:
        virtual ~Source() {}
        virtual llvm::StringRef ToString() const = 0;
    };

    struct SourceFile;
    struct SourceDirectory;
    struct SourceLine;
    struct SourceLocation;
    struct SourceRange;

    struct SourceFilesystemLocation : public Source {

        Graph<SourceDirectory>::One dir;

        virtual Ides::Path GetPath() const = 0;

        llvm::StringRef ToString() const {
            return GetPath().string();
        }

    };

    struct SourceDirectory : public SourceFilesystemLocation {
        virtual Ides::Path GetPath() const;

        const std::string dirname;

        Graph<SourceDirectory>::Many dirs;
        Graph<SourceFile>::Many files;

    };

    struct SourceFile : public SourceFilesystemLocation, private std::enable_shared_from_this<SourceFile> {
    public:
        virtual Ides::Path GetPath() const;

        void Open();

        void Close();

        bool IsOpen() const {
            return buffer.get() != NULL;
        }

        llvm::MemoryBuffer& GetBuffer() const {
            return *buffer;
        }

        const Graph<SourceLine>::Many& GetLines() const { return lines; }
        const Graph<SourceLine>::One& GetLineForOffset(size_t offset) const;
    private:
        Graph<SourceLocation>::One OffsetToLocation(size_t offset) const;

        std::string filename;
        llvm::OwningPtr<llvm::MemoryBuffer> buffer;
        Graph<SourceLine>::Many lines;
    };

    struct SourceLocation : public Source {
        SourceLocation(Graph<SourceFile>::One file, size_t offset)
            : file(file), offset(offset) { }

        const Graph<SourceFile>::One file;
        const size_t offset;

        llvm::StringRef ToString() const {
            return "";
        }
    };

    struct SourceRange : public Source {
        SourceRange(Graph<SourceLocation>::One begin, Graph<SourceLocation>::One end)
            : begin(begin), end(end) { assert(begin->offset <= end->offset && begin->file == end->file); }

        const Graph<SourceLocation>::One begin;
        const Graph<SourceLocation>::One end;

        llvm::StringRef ToString() const {
            llvm::MemoryBuffer& buf = begin->file->GetBuffer();
            return llvm::StringRef(buf.getBufferStart() + begin->offset, end->offset - begin->offset);
        }
    };

    struct SourceLine : public SourceRange {
        SourceLine(size_t number, Graph<SourceLocation>::One begin, Graph<SourceLocation>::One end)
            : SourceRange(begin, end), number(number) { }

        const size_t number;

    };

}

inline Ides::SourceRange operator+(const Ides::SourceRange& lhs, const Ides::SourceRange& rhs) {
    return Ides::SourceRange(lhs.begin, rhs.end);
}

#endif /* defined(__ides__SourceLocation__) */
