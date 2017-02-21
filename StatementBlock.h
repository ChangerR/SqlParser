#ifndef __SQLPARSER_STATEMENT_H
#define __SQLPARSER_STATEMENT_H
#include <vector>
class SingleStatement;

class StatementBlock {
public:
    virtual ~StatementBlock() {}
    
    void push(SingleStatement* stmt);

private:
    std::vector<SingleStatement*> stmts_;
};
#endif