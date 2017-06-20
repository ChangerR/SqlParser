#ifndef __SQLPARSER_SELECT_STATEMENT_H
#define __SQLPARSER_SELECT_STATEMENT_H
#include "SingleStatement.h"
#include <vector>

class SelectStatement : public SingleStatement
{
  public:
    SelectStatement() {}

    virtual ~SelectStatement()
    {
        if (opt_target_list != NULL)
        {
            for (auto itr = opt_target_list->begin(); itr != opt_target_list->end(); ++itr)
            {
                delete *itr;
            }
            delete opt_target_list;
        }
    }

    virtual void accept(SQLASTVisitor *visitor)
    {
        if (visitor->visit(this))
        {
            for (auto itr = opt_target_list->begin(); itr != opt_target_list->end(); ++itr)
            {
                (*itr)->accept(visitor);
            }
            if (where_clause != NULL)
            {
                where_clause->accept(visitor);
            }
        }
        visitor->endVisit(this);
    }

    virtual const std::string getSQLType() const
    {
        return "SELECT";
    }

    virtual NodeType getNodeType()
    {
        return SELECT_STMT;
    }

  public:
    List *opt_target_list;
    List *from_list;
    SQLNode *where_clause;
};

class SQLSubSelect : public SQLNode
{
  public:
    SQLSubSelect(SelectStatement *stmt, SQLBaseElem *alias) : stmt_(stmt),alias_(alias) {}
    virtual ~SQLSubSelect() {
        if (stmt_) {
            delete stmt_;
            stmt_ = NULL;
        }
        if (alias_) {
            delete alias_;
            alias_ = NULL;
        }
    }

    virtual void accept(SQLASTVisitor *visitor)
    {
        visitor->visit(this);
        visitor->endVisit(this);
    }

    virtual NodeType getNodeType()
    {
        return SUBSELECT;
    }

  public:
    SelectStatement *stmt_;
    SQLBaseElem *alias_;
};
#endif