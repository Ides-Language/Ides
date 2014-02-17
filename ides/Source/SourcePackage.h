//
//  SourcePackage.h
//  ides
//
//  Created by Sean Edwards on 12/30/13.
//
//

#ifndef __ides__SourcePackage__
#define __ides__SourcePackage__

#include <ides/common.h>
#include <ides/Source/SourceLocation.h>
#include <yaml-cpp/yaml.h>

namespace Ides {

    class SourcePackage : public Ides::SourceFilesystemLocation {
    public:
        SourcePackage(const Ides::Path& location);
        virtual Ides::Path GetPath() const { return root.GetPath(); }
        const Ides::SourceDirectory& GetRoot() const { return root; }

        template<typename T>
        T GetProperty(llvm::StringRef prop) const {
            return GetPropertyOf<T>(prop, config);
        }

    private:
        template<typename T>
        T GetPropertyOf(llvm::StringRef prop, const YAML::Node& node) const {
            auto split = prop.split('.');
            if (split.first == prop) {
                return config[prop.str()].as<T>();
            }
            return GetPropertyOf<T>(split.second, node[split.first.str()]);
        }
        Ides::SourceDirectory root;
        YAML::Node config;
    };

}

#endif /* defined(__ides__SourcePackage__) */