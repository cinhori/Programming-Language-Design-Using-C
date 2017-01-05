#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt){
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused){}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

/*Use operator string to see which operation to perform */
long eval_op(long x, char* op, long y){
    if(strcmp(op, "+") == 0){ return x + y; };
    if(strcmp(op, "-") == 0){ return x - y; };
    if(strcmp(op, "*") == 0){ return x * y; };
    if(strcmp(op, "/") == 0){ return x / y; };

    return 0;
}

long eval(mpc_ast_t* t){
    
    /*If tagged as number return it directly. */
    if(strstr(t->tag, "number")){
        return atoi(t->contents);
    }

    /*The operator is always second child. */
    char* op = t->children[1]->contents;

    /*We store the third child int 'x' */
    long x = eval(t->children[2]);

    /*Iterate the remaining children and combing. */
    int i = 3;
    while(strstr(t->children[i]->tag, "expr")){
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;

}

int main(int argc, char** argv){

    /*Create Some Parsers*/
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    /*Define them with the following Language*/
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                       \
    number  :/-?[0-9]+/;                                    \
    operator:'+'|'-'|'*'|'/';                               \
    expr    :<number> | '('<operator> <expr>+ ')';          \
    lispy   :/^/ <operator> <expr>+ /$/;                    \
    ",                                                      \
    Number, Operator, Expr, Lispy                           \
    );/* "//" is used for regular expression*/

	puts("Lispy Version 0.1");
	puts("Press Ctrl+c to Exit\n");

	while(1){
		//fputs("lispy>", stdout);
        char* input = readline("lispy> ");
        add_history(input);

        /* Attempt to parse the user input */
        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Lispy, &r)){
            
            mpc_ast_t* a = r.output;
            printf("Tag: %s\n", a->tag);
            printf("Contents: %s\n", a->contents);
            printf("Number of children: %d\n", a->children_num);

            mpc_ast_t* c0 = a->children[0];
            printf("First Child Tag: %s\n", c0->tag);
            printf("First Child Contents:%s\n", c0->contents);
            printf("First Child Number of Children: %d\n", c0->children_num);

            mpc_ast_t* c1 = a->children[1];
            printf("Second Child Tag: %s\n", c1->tag);
            printf("Second Child Contents:%s\n", c1->contents);
            printf("Second Child Number of Children: %d\n", c1->children_num);

            mpc_ast_t* c2 = a->children[2];
            printf("Third Child Tag: %s\n", c2->tag);
            printf("Third Child Contents:%s\n", c2->contents);
            printf("Third Child Number of Children: %d\n", c2->children_num);
            
            long result = eval(r.output);
            printf("%ld\n", result);
            mpc_ast_delete(r.output);

        }else{
            /*Otherwise print and delete the Error*/
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
	}

    /*Undefine and delete our parsers*/
    mpc_cleanup(4, Number, Operator, Expr, Lispy);

	return 0;
}
