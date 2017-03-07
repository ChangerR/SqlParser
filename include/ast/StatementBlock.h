#ifndef __SQLPARSER_STATEMENT_H
#define __SQLPARSER_STATEMENT_H
#include <vector>
class SingleStatement;
class SQLASTVisitor;

class StatementBlock {
public:
    virtual ~StatementBlock() {
        for ( auto itr = stmts_.begin(); itr != stmts_.end(); ++ itr){
            delete *itr;
        }
    }
    
    void push(SingleStatement* stmt) {
        stmts_.push_back(stmt);
    }

    void accept(SQLASTVisitor* visitor) {
        if ( visitor->visit(this) ) {
            for (auto itr = stmts_.begin();itr != stmts_.end(); ++itr) {
                (*itr)->accept(visitor);
            }
        }
        visitor->endVisit(this);
    }

    const std::vector<SingleStatement*>& getStmts() const {
        return stmts_;
    }
private:
    std::vector<SingleStatement*> stmts_;
};
#endif