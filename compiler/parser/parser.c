#include <compiler/lexer/lexer.h>
#include "../lexer/lexer.h"
#include "parser.h"

Parser *parser_new(Lexer *lex) {
    Parser *parser = malloc(sizeof(Parser));
    parser->lex = lex;
    return parser;
};

void parser_del(Parser *parser) {
    lex_del(parser->lex);
    free(parser);
};

Token eattok(Parser *parser, Token token) {
    //YASL_TRACE_LOG("current token is %s\n", YASL_TOKEN_NAMES[curtok(parser)]);
    if (curtok(parser) != token) {
        //printf("ParsingError: Expected %x, got %x\n", token, curtok(parser));
        printf("ParsingError: Expected %s, got %s\n", YASL_TOKEN_NAMES[token], YASL_TOKEN_NAMES[curtok(parser)]);
        exit(EXIT_FAILURE);
    }
    gettok(parser->lex);
    return token;
}

Node *parse(Parser *parser) {
    return parse_program(parser);
}

Node *parse_program(Parser *parser) {
    //YASL_DEBUG_LOG("parse. type: %s, ", YASL_TOKEN_NAMES[curtok(parser)]);
    //YASL_DEBUG_LOG("value: %s\n", parser->lex->value);
    YASL_TRACE_LOG("parsing statement in line %d\n", parser->lex->line);
    switch (curtok(parser)) {
        case T_PRINT:
            eattok(parser, T_PRINT);
            return new_Print(parse_expr(parser), parser->lex->line);
        case T_FN: return parse_fn(parser);
        case T_RET:
            eattok(parser, T_RET);
            return new_Return(parse_expr(parser), parser->lex->line);
        case T_LET: return parse_let(parser);
        case T_WHILE: return parse_while(parser);
        case T_BREAK:
            eattok(parser, T_BREAK);
            return new_Break(parser->lex->line);
        case T_CONT:
            eattok(parser, T_CONT);
            eattok(parser, T_CONT);
            return new_Continue(parser->lex->line);
        case T_IF: return parse_if(parser);
        case T_ELSEIF:
        case T_ELSE:
            printf("ParsingError: `%s` without previous `if`\n", YASL_TOKEN_NAMES[curtok(parser)]);
            exit(EXIT_FAILURE);
        default: return new_ExprStmt(parse_expr(parser), parser->lex->line);
    }
}

Node *parse_fn(Parser *parser) {
    YASL_TRACE_LOG("parsing fn in line %d\n", parser->lex->line);
    eattok(parser, T_FN);
    char *name = malloc(parser->lex->val_len);
    memcpy(name, parser->lex->value, parser->lex->val_len);
    int64_t name_len = parser->lex->val_len;
    eattok(parser, T_ID);
    eattok(parser, T_LPAR);
    Node *block = new_Block(parser->lex->line);
    while (curtok(parser) == T_ID) {
        block_append(block, parse_id(parser));
        if (curtok(parser) == T_COMMA) eattok(parser, T_COMMA);
        else break;
    }
    eattok(parser, T_RPAR);
    eattok(parser, T_LBRC);
    Node *body = new_Block(parser->lex->line);
    while (curtok(parser) != T_RBRC) {
        block_append(body, parse_program(parser));
        eattok(parser, T_SEMI);
    }
    eattok(parser, T_RBRC);
    return new_FunctionDecl(block, body, name, name_len, parser->lex->line);
}

Node *parse_let(Parser *parser) {
    YASL_TRACE_LOG("parsing let in line %d\n", parser->lex->line);
    eattok(parser, T_LET);
    char *name = malloc(parser->lex->val_len);
    memcpy(name, parser->lex->value, parser->lex->val_len);
    int64_t name_len = parser->lex->val_len;
    eattok(parser, T_ID);
    if (curtok(parser) != T_EQ) return new_Let(name, name_len, NULL, parser->lex->line);
    eattok(parser, T_EQ);
    return new_Let(name, name_len, parse_expr(parser), parser->lex->line);
}

