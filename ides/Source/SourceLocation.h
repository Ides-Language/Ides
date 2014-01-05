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
        SourceFilesystemLocation(SourceDirectory* dir) : dir(dir) {}

        const SourceDirectory* dir;

        virtual Ides::Path GetPath() const = 0;

        llvm::StringRef ToString() const {
            return GetPath().string();
        }

    };

    struct SourceDirectory : public SourceFilesystemLocation {
        SourceDirectory(Ides::StringRef dirname);
        SourceDirectory(SourceDirectory* dir, Ides::StringRef dirname);
        virtual Ides::Path GetPath() const;

        const Ides::String dirname;

        const std::vector<SourceDirectory*>& GetDirs() const { return dirs; }
        const std::vector<SourceFile*>& GetFiles() const { return files; }

    private:

        std::vector<SourceDirectory*> dirs;
        std::vector<SourceFile*> files;

        void FillMembers();

    };

    struct SourceFile : public SourceFilesystemLocation, private boost::noncopyable {
    public:
        SourceFile(SourceDirectory* dir, Ides::StringRef name)
            : SourceFilesystemLocation(dir), filename(name) {}

        virtual Ides::Path GetPath() const;

        void Open();

        void Close();

        bool IsOpen() const {
            return buffer.get() != NULL;
        }

        llvm::MemoryBuffer& GetBuffer() const {
            return *buffer;
        }

        const std::vector<SourceLine*>& GetLines() const { return lines; }
        const SourceLine* GetLineForOffset(size_t offset) const;
    private:
        SourceLocation* OffsetToLocation(size_t offset) const;

        std::string filename;
        llvm::OwningPtr<llvm::MemoryBuffer> buffer;
        std::vector<SourceLine*> lines;
    };

    struct SourceLocation : public Source {
        SourceLocation(const SourceFile* file, size_t offset)
            : file(file), offset(offset) { }

        const SourceFile* file;
        const size_t offset;

        llvm::StringRef ToString() const {
            return "";
        }
    };

    struct SourceRange : public Source {
        SourceRange(SourceLocation* begin, SourceLocation* end)
            : begin(begin), end(end) { assert(begin->offset <= end->offset && begin->file == end->file); }

        const SourceLocation* begin;
        const SourceLocation* end;

        llvm::StringRef ToString() const {
            llvm::MemoryBuffer& buf = begin->file->GetBuffer();
            return llvm::StringRef(buf.getBufferStart() + begin->offset, end->offset - begin->offset);
        }
    };

    struct SourceLine : public SourceRange {
        SourceLine(size_t number, SourceLocation* begin, SourceLocation* end)
            : SourceRange(begin, end), number(number) { }

        const size_t number;

    };

}

#endif /* defined(__ides__SourceLocation__) */
