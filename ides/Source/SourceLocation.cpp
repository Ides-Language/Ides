//
//  SourceLocation.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#include "SourceLocation.h"

Ides::Path Ides::SourceDirectory::GetPath() const {
    return dir->GetPath() / dirname;
}

Ides::Path Ides::SourceFile::GetPath() const {
    return dir->GetPath() / filename;
}

Ides::Graph<Ides::SourceLocation>::One Ides::SourceFile::OffsetToLocation(size_t offset) const {
    return Ides::Graph<Ides::SourceLocation>::One(new Ides::SourceLocation(this->shared_from_this(), offset));
}

void Ides::SourceFile::Open() {
    llvm::MemoryBuffer::getFile(this->ToString(), buffer);
    auto rest = llvm::StringRef(buffer->getBufferStart(), buffer->getBufferSize());

    size_t last_linesep = 0;
    size_t linesep = rest.find_first_of('\n');
    size_t linenum = 1;
    while (linesep != llvm::StringRef::npos) {
        auto sourceline = new Ides::SourceLine(linenum, OffsetToLocation(last_linesep), OffsetToLocation(linesep));
        lines.push_back(Graph<Ides::SourceLine>::One(sourceline));
        last_linesep = linesep;
        linesep = rest.find_first_of('\n');
        ++linenum;
    }
    auto sourceline = new Ides::SourceLine(linenum, OffsetToLocation(last_linesep), OffsetToLocation(rest.size()));
    lines.push_back(Graph<Ides::SourceLine>::One(sourceline));
}

void Ides::SourceFile::Close() {
    lines.clear();
    buffer.reset();
}

const Ides::Graph<Ides::SourceLine>::One& Ides::SourceFile::GetLineForOffset(size_t offset) const {
    for (auto& line : lines) {
        if (line->begin->offset <= offset && line->end->offset > offset) {
            return line;
        }
    }
    throw std::runtime_error("offset out of bounds.");
}
