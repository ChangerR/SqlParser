#ifndef __SQLPARSER_SQLOUTPUTVISITOR_H
#define __SQLPARSER_SQLOUTPUTVISITOR_H
#include <string>

class SQLOutputVisitor :public SQLASTVisitor{
public:
    SQLOutputVisitor() {
        output_.reserve(1024);
    }

    virtual ~SQLOutputVisitor() {

    }

    const char* getOutputSQL() const {
        return output_.c_str();
    }

    virtual bool visit(CloumnRef* ref) {
        for(auto itr = ref->fields.begin(); itr != ref->fields.end(); ++itr) {
            output_.append(*itr);
            output_.push_back(' ');
        }
        output_.pop_back();
        return false;
    }

    virtual void endVisit(CloumnRef* ref) {
        
    }

    virtual bool visit(Expression* expr) {
        return false;
    }

    virtual void endVisit(Expression* expr) {
        
    }

    virtual bool visit(ResTarget* resTarget) {
        resTarget->val->accept(this);
        if ( resTarget->name != NULL) {
            output_.append(" AS ");
            output_.append(resTarget->name);
        }
        return false;
    }

    virtual void endVisit(ResTarget* resTarget) {
        
    }

    virtual bool visit(SelectStatement* select) {
        output_.append("SELECT ");

        if ( select->opt_target_list != NULL ) {
            for ( auto itr = select->opt_target_list->begin(); itr != select->opt_target_list->end();++itr) {
                (*itr)->accept(this);
                output_.push_back(',');
            }
            output_.pop_back();
        }
        return false;
    }

    virtual void endVisit(SelectStatement* select) {
        
    }
private:
    std::string output_;
};
#endif