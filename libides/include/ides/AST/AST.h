#ifndef _IDES_AST_NODE_H_
#define _IDES_AST_NODE_H_

#include <boost/shared_ptr.hpp>
namespace Ides {
namespace AST {
    
class AST {
public:
    typedef boost::shared_ptr<AST> Ptr;
};
    
} // namespace AST
} // namespace Ides

#endif // _IDES_AST_NODE_H_

