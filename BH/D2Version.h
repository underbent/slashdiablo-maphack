#pragma once

#ifndef _D2VERSION_H
#define _D2VERSION_H

#include <string>

enum VersionID {
    INVALID = -1,
    VERSION_113c = 0,
    VERSION_113d
};

namespace D2Version {
    extern VersionID versionID;
    VersionID GetGameVersionID();
    void Init();
    std::string GetGameVersionString();
};

#endif
