#include "gramparser.h"
#include "parser.h"
#include "SQLOutputVisitor.h"
#include <stdio.h>

int main(int args,char** argv   ) {
    StatementBlock* block = raw_parser("select *");
    SQLOutputVisitor* visitor = new SQLOutputVisitor();
    block->accept(visitor);
    printf("%s\n",visitor->getOutputSQL());
    delete block;
    return 0;
}