//
//  SourcePackage.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#include "SourcePackage.h"

Ides::SourcePackage::SourcePackage(const Ides::Path& location) :
    SourceFilesystemLocation(NULL),
    root(location.string()),
    config(YAML::LoadFile((location / "ides.yaml").string()))
{

}
