#include <gramparser.h>
#include <parser.h>
#include <visitor/SQLOutputVisitor.h>
#include <stdio.h>
#include <allocator.h>

int main(int args,char** argv)
{
    int i;
    if ( args < 2) {
        fprintf(stderr,"please enter sql file.\n");
        return 1;
    }
    FILE* sqlfile = fopen(argv[1],"rb");

    if ( sqlfile == NULL ) {
        fprintf(stderr,"cannot open %s,please check\n",argv[1]);
        return 1;
    }

    fseek(sqlfile,0,SEEK_END);
    int filelen = ftell(sqlfile);
    fseek(sqlfile,0,SEEK_SET);

    char* buf = (char*)Allocator::malloc(filelen + 1);
    buf[filelen] = 0;

    fread(buf,1,filelen,sqlfile);

    printf("--ORIGIN:\n%s\n",buf);

    StatementBlock* block = raw_parser(buf);

    Allocator::free(buf);

    if ( block != NULL ) {
        SQLOutputVisitor* visitor = new SQLOutputVisitor();
        block->accept(visitor);
        printf("\n--OUTPUT:\n%s\n",visitor->getOutputSQL());
        delete block;
        delete visitor;
    } else {
        printf("parse occur error!\n");
        return 1;
    }
   
    return 0;
}