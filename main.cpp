#include "gramparser.h"
#include "parser.h"
#include "SQLOutputVisitor.h"
#include <stdio.h>

int main(int args,char** argv   ) {
    StatementBlock* block = raw_parser("select hello as t,kk,lo from test");
    if ( block != NULL ) {
         SQLOutputVisitor* visitor = new SQLOutputVisitor();
        block->accept(visitor);
        printf("%s\n",visitor->getOutputSQL());
        delete block;
    } else {
        printf("parse occur error!\n");
    }
   
    return 0;
}