grammar CHTL;

// Lexer rules
COMMENT_SINGLE: '//' ~[\r\n]* -> skip;
COMMENT_MULTI: '/*' .*? '*/' -> skip;
GENERATOR_COMMENT: '#' ' ' .*? '\n' -> skip;

// Keywords
TEMPLATE: '[Template]';
CUSTOM: '[Custom]';
ORIGIN: '[Origin]';
IMPORT: '[Import]';
NAMESPACE: '[Namespace]';
CONFIGURATION: '[Configuration]';
INFO: '[Info]';
EXPORT: '[Export]';

// Style and element keywords
AT_STYLE: '@Style';
AT_ELEMENT: '@Element';
AT_VAR: '@Var';
AT_HTML: '@Html';
AT_JAVASCRIPT: '@JavaScript';
AT_CHTL: '@Chtl';
AT_CONFIG: '@Config';
AT_CJMOD: '@CJmod';

// Control keywords
KEYWORD_TEXT: 'text';
KEYWORD_STYLE: 'style';
KEYWORD_SCRIPT: 'script';
KEYWORD_INHERIT: 'inherit';
KEYWORD_DELETE: 'delete';
KEYWORD_INSERT: 'insert';
KEYWORD_AFTER: 'after';
KEYWORD_BEFORE: 'before';
KEYWORD_REPLACE: 'replace';
KEYWORD_ATTOP: 'at top';
KEYWORD_ATBOTTOM: 'at bottom';
KEYWORD_FROM: 'from';
KEYWORD_AS: 'as';
KEYWORD_EXCEPT: 'except';
KEYWORD_USE: 'use';
KEYWORD_HTML5: 'html5';

// Operators
COLON: ':';
EQUAL: '=';
PLUS: '+';
MINUS: '-';
MULTIPLY: '*';
DIVIDE: '/';
MODULO: '%';
POWER: '**';
LOGICAL_AND: '&&';
LOGICAL_OR: '||';
QUESTION: '?';
COMMA: ',';
SEMICOLON: ';';

// Brackets and delimiters
LBRACE: '{';
RBRACE: '}';
LPAREN: '(';
RPAREN: ')';
LBRACKET: '[';
RBRACKET: ']';
DOT: '.';
SLASH: '/';

// Comparison operators
GT: '>';
LT: '<';
GE: '>=';
LE: '<=';
EQ: '==';
NE: '!=';

// String literals
STRING_DOUBLE: '"' (~["\\] | '\\' .)* '"';
STRING_SINGLE: '\'' (~['\\] | '\\' .)* '\'';
UNQUOTED_LITERAL: [a-zA-Z_][a-zA-Z0-9_-]*;

// Numbers
NUMBER: [0-9]+ ('.' [0-9]+)? ([a-zA-Z%]+)?;

// Whitespace
WS: [ \t\r\n]+ -> skip;

// Element names (HTML tags)
ELEMENT_NAME: [a-zA-Z][a-zA-Z0-9]*;

// Parser rules
program: (directive | element | text_block)* EOF;

directive: 
    template_directive
    | custom_directive
    | origin_directive
    | import_directive
    | namespace_directive
    | configuration_directive
    | use_directive;

template_directive:
    TEMPLATE (AT_STYLE | AT_ELEMENT | AT_VAR) identifier block;

custom_directive:
    CUSTOM (AT_STYLE | AT_ELEMENT | AT_VAR) identifier block;

origin_directive:
    ORIGIN (AT_HTML | AT_JAVASCRIPT | AT_STYLE) identifier? block;

import_directive:
    IMPORT (AT_HTML | AT_JAVASCRIPT | AT_STYLE | AT_CHTL | AT_CONFIG | AT_CJMOD) 
    identifier? FROM path (AS identifier)?;

namespace_directive:
    NAMESPACE identifier block?;

configuration_directive:
    CONFIGURATION AT_CONFIG? identifier? block;

use_directive:
    USE (KEYWORD_HTML5 | AT_CONFIG identifier);

element:
    ELEMENT_NAME attributes? block?;

attributes:
    attribute+;

attribute:
    identifier (COLON | EQUAL) value SEMICOLON;

value:
    string_literal
    | unquoted_literal
    | expression;

string_literal:
    STRING_DOUBLE
    | STRING_SINGLE;

unquoted_literal:
    UNQUOTED_LITERAL;

expression:
    arithmetic_expression
    | conditional_expression
    | property_reference;

arithmetic_expression:
    term (PLUS | MINUS) term;

term:
    factor (MULTIPLY | DIVIDE | MODULO | POWER) factor;

factor:
    NUMBER
    | string_literal
    | unquoted_literal
    | property_reference
    | LPAREN expression RPAREN;

conditional_expression:
    condition QUESTION value (COLON value)?;

condition:
    comparison (LOGICAL_AND | LOGICAL_OR) comparison;

comparison:
    expression (GT | LT | GE | LE | EQ | NE) expression;

property_reference:
    selector DOT identifier;

selector:
    identifier
    | DOT identifier
    | identifier DOT identifier;

block:
    LBRACE content* RBRACE;

content:
    element
    | text_block
    | style_block
    | script_block
    | template_usage
    | custom_usage
    | origin_usage;

text_block:
    KEYWORD_TEXT block;

style_block:
    KEYWORD_STYLE block;

script_block:
    KEYWORD_SCRIPT block;

template_usage:
    (AT_STYLE | AT_ELEMENT | AT_VAR) identifier usage_block?;

custom_usage:
    CUSTOM (AT_STYLE | AT_ELEMENT | AT_VAR) identifier usage_block?;

origin_usage:
    ORIGIN (AT_HTML | AT_JAVASCRIPT | AT_STYLE) identifier;

usage_block:
    LBRACE usage_content* RBRACE;

usage_content:
    attribute
    | template_usage
    | custom_usage
    | delete_operation
    | insert_operation;

delete_operation:
    KEYWORD_DELETE identifier (COMMA identifier)* SEMICOLON;

insert_operation:
    KEYWORD_INSERT position selector usage_block;

position:
    KEYWORD_AFTER | KEYWORD_BEFORE | KEYWORD_REPLACE | KEYWORD_ATTOP | KEYWORD_ATBOTTOM;

path:
    string_literal
    | unquoted_literal;

identifier:
    UNQUOTED_LITERAL;