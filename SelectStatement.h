#ifndef __SQLPARSER_SELECT_STATEMENT_H
#define __SQLPARSER_SELECT_STATEMENT_H
#include "SingleStatement.h"
#include <vector>

class SelectStatement : public SingleStatement
{
public:
    SelectStatement(){}
    virtual ~SelectStatement(){}

    virtual void accept(SQLASTVisitor visitor) {
        //TODO:fix this accept
    }

    virtual const std::string getSQLType() const {
        return "SELECT";
    }
public:
    std::vector<SQLNode*> * opt_target_list;
};
#endif