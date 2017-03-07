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
        STRING,
        TABLE
    };

    virtual ~SQLNode() {}

    virtual void accept(SQLASTVisitor* visitor) = 0;

    virtual NodeType getNodeType() = 0;
};

class SQLString :public SQLNode {
public:
    SQLString(char * string) :string(string){}

    virtual ~SQLString() {
        if (string != NULL ) {
            Allocator::free(string);
        }
    }

    virtual void accept(SQLASTVisitor* visitor) {
        visitor->visit(this);
        visitor->endVisit(this);
    }

    virtual NodeType getNodeType() {
        return STRING;
    }
public:
    char* string;
};

class SQLTable : public SQLNode {
public:
    SQLTable(char* schema,char* table) :schema(schema),table(table),alias(NULL){}

    virtual ~SQLTable() {
        if ( schema ) {
            Allocator::free(schema);
        }
        if ( table ) {
            Allocator::free(table);
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
    char* schema;
    char* table;
    char* alias;
};

class CloumnRef : public SQLNode {
public:
    CloumnRef() {}

    virtual ~CloumnRef() {
        for( auto itr = fields.begin() ; itr != fields.end() ; ++itr) {
            if ( *itr != NULL ) {
                Allocator::free(*itr);
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
    std::vector<char*> fields;
};

class Expression : public SQLNode {
public:
    enum Kind {
        AEXPR_OP,
    };

    Expression() : name(NULL),lexpr(NULL),rexpr(NULL) {
       
    }

    virtual ~Expression(){
        if ( name != NULL) {
            Allocator::free(name);
        }
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
    char* name;
    Expression* lexpr;
    Expression* rexpr;
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