Node *parse_while(Parser *parser) {
    YASL_TRACE_LOG("parsing while in line %d\n", parser->lex->line);
    eattok(parser, T_WHILE);
    Node *cond = parse_expr(parser);
    eattok(parser, T_LBRC);
    Node *body = new_Block(parser->lex->line);
    while (curtok(parser) != T_RBRC && curtok(parser) != T_EOF) {
        block_append(body, parse_program(parser));
        if (curtok(parser) == T_SEMI) eattok(parser, T_SEMI);
        else if (curtok(parser) != T_RBRC) {
            printf("ParsingError: expected newline or `}`, got `%s`.\n", YASL_TOKEN_NAMES[curtok(parser)]);
            exit(EXIT_FAILURE);
        }
    }
    eattok(parser, T_RBRC);
    return new_While(cond, body, parser->lex->line);
}

Node *parse_if(Parser *parser) {
    YASL_TRACE_LOG("parsing if in line %d\n", parser->lex->line);
    if (curtok(parser) == T_IF) eattok(parser, T_IF);
    else if (curtok(parser) == T_ELSEIF) eattok(parser, T_ELSEIF);
    else {
        printf("ParsingError: Expected if or elseif, got %s\n", YASL_TOKEN_NAMES[curtok(parser)]);
        exit(EXIT_FAILURE);
    }
    Node *cond = parse_expr(parser);
    eattok(parser, T_LBRC);
    Node *then_block = new_Block(parser->lex->line);
    while (curtok(parser) != T_RBRC && curtok(parser) != T_EOF) {
        block_append(then_block, parse_program(parser));
        if (curtok(parser) == T_SEMI) eattok(parser, T_SEMI);
        else if (curtok(parser) != T_RBRC) {
            printf("ParsingError: expected newline or `}`, got `%s`.\n", YASL_TOKEN_NAMES[curtok(parser)]);
            exit(EXIT_FAILURE);
        }
    }
    eattok(parser, T_RBRC);
    if (curtok(parser) != T_ELSE && curtok(parser) != T_ELSEIF) {
        YASL_DEBUG_LOG("%s\n", "no else");
        return new_If(cond, then_block, NULL, parser->lex->line);
    }
    // TODO: eat semi
    if (curtok(parser) == T_ELSEIF) {
        YASL_DEBUG_LOG("%s\n", "elseif");
        return new_If(cond, then_block, parse_if(parser), parser->lex->line);
    }
    if (curtok(parser) == T_ELSE) {
        YASL_DEBUG_LOG("%s\n", "else");
        eattok(parser, T_ELSE);
        eattok(parser, T_LBRC);
        Node *else_block = new_Block(parser->lex->line);
        while (curtok(parser) != T_RBRC && curtok(parser) != T_EOF) {
            block_append(else_block, parse_program(parser));
            if (curtok(parser) == T_SEMI) eattok(parser, T_SEMI);
            else if (curtok(parser) != T_RBRC) {
                printf("ParsingError: expected newline or `}`, got `%s`.\n", YASL_TOKEN_NAMES[curtok(parser)]);
                exit(EXIT_FAILURE);
            }
        }
        eattok(parser, T_RBRC);
        return new_If(cond, then_block, else_block, parser->lex->line);
    }
    printf("ParsingError: expected newline, got `%s`.\n", YASL_TOKEN_NAMES[curtok(parser)]);
    exit(EXIT_FAILURE);

}

Node *parse_expr(Parser *parser) {
    YASL_TRACE_LOG("%s\n", "parsing expression.");
    return parse_assign(parser);
}

Node *parse_assign(Parser *parser) {
    YASL_TRACE_LOG("parsing = in line %d\n", parser->lex->line);
    Node *cur_node = parse_ternary(parser);
    if (curtok(parser) == T_EQ) { // || curtok(parser) == T_DLT)
        eattok(parser, T_EQ);
        if (cur_node->nodetype == N_VAR) {
            Node *assign_node = new_Assign(cur_node->name, cur_node->name_len, parse_assign(parser), parser->lex->line);
            node_del(cur_node);
            return assign_node;
        } else if (cur_node->nodetype == N_INDEX) {
            Node *left = cur_node->children[0];
            Node *block = new_Block(parser->lex->line);
            block_append(block, cur_node->children[1]);
            block_append(block, parse_expr(parser));
            free(cur_node->children);
            free(cur_node);
            return new_MethodCall(left, block, "__set", 5, parser->lex->line);
        } else {
            puts("Invalid lvalue.");
            exit(EXIT_FAILURE);
        }
        // TODO: add indexing case
    } else if (isaugmented(curtok(parser))) {
        Token op = eattok(parser, curtok(parser)) - 1; // relies on enum
        if (cur_node->nodetype == N_VAR) {
            return new_Assign(cur_node->name, cur_node->name_len, new_BinOp(op, cur_node, parse_assign(parser), parser->lex->line), parser->lex->line);
        }
        // TODO: add indexing case
    }
    return cur_node;
}

