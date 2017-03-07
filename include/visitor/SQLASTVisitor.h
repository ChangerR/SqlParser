#ifndef __SQLPARSER_SQLASTVISITOR_H
#define __SQLPARSER_SQLASTVISITOR_H
class CloumnRef;
class Expression;
class ResTarget;
class SelectStatement;
class SQLString;
class SQLTable;
class StatementBlock;

class SQLASTVisitor{
public:
    virtual ~SQLASTVisitor() {}

    virtual bool visit(CloumnRef* ref) = 0;

    virtual void endVisit(CloumnRef* ref) = 0;

    virtual bool visit(Expression* expr) = 0;

    virtual void endVisit(Expression* expr) = 0;

    virtual bool visit(ResTarget* resTarget) = 0;

    virtual void endVisit(ResTarget* resTarget) = 0;

    virtual bool visit(SelectStatement* select) = 0;

    virtual void endVisit(SelectStatement* select) = 0;

    virtual bool visit(SQLString* string) = 0;

    virtual void endVisit(SQLString* string) = 0;
    
    virtual bool visit(SQLTable* table) = 0;

    virtual void endVisit(SQLTable* table) = 0;

    virtual bool visit(StatementBlock* block) = 0;

    virtual void endVisit(StatementBlock* block) = 0;
};
#endif