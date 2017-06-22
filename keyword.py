#! /usr/bin/python
# -*- coding:utf-8 -*-
'''
sqlparse util for generate key word list
'''

INITIAL = 1
COMMENT = 2
LINER_COMMENT = 3
IDENT = 4
DIGIT = 5
QOUTE_STRING = 6
DOUBLE_QOUTE_STRING = 7

class Lex(object):
    '''
    python lexer
    '''

    def __init__(self, content):
        self.content = content
        self.position = 0
        self.state = INITIAL

    def __shift__(self):
        if self.position >= len(self.content):
            return None
        character = self.content[self.position]
        self.position += 1
        return character

    def __unshift__(self):
        self.position -= 1

    def __fetch__(self):
        if self.position >= len(self.content):
            return None
        return self.content[self.position]

    def lexer(self):
        '''
            parse one token and return
        '''
        c = self.__shift__()
        token = ''
        while c:
            if self.state == INITIAL:
                if c == '/':
                    ahead = self.__fetch__()
                    if ahead == '*':
                        self.__shift__()
                        self.state = COMMENT
                    elif ahead == '/':
                        self.__shift__()
                        self.state = LINER_COMMENT
                    elif ahead == '=':
                        token = c + ahead
                        self.__shift__()
                        return (INITIAL, token)
                    else:
                        token = c
                        return (INITIAL, token)
                elif (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z') or (c == '_') or (c == '%'):
                    token += c
                    self.state = IDENT
                elif c >= '0' and c <= '9':
                    self.state = DIGIT
                    token += c
                elif c == ' ' or c == '\t' or c == '\r' or c == '\n':
                    c = self.__shift__()
                    continue
                elif c == '\'':
                    self.state = QOUTE_STRING
                elif c == '"':
                    self.state = DOUBLE_QOUTE_STRING
                else:
                    return (INITIAL, c)
            elif self.state == COMMENT:
                if c == '*' and self.__fetch__() == '/':
                    self.__shift__()
                    self.state = INITIAL
            elif self.state == LINER_COMMENT:
                if c == '\n':
                    self.state = INITIAL
            elif self.state == IDENT:
                if (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z') or \
                   (c == '_') or (c >= '0' and c <= '9'):
                    token += c
                else:
                    self.__unshift__()
                    self.state = INITIAL
                    return (IDENT, token)
            elif self.state == DIGIT:
                if (c >= 'A' and c <= 'F') or (c >= 'a' and c <= 'f') or \
                   (c == '.') or (c == 'x') or (c == 'X') or (c >= '0' and c <= '9'):
                    token += c
                else:
                    self.__unshift__()
                    self.state = INITIAL
                    return (DIGIT, token)
            elif self.state == QOUTE_STRING:
                if c != '\'':
                    token += c
                else:
                    self.state = INITIAL
                    return (QOUTE_STRING, token)
            elif self.state == DOUBLE_QOUTE_STRING:
                if c == '"':
                    self.state = INITIAL
                    return (DOUBLE_QOUTE_STRING, token)
                elif c == '\\':
                    ahead = self.__fetch__()
                    if ahead == 't':
                        self.__shift__()
                        token += '\t'
                    elif ahead == 'r':
                        self.__shift__()
                        token += '\r'
                    elif ahead == 'n':
                        self.__shift__()
                        token += '\n'
                    elif ahead == '\\':
                        self.__shift__()
                        token += '\\'
                    elif ahead == '"':
                        self.__shift__()
                        token += '"'
                    else:
                        token += '\\'
                else:
                    token += c
            c = self.__shift__()
        if token:
            return (self.state, token)
        return None


# def generate(filename):
#     '''
#         generate function
#     '''
#     try:
#         yyfile = open(filename, 'r')
#         line = yyfile.readline()
#         while line:

#     except IOError as e:
#         print e
#         return False

if __name__ == '__main__':
    lex = Lex('''a100 + b =100;//testa
    a +b =1; /**
    ???
    **/a /= 10 
    '1000 1000'
    "1000\n10000"
    ''')
    token = lex.lexer()
    while token:
        print token
        token = lex.lexer()