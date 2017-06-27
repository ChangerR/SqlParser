#ifndef __SQLPARSER_SQLNODE_H
#define __SQLPARSER_SQLNODE_H
#include <vector>
#include "allocator.h"
#include "visitor/SQLASTVisitor.h"

class SQLNode {
public:
    enum NodeType {
        EXPRESSION = 0,
        RESTARGET,
        COLUMNREF,
        SELECT_STMT,
        BASE_ELEM,
        TABLE,
        SUBSELECT,
        JOIN_EXPR,
    };

    virtual ~SQLNode() {}

    virtual void accept(SQLASTVisitor* visitor) = 0;

    virtual NodeType getNodeType() = 0;
};

class SQLBaseElem :public SQLNode {
public:
    enum Type {
        BASE_STRING = 0,
        BASE_QUOTE_STRING,
        BASE_DQUOTE_STRING,
        BASE_INT,
        BASE_FLOAT,
        BASE_UKNOWN,
    };
    struct Elem {
        Type type;
        union {
            char* string;
            int ival;
            float fval;
        } val;
    };

    explicit SQLBaseElem(char* str,Type type = BASE_STRING) {
        switch(type) {
            case BASE_STRING:
            case BASE_QUOTE_STRING:
            case BASE_DQUOTE_STRING:
                element.type = type; 
                element.val.string = str;
                break;
            default:
                element.type = BASE_UKNOWN;
                break;
        }
         
    }

    SQLBaseElem(int ival) {
        element.type = BASE_INT;
        element.val.ival = ival;
    }

    SQLBaseElem(float fval) {
        element.type = BASE_FLOAT;
        element.val.fval = fval;
    }

    virtual NodeType getNodeType() {
        return BASE_ELEM;
    }

    virtual ~SQLBaseElem() {
        switch (element.type) {
            case BASE_STRING:
            case BASE_QUOTE_STRING:
            case BASE_DQUOTE_STRING:
                if (element.val.string != NULL) {
                    Allocator::free(element.val.string);
                }
                break;
            default:
                break;
        }
       
    }

    virtual void accept(SQLASTVisitor* visitor) {
        visitor->visit(this);
        visitor->endVisit(this);
    }
public:
    Elem element;
private:
    SQLBaseElem() {}
};

class SQLTable : public SQLNode {
public:
    SQLTable(SQLBaseElem* schema,SQLBaseElem* table) :schema_(schema),table_(table),alias_(NULL){}

    virtual ~SQLTable() {
        if ( schema_ ) {
            delete schema_;
        }
        if ( table_ ) {
            delete table_;
        }
        if ( alias_ ) {
            delete alias_;
        }
    }

     virtual void accept(SQLASTVisitor* visitor) {
        visitor->visit(this);
        visitor->endVisit(this);
    }

    virtual NodeType getNodeType() {
        return TABLE;
    }

public:
    SQLBaseElem* schema_;
    SQLBaseElem* table_;
    SQLBaseElem* alias_;
};


class CloumnRef : public SQLNode {
public:
    CloumnRef() {}

    virtual ~CloumnRef() {
        for( std::vector<SQLBaseElem*>::iterator itr = fields.begin() ; itr != fields.end() ; ++itr) {
            if ( *itr != NULL ) {
                delete *itr;
            }
        }
    }

    virtual void accept(SQLASTVisitor* visitor) {
        visitor->visit(this);
        visitor->endVisit(this);
    }

    virtual NodeType getNodeType() {
        return COLUMNREF;
    }
public:
    std::vector<SQLBaseElem*> fields;
};

class Expression : public SQLNode {
public:
    enum Kind {
        AEXPR_OP,
        AND_EXPR,
        OR_EXPR
    };

    Expression() : name(NULL),lexpr(NULL),rexpr(NULL) {
       kind = AEXPR_OP;
    }

    Expression(Kind kind,const char* op,SQLNode* lexpr,SQLNode* rexpr) :
        kind(kind),name(op),lexpr(lexpr),rexpr(rexpr) {}

    virtual ~Expression(){
        if ( lexpr != NULL ) {
            delete lexpr;
        }
        if ( rexpr != NULL ) {
            delete rexpr;
        }
    }

    virtual void accept(SQLASTVisitor* visitor) {
        if ( visitor->visit(this) ) {
            lexpr->accept(visitor);
            rexpr->accept(visitor);
        }
        visitor->endVisit(this);
    }

    virtual NodeType getNodeType() {
        return EXPRESSION;
    }
    
public:
    Kind kind;
    const char* name;
    SQLNode* lexpr;
    SQLNode* rexpr;
};

class JoinExpr : public SQLNode {
public:
    enum JOIN_TYPE{
        INNER_JOIN = 0,
        CROSS_JOIN,
        LEFT_JOIN,
        RIGHT_JOIN
    };
    JoinExpr(JOIN_TYPE type):larg_(NULL),rarg_(NULL),isNatural_(false),usingClause_(NULL) {}
    virtual ~JoinExpr() {
        if (larg_) {
            delete larg_;
            larg_ = NULL;
        }
        if (rarg_) {
            delete rarg_;
            rarg_ = NULL;
        }
        if (usingClause_) {
            for (List::iterator itr = usingClause_->begin(); itr != usingClause_->end(); ++itr) {
                if (*itr != NULL) {
                    delete *itr;
                    *itr = NULL;
                }
            }
            usingClause_->clear();
        }
    }

    virtual void accept(SQLASTVisitor* visitor) {
        if ( visitor->visit(this) ) {
            if (larg_) {
                larg_->accept(visitor);
            }
            if (rarg_) {
                rarg_->accept(visitor);
            }
            if (usingClause_) {
                for (List::iterator itr = usingClause_->begin(); itr != usingClause_->end(); ++itr) {
                    if (*itr) {
                        (*itr)->accept(visitor);
                    }
                }
            }
        }
        visitor->endVisit(visitor);
    }

    virtual NodeType getNodeType() {
        return JOIN_EXPR;
    }
public:
    JOIN_TYPE type_;
    SQLNode* larg_;
    SQLNode* rarg_;
    bool isNatural_;
    List* usingClause_;
};

class ResTarget : public SQLNode {
public:
    ResTarget(SQLNode* val,SQLBaseElem* name) :val(val),name(name){}
    
    virtual ~ResTarget() {
        if ( val != NULL ) {
            delete val;
        }
        if ( name != NULL ) {
            Allocator::free(name);
        }
    }

    virtual void accept(SQLASTVisitor* visitor) {
        if ( visitor->visit(this) ) {
            val->accept(visitor);
        }
        visitor->endVisit(this);
    }

    virtual NodeType getNodeType() {
        return RESTARGET;
    }
public:
    SQLNode* val;
    SQLBaseElem* name;
};

typedef std::vector<SQLNode*> List;

#endif