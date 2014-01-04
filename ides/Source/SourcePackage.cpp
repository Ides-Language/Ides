//
//  SourcePackage.cpp
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#include "SourcePackage.h"

Ides::SourcePackage::SourcePackage(const Ides::Path& location) : path(location),
    config(YAML::LoadFile((location / "build.yaml").string()))
{

}
