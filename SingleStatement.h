#ifndef __SQLPARSER_SINGLE_STATEMENT_H
#define __SQLPARSER_SINGLE_STATEMENT_H
#include <string>
#include "SQLNode.h"
class SQLASTVisitor;

class SingleStatement :public SQLNode {

public:
    virtual ~SingleStatement() {}

    virtual const std::string getSQLType() const = 0;    
};
#endif