#ifndef __SQLPARSER_SQLASTVISITOR_H
#define __SQLPARSER_SQLASTVISITOR_H
class CloumnRef;
class Expression;
class ResTarget;
class SelectStatement;

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
    
};
#endif