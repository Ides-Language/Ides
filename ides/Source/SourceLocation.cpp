//
//  SourceLocation.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#include "SourceLocation.h"

Ides::Path Ides::SourceDirectory::GetPath() const {
    if (dir) return dir->GetPath() / dirname;
    return Ides::Path(dirname);
}

Ides::Path Ides::SourceFile::GetPath() const {
    if (dir) return dir->GetPath() / filename;
    return Ides::Path(filename);
}

void Ides::SourceFile::Open() {
    const auto openPath = this->GetPath();
    auto code = llvm::MemoryBuffer::getFile(openPath.string(), buffer);
    if (code != llvm::error_code::success()) {
        throw std::runtime_error(StringBuilder() << "Could not open " << openPath << ": " << code.message());
    }
    auto rest = llvm::StringRef(buffer->getBufferStart(), buffer->getBufferSize());

    size_t last_linesep = 0;
    size_t linesep = rest.find_first_of('\n');
    size_t linenum = 1;
    while (linesep != llvm::StringRef::npos) {
        auto begin = SourceLocation(this, last_linesep);
        auto end = SourceLocation(this, linesep);
        auto sourceline = new Ides::SourceLine(linenum, begin, end);
        lines.push_back(sourceline);
        last_linesep = linesep + 1;
        linesep = rest.find('\n', last_linesep);
        ++linenum;
    }
    auto sourceline = new Ides::SourceLine(linenum, SourceLocation(this, last_linesep), SourceLocation(this, rest.size()));
    lines.push_back(sourceline);
}

void Ides::SourceFile::Close() {
    lines.clear();
    buffer.reset();
}

const Ides::SourceLine* Ides::SourceFile::GetLineForOffset(size_t offset) const {
    for (auto& line : lines) {
        if (line->begin.offset <= offset && line->begin.offset + line->length >= offset) {
            return line;
        }
    }
    throw std::runtime_error("offset out of bounds.");
}

Ides::SourceDirectory::SourceDirectory(Ides::StringRef dirname)
    : SourceFilesystemLocation(NULL), dirname(dirname)
{
    FillMembers();
}

Ides::SourceDirectory::SourceDirectory(SourceDirectory* dir, Ides::StringRef dirname)
    : SourceFilesystemLocation(dir), dirname(dirname)
{
    FillMembers();
}

void Ides::SourceDirectory::FillMembers()
{
    boost::filesystem::directory_iterator begin(GetPath());
    boost::filesystem::directory_iterator end;

    for (auto i = begin; i != end; ++i) {
        if (boost::filesystem::is_regular_file(*i) && i->path().extension() == ".ides") {
            Ides::SourceFile* file(new Ides::SourceFile(this, i->path().filename().string()));
            files.push_back(file);
            file->Open();
        }

        if (boost::filesystem::is_directory(*i)) {
            Ides::SourceDirectory* file(new Ides::SourceDirectory(this, i->path().filename().string()));
            dirs.push_back(file);
        }
    }
}