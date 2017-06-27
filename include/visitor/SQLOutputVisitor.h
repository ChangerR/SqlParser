#ifndef __SQLPARSER_SQLOUTPUTVISITOR_H
#define __SQLPARSER_SQLOUTPUTVISITOR_H
#include <string>
#include "SQLASTVisitor.h"

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
        for(std::vector<SQLBaseElem*>::iterator itr = ref->fields.begin(); itr != ref->fields.end(); ++itr) {
            visit(*itr);
            output_.push_back('.');
        }
        output_.pop_back();
        return false;
    }

    virtual void endVisit(CloumnRef* ref) {
        
    }

    virtual bool visit(Expression* expr) {

        if ( expr->lexpr ) {
            expr->lexpr->accept(this);
            output_.push_back(' ');
        }

        if ( expr->kind == Expression::AEXPR_OP) {         
            output_.append(expr->name);
            output_.push_back(' ');  
        } else if ( expr->kind == Expression::AND_EXPR ) {
            output_.append("AND ");
        } else if ( expr->kind == Expression::OR_EXPR ) {
            output_.append("OR ");
        }

        if ( expr->rexpr ) {
            expr->rexpr->accept(this);
            output_.push_back(' ');
        }
        output_.pop_back();
        return false;
    }

    virtual void endVisit(Expression* expr) {
        
    }

    virtual bool visit(ResTarget* resTarget) {
        resTarget->val->accept(this);
        if ( resTarget->name != NULL) {
            output_.append(" AS ");
            visit(resTarget->name);
        }
        return false;
    }

    virtual void endVisit(ResTarget* resTarget) {
        
    }

    virtual bool visit(SelectStatement* select) {
        output_.append("SELECT ");

        if ( select->opt_all_clause != NULL ) {
            visit(select->opt_all_clause);
            output_.push_back(' ');
        }
        if ( select->opt_target_list != NULL ) {
            for ( List::iterator itr = select->opt_target_list->begin(); itr != select->opt_target_list->end();++itr) {
                (*itr)->accept(this);
                output_.push_back(',');
            }
            output_.pop_back();
        }
        if ( select->from_list != NULL ) {
            output_.append(" FROM ");
            for ( List::iterator itr = select->from_list->begin(); itr != select->from_list->end();++itr) {
                (*itr)->accept(this);
                output_.push_back(',');
            }
            output_.pop_back();
        }

        if ( select->where_clause != NULL ) {
            output_.append(" WHERE ");
            select->where_clause->accept(this);
        }
        return false;
    }

    virtual void endVisit(SelectStatement* select) {
        
    }

    virtual bool visit(SQLBaseElem* elem) {

        switch (elem->element.type) {
            case SQLBaseElem::BASE_STRING:
                if (elem->element.val.string)
                    output_.append(elem->element.val.string);
                break;
            case SQLBaseElem::BASE_QUOTE_STRING:
                if (elem->element.val.string) {
                    output_.push_back('\'');
                    output_.append(elem->element.val.string);
                    output_.push_back('\'');
                }  
                break;
            case SQLBaseElem::BASE_DQUOTE_STRING:
                if (elem->element.val.string) {
                    output_.push_back('"');
                    output_.append(elem->element.val.string);
                    output_.push_back('"');
                }  
                break;
            case SQLBaseElem::BASE_INT:
            {
                char buf[256];
                snprintf(buf,256,"%d",elem->element.val.ival);
                output_.append(buf);
            }
                break;
            case SQLBaseElem::BASE_FLOAT:
            {
                char buf[256];
                snprintf(buf,256,"%f",elem->element.val.fval);
                output_.append(buf);
            }
                break;
            default:
                break;
        }
        return false;
    }

    virtual void endVisit(SQLBaseElem* elem) {

    }

    virtual bool visit(SQLTable* table) {
        if(table->schema_) {
            visit(table->schema_);
            output_.push_back('.');
        }
        visit(table->table_);

        if ( table->alias_ ) {
            output_.push_back(' ');
            visit(table->alias_);
        }

        return false;
    }

    virtual void endVisit(SQLTable* table) {

    }

    virtual bool visit(StatementBlock* block) {
        
        if ( block != NULL ) {
            for ( std::vector<SingleStatement*>::const_iterator itr = block->getStmts().begin(); itr != block->getStmts().end(); ++itr) {
                (*itr)->accept(this);
                output_.push_back(';');
                output_.push_back('\n');
            }
            output_.pop_back();
            output_.pop_back();
        }
        return false;
    }

    virtual void endVisit(StatementBlock* block) {

    }

    virtual bool visit(SQLSubSelect* sub) {
        if (sub->stmt_) {
            output_.push_back('(');
            sub->stmt_->accept(this);
            output_.push_back(')');
            if (sub->alias_) {
                output_.push_back(' ');
                visit(sub->alias_);
            }
        }
        return false;
    }

    virtual void endVisit(SQLSubSelect* sub) {
    }

    virtual bool visit(JoinExpr* expr) {
        return false;
    }

    virtual void endVisit(JoinExpr* expr) {
    }
private:
    std::string output_;
};
#endif