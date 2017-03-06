#ifndef __SQLPARSER_SELECT_STATEMENT_H
#define __SQLPARSER_SELECT_STATEMENT_H
#include "SingleStatement.h"
#include <vector>

class SelectStatement : public SingleStatement
{
public:
    SelectStatement(){}
    
    virtual ~SelectStatement() {
        if ( opt_target_list != NULL ) {
            for ( auto itr = opt_target_list->begin(); itr != opt_target_list->end(); ++itr ) {
                delete *itr;
            }
            delete opt_target_list;
        }
    }
    
    virtual void accept(SQLASTVisitor* visitor) {
        if ( visitor->visit(this) ) {
            for ( auto itr = opt_target_list->begin() ; itr != opt_target_list->end() ; ++itr) {
                (*itr)->accept(visitor);
            }
        }
        visitor->endVisit(this);
    }

    virtual const std::string getSQLType() const {
        return "SELECT";
    }

    virtual NodeType getNodeType() {
        return SELECT_STMT;
    }
public:
    List * opt_target_list;
    List * from_list;
};
#endif