Node *parse_ternary(Parser *parser) {
    YASL_TRACE_LOG("parsing ?: in line %d\n", parser->lex->line);
    Node *cur_node = parse_or(parser);
    if (curtok(parser) == T_DQMARK) {
        eattok(parser, T_DQMARK);
        return new_BinOp(T_DQMARK, cur_node, parse_ternary(parser), parser->lex->line);
    } else if (curtok(parser) == T_QMARK) {
        eattok(parser, T_QMARK);
        Node *left = parse_ternary(parser);
        eattok(parser, T_COLON);
        Node *right = parse_ternary(parser);
        return new_TriOp(T_QMARK, cur_node, left, right, parser->lex->line);
    }
    return cur_node;
}

Node *parse_or(Parser *parser) {
    YASL_TRACE_LOG("parsing or in line %d\n", parser->lex->line);
    Node *cur_node = parse_and(parser);
    while (curtok(parser) == T_OR) {
        eattok(parser, T_OR);
        cur_node = new_BinOp(T_OR, cur_node, parse_and(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_and(Parser *parser) {
    YASL_TRACE_LOG("parsing and in line %d\n", parser->lex->line);
    Node *cur_node = parse_equals(parser);
    while (curtok(parser) == T_AND) {
        eattok(parser, T_AND);
        cur_node = new_BinOp(T_AND, cur_node, parse_equals(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_equals(Parser *parser) {
    YASL_TRACE_LOG("parsing == in line %d\n", parser->lex->line);
    Node *cur_node = parse_comparator(parser);
    while (curtok(parser) == T_DEQ || curtok(parser) == T_BANGEQ ||
            curtok(parser) == T_TEQ || curtok(parser) == T_BANGDEQ) {
        Token op = eattok(parser, curtok(parser));
        cur_node = new_BinOp(op, cur_node, parse_comparator(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_comparator(Parser *parser) {
    YASL_TRACE_LOG("parsing > in line %d\n", parser->lex->line);
    Node *cur_node = parse_concat(parser);
    while (curtok(parser) == T_LT || curtok(parser) == T_GT||
            curtok(parser) == T_GTEQ || curtok(parser) == T_LTEQ) {
        Token op = eattok(parser, curtok(parser));
        cur_node = new_BinOp(op, cur_node, parse_concat(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_concat(Parser *parser) {
    YASL_TRACE_LOG("parsing || in line %d\n", parser->lex->line);
    Node *cur_node = parse_bor(parser);
    if (curtok(parser) == T_DBAR || curtok(parser) == T_TBAR) {
        Token op = eattok(parser, curtok(parser));
        return new_BinOp(op, cur_node, parse_concat(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_bor(Parser *parser) {
    YASL_TRACE_LOG("parsing | in line %d\n", parser->lex->line);
    Node *cur_node = parse_bxor(parser);
    while (curtok(parser) == T_BAR) {
        eattok(parser, T_BAR);
        cur_node = new_BinOp(T_BAR, cur_node, parse_bxor(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_bxor(Parser *parser) {
    YASL_TRACE_LOG("parsing ^ in line %d\n", parser->lex->line);
    Node *cur_node = parse_band(parser);
    while (curtok(parser) == T_CARET) {
        eattok(parser, T_CARET);
        cur_node = new_BinOp(T_CARET, cur_node, parse_band(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_band(Parser *parser) {
    YASL_TRACE_LOG("parsing & in line %d\n", parser->lex->line);
    Node *cur_node = parse_bshift(parser);
    while (curtok(parser) == T_AMP) {
        eattok(parser, T_AMP);
        cur_node = new_BinOp(T_AMP, cur_node, parse_bshift(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_bshift(Parser *parser) {
    YASL_TRACE_LOG("parsing >> in line %d\n", parser->lex->line);
    Node *cur_node = parse_add(parser);
    while (curtok(parser) == T_DGT || curtok(parser) == T_DLT) {
        Token op = eattok(parser, curtok(parser));
        cur_node = new_BinOp(op, cur_node, parse_add(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_add(Parser *parser) {
    YASL_TRACE_LOG("parsing + in line %d\n", parser->lex->line);
    Node *cur_node = parse_multiply(parser);
    while (curtok(parser) == T_PLUS || curtok(parser) == T_MINUS) {
        Token op = eattok(parser, curtok(parser));
        cur_node = new_BinOp(op, cur_node, parse_multiply(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_multiply(Parser *parser) {
    YASL_TRACE_LOG("parsing * in line %d\n", parser->lex->line);
    Node *cur_node = parse_unary(parser);
    while (curtok(parser) == T_STAR || curtok(parser) == T_SLASH ||
            curtok(parser) == T_DSLASH || curtok(parser) == T_MOD) {
        Token op = eattok(parser, curtok(parser));
        cur_node = new_BinOp(op, cur_node, parse_unary(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_unary(Parser *parser) {
    YASL_TRACE_LOG("parsing ! in line %d\n", parser->lex->line);
    if (curtok(parser) == T_PLUS || curtok(parser) == T_MINUS || curtok(parser) == T_BANG ||
     curtok(parser) == T_CARET ||curtok(parser) == T_HASH) {
        Token op = eattok(parser, curtok(parser));
        return new_UnOp(op, parse_unary(parser), parser->lex->line);
    } else {
        return parse_power(parser);
    }
}

Node *parse_power(Parser *parser) {
    YASL_TRACE_LOG("parsing ** in line %d\n", parser->lex->line);
    Node *cur_node = parse_call(parser);
    if (curtok(parser) == T_DSTAR) {
        eattok(parser, T_DSTAR);
        return new_BinOp(T_DSTAR, cur_node, parse_unary(parser), parser->lex->line);
    }
    return cur_node;
}

Node *parse_call(Parser *parser) {
    Node *cur_node = parse_constant(parser);
    while (curtok(parser) == T_LSQB || curtok(parser) == T_DOT) {
        if (curtok(parser) == T_DOT) {
            eattok(parser, T_DOT);
            Node *right = parse_constant(parser);
            if (right->nodetype == N_CALL) {
                cur_node = new_MethodCall(cur_node, right->children[0], right->name, right->name_len, right->line);
                free(right);
            } else {
                puts("Invalid member access");
                exit(EXIT_FAILURE);
            }
        } else {
            eattok(parser, T_LSQB);
            cur_node = new_Index(cur_node, parse_expr(parser), parser->lex->line);
            eattok(parser, T_RSQB);
        }
    }
    return cur_node;

}

Node *parse_constant(Parser *parser) {
    switch (curtok(parser)) {
        case T_ID: return parse_id(parser);
        case T_LPAR:
            eattok(parser, T_LPAR);
            Node *expr = parse_expr(parser);
            eattok(parser, T_RPAR);
            return expr;
        case T_LSQB: return parse_collection(parser);
        case T_STR: return parse_string(parser);
        case T_INT64: return parse_integer(parser);
        case T_FLOAT64: return parse_float(parser);
        case T_BOOL: return parse_boolean(parser);
        case T_UNDEF: return parse_undef(parser);
        // handle invalid expressions with sensible error messages.
        case T_PRINT:
        case T_FN:
        case T_LET:
        case T_WHILE:
        case T_BREAK:
        case T_RET:
        case T_CONT:
        case T_IF:
        case T_ELSEIF:
        case T_ELSE:
            printf("ParsingError in line %d: expected expression, got `%s`\n", parser->lex->line, YASL_TOKEN_NAMES[curtok(parser)]);
            exit(EXIT_FAILURE);
        default:
            printf("ParsingError: Invalid expression in line %d.\n", parser->lex->line);
            exit(EXIT_FAILURE);
    }
}

Node *parse_id(Parser *parser) {
    char *name = malloc(parser->lex->val_len);
    memcpy(name, parser->lex->value, parser->lex->val_len);
    int64_t name_len = parser->lex->val_len;
    eattok(parser, T_ID);
    if (curtok(parser) == T_LPAR) {
        YASL_TRACE_LOG("%s\n", "Parsing function call");
        Node *cur_node = new_FunctionCall(new_Block(parser->lex->line), name, name_len, parser->lex->line);
        eattok(parser, T_LPAR);
        while (curtok(parser) != T_RPAR && curtok(parser) != T_EOF) {
            block_append(cur_node->children[0], parse_expr(parser));
            if (curtok(parser) != T_COMMA) break;
            eattok(parser, T_COMMA);
        }
        eattok(parser, T_RPAR);
        return cur_node;
    } else {
        YASL_TRACE_LOG("%s\n", "Parsing variable");
        Node *cur_node = new_Var(name, name_len, parser->lex->line);
        free(name);
        return cur_node;
    }
}

Node *parse_undef(Parser *parser) {
    YASL_TRACE_LOG("%s\n", "Parsing undef");
    Node *cur_node = new_Undef(parser->lex->line);
    eattok(parser, T_UNDEF);
    return cur_node;
}

Node *parse_float(Parser *parser) {
    YASL_TRACE_LOG("%s\n", "Parsing float64");
    Node* cur_node = new_Float(parser->lex->value, parser->lex->val_len, parser->lex->line);
    eattok(parser, T_FLOAT64);
    return cur_node;
}

Node *parse_integer(Parser *parser) {
    YASL_TRACE_LOG("%s\n", "Parsing int64");
    Node *cur_node = new_Integer(parser->lex->value, parser->lex->val_len, parser->lex->line);
    eattok(parser, T_INT64);
    return cur_node;
}

Node *parse_boolean(Parser *parser) {
    YASL_TRACE_LOG("%s\n", "Parsing bool");
    Node *cur_node = new_Boolean(parser->lex->value, parser->lex->val_len, parser->lex->line);
    eattok(parser, T_BOOL);
    return cur_node;
}

Node *parse_string(Parser *parser) {
    YASL_TRACE_LOG("%s\n", "Parsing str");
    Node *cur_node = new_String(parser->lex->value, parser->lex->val_len, parser->lex->line);
    eattok(parser, T_STR);
    return cur_node;
}

// parse list and map literals
Node *parse_collection(Parser *parser) {
    eattok(parser, T_LSQB);
    Node *keys = new_Block(parser->lex->line);
    Node *vals = new_Block(parser->lex->line); // free if we have list.

    // empty map
    if (curtok(parser) == T_RARR) {
        YASL_TRACE_LOG("%s\n", "Parsing map");
        eattok(parser, T_RARR);
        eattok(parser, T_RSQB);
        return new_Map(keys, vals, parser->lex->line);
    }

    // empty list
    if (curtok(parser) == T_RSQB) {
        YASL_TRACE_LOG("%s\n", "Parsing list");
        node_del(vals);
        eattok(parser, T_RSQB);
        return new_List(keys, parser->lex->line);
    }

    block_append(keys, parse_expr(parser));

    // non-empty map
    if (curtok(parser) == T_RARR) {
        YASL_TRACE_LOG("%s\n", "Parsing map");
        eattok(parser, T_RARR);
        block_append(vals, parse_expr(parser));
        while (curtok(parser) == T_COMMA) {
            eattok(parser, T_COMMA);
            block_append(keys, parse_expr(parser));
            eattok(parser, T_RARR);
            block_append(vals, parse_expr(parser));
        }
        eattok(parser, T_RSQB);
        return new_Map(keys, vals, parser->lex->line);
    }

    // non-empty list
    node_del(vals);
    while (curtok(parser) == T_COMMA) {
        YASL_TRACE_LOG("%s\n", "Parsing list");
        eattok(parser, T_COMMA);
        block_append(keys, parse_expr(parser));
    }
    eattok(parser, T_RSQB);
    return new_List(keys, parser->lex->line);

}