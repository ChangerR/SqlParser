#ifndef __SQLPARSER_SQLNODE_H
#define __SQLPARSER_SQLNODE_H
#include <string>
#include <vector>

class SQLNode {
public:
    enum NodeType {
        EXPRESSION = 0,
        RESTARGET,
        COLUMNREF,
    };

    virtual void accept(SQLASTVisitor visitor) = 0;

    virtual NodeType getNodeType() = 0;
};

class CloumnRef : public SQLNode {
public:
    CloumnRef() {}

    virtual ~CloumnRef() {}

    virtual void accept(SQLASTVisitor visitor) {
        //TODO:fix this accept
    }

    virtual NodeType getNodeType() {
        return NodeType::COLUMNREF;
    }
public:
    std::vector fields;
};

class Expression : public SQLNode {
public:
    enum Kind {
        AEXPR_OP,
    };

    Expression() {
       
    }

    virtual ~Expresstion(){
        if ( lexpr != NULL ) {
            delete lexpr;
        }
        if ( rexpr != NULL ) {
            delete rexpr;
        }
    }

    virtual void accept(SQLASTVisitor visitor) {
        //TODO:fix this accept
    }
    
public:
    Kind kind;
    std::string name;
    Expression* lexpr;
    Expression* rexpr;
};

class ResTarget : public SQLNode {
public:
    ResTarget(SQLNode* val,const std::string& name) :name(name),val(val){}
    
    virtual ~ResTarget() {
        if ( val != NULL ) {
            delete val;
        }
    }

    virtual void accept(SQLASTVisitor visitor) {
        //TODO:fix this accept
    }
public:
    SQLNode* val;
    std::string name;
};

#endif