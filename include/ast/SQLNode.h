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
    };

    virtual ~SQLNode() {}

    virtual void accept(SQLASTVisitor* visitor) = 0;

    virtual NodeType getNodeType() = 0;
};

class SQLBaseElem :public SQLNode {
public:
    enum Type {
        BASE_STRING = 0,
        BASE_INT,
        BASE_FLOAT,
    };
    struct Elem {
        Type type;
        union {
            char* string;
            int ival;
            float fval;
        } val;
    };

    explicit SQLBaseElem(char* str) {
         element.type = BASE_STRING; 
         element.val.string = str;
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
        if (element.type == BASE_STRING && element.val.string != NULL) {
            Allocator::free(element.val.string);
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
        for( auto itr = fields.begin() ; itr != fields.end() ; ++itr) {
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

class ResTarget : public SQLNode {
public:
    ResTarget(SQLNode* val,char* name) :val(val),name(name){}
    
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
    char* name;
};

typedef std::vector<SQLNode*> List;

#endif