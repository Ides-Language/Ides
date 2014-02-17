//
//  Diagnostics.cpp
//  ides
//
//  Created by Sean Edwards on 1/4/14.
//
//

#include "Diagnostics.h"

namespace Ides {
    Severity MessageBuilder::min_print = DBG_DEFAULT;
    Severity MessageBuilder::min_except = ERROR;
    
    const char* SeverityToString(Severity sev) {
        switch(sev) {
            case TRACE: return "TRACE";
            case INFO: return "INFO";
            case DEBUG: return "DEBUG";
            case LINT: return "LINT";
            case NOTE: return "NOTE";
            case WARNING: return "WARN";
            case ERROR: return "ERROR";
            case FATAL: return "FATAL";
        }
        return "?";
    }
}
