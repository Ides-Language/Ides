//
//  Diagnostics.h
//  ides
//
//  Created by Sean Edwards on 1/4/14.
//
//

#ifndef __ides__Diagnostics__
#define __ides__Diagnostics__

#include <ides/common.h>
#include <boost/format.hpp>

#define MSG(x) Ides::MessageBuilder(#x, ::Ides::x)

namespace Ides {

    enum Severity {
        TRACE,
        INFO,
        DEBUG,
        NOTE,
        WARNING,
        ERROR,
        FATAL,

        DBG_DEFAULT = NOTE
    };

    struct MessageDef {
        Severity sev;
        const char* contents;
    };

    #define IDES_MESSAGE(sev, name, val) const MessageDef name { sev, val }

#include <ides/Diagnostics/Messages.h>

    const char* SeverityToString(Severity sev);

    class CompilerError : public std::runtime_error {
    public:
        CompilerError(const MessageDef& msg, Ides::StringRef what) : std::runtime_error(what), msg(msg) { }

        const MessageDef& msg;

    };

    class MessageBuilder {
    public:
        MessageBuilder(const char* msgid, const MessageDef& message)
            : msgid(msgid), msg(message), fmt(new boost::format(message.contents)) { }

        MessageBuilder(MessageBuilder&& other) = default;

        MessageBuilder(MessageBuilder&) = delete;
        MessageBuilder(const MessageBuilder&) = delete;

        ~MessageBuilder() {
            if (fmt) {
                std::string message = Ides::Util::StringBuilder() << SeverityToString(msg.sev) << "(" << msgid << "): " << *fmt;
                if (msg.sev == FATAL) {
                    std::cerr << message << std::endl;
                    exit(1);
                }
                else if (msg.sev < MessageBuilder::min_except) {
                    if (msg.sev >= min_print)
                        std::cerr << message << std::endl;
                }
                else {
                    throw Ides::CompilerError(msg, message);
                }
            }
        }

        template<typename T>
        MessageBuilder operator%(const T& arg) {
            *fmt % arg;
            return std::move(*this);
        }

        template<typename T>
        MessageBuilder operator<<(const T& arg) {
            *fmt % arg;
            return std::move(*this);
        }

        const char* msgid;
        const MessageDef& msg;
        std::unique_ptr<boost::format> fmt;

        static Severity min_print;
        static Severity min_except;
    };

    IDES_MESSAGE(TRACE, T_TRACEENTER, "%1%");
    IDES_MESSAGE(TRACE, T_TRACELEAVE, "%1%");
    
    class IdesStackTrace {
    public:
        IdesStackTrace(const char* msg) : msg(msg), stack(msg) {
            MSG(T_TRACEENTER) % msg;
        }
        ~IdesStackTrace() {
            MSG(T_TRACELEAVE) % msg;
        }

    private:
        const char* msg;
        llvm::PrettyStackTraceString stack;
    };


}

//#ifdef _DEBUG
    #define DBG(contents) MSG(D_GENERIC) % (std::string)(Ides::Util::StringBuilder() << contents)
    #define INFO(contents) MSG(I_GENERIC) % (std::string)(Ides::Util::StringBuilder() << contents)
    #define SETTRACE(str) Ides::IdesStackTrace __stack_t(__FILE__ ":" ppstr(__LINE__) " - " str);
//#else
//    #define DBG(contents)
//    #define INFO(contents)
//    #define SETTRACE(str)
//#endif

#endif /* defined(__ides__Diagnostics__) */
