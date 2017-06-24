#! /usr/bin/python
# -*- coding:utf-8 -*-
import sys
'''
sqlparse util for generate key word list
'''


class LEXSTATE(object):
    INITIAL = 'OP'
    COMMENT = 'COMMENT'
    LINER_COMMENT = 'LINER_COMMENT'
    IDENT = 'IDENT'
    DIGIT = "DIGIT"
    QOUTE_STRING = 'QOUTE_STRING'
    DOUBLE_QOUTE_STRING = 'DOUBLE_QOUTE_STRING'


class Lex(object):
    '''
    python lexer
    '''

    def __init__(self, content):
        self.content = content
        self.position = 0
        self.state = LEXSTATE.INITIAL

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
            if self.state == LEXSTATE.INITIAL:
                if c == '/':
                    ahead = self.__fetch__()
                    if ahead == '*':
                        self.__shift__()
                        self.state = LEXSTATE.COMMENT
                    elif ahead == '/':
                        self.__shift__()
                        self.state = LEXSTATE.LINER_COMMENT
                    elif ahead == '=':
                        token = c + ahead
                        self.__shift__()
                        return (LEXSTATE.INITIAL, token)
                    else:
                        token = c
                        return (LEXSTATE.INITIAL, token)
                elif (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z') or (c == '_') \
                        or (c == '%') or (c == '$'):
                    token += c
                    self.state = LEXSTATE.IDENT
                elif c >= '0' and c <= '9':
                    self.state = LEXSTATE.DIGIT
                    token += c
                elif c == ' ' or c == '\t' or c == '\r' or c == '\n':
                    c = self.__shift__()
                    continue
                elif c == '\'':
                    self.state = LEXSTATE.QOUTE_STRING
                elif c == '"':
                    self.state = LEXSTATE.DOUBLE_QOUTE_STRING
                else:
                    return (LEXSTATE.INITIAL, c)
            elif self.state == LEXSTATE.COMMENT:
                if c == '*' and self.__fetch__() == '/':
                    self.__shift__()
                    self.state = LEXSTATE.INITIAL
            elif self.state == LEXSTATE.LINER_COMMENT:
                if c == '\n':
                    self.state = LEXSTATE.INITIAL
            elif self.state == LEXSTATE.IDENT:
                if (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z') or \
                   (c == '_') or (c >= '0' and c <= '9') or (c == '$'):
                    token += c
                else:
                    self.__unshift__()
                    self.state = LEXSTATE.INITIAL
                    return (LEXSTATE.IDENT, token)
            elif self.state == LEXSTATE.DIGIT:
                if (c >= 'A' and c <= 'F') or (c >= 'a' and c <= 'f') or \
                   (c == '.') or (c == 'x') or (c == 'X') or (c >= '0' and c <= '9'):
                    token += c
                else:
                    self.__unshift__()
                    self.state = LEXSTATE.INITIAL
                    return (LEXSTATE.DIGIT, token)
            elif self.state == LEXSTATE.QOUTE_STRING:
                if c != '\'':
                    token += c
                else:
                    self.state = LEXSTATE.INITIAL
                    return (LEXSTATE.QOUTE_STRING, token)
            elif self.state == LEXSTATE.DOUBLE_QOUTE_STRING:
                if c == '"':
                    self.state = LEXSTATE.INITIAL
                    return (LEXSTATE.DOUBLE_QOUTE_STRING, token)
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


def scanYYFile(filename):
    try:
        yyfile = open(filename, 'rb')
        lex = Lex(yyfile.read())
        yyfile.close()
    except IOError as e:
        print "Cannot open this file [%s]" % (filename)
        print e
        return None
    token = lex.lexer()
    state = 0
    keys = {}
    keys['reserved_keyword'] = []
    keys['unreserved_keyword'] = []
    keys['col_name_keyword'] = []
    keys['type_func_name_keyword'] = []
    while token:
        if state == 0:
            if token[1] == 'reserved_keyword':
                ahead = lex.lexer()
                if ahead and ahead[1] == ':':
                    state = 1
                else:
                    token = ahead
                    continue
            elif token[1] == 'unreserved_keyword':
                ahead = lex.lexer()
                if ahead and ahead[1] == ':':
                    state = 2
                else:
                    token = ahead
                    continue
            elif token[1] == 'col_name_keyword':
                ahead = lex.lexer()
                if ahead and ahead[1] == ':':
                    state = 3
                else:
                    token = ahead
                    continue
            elif token[1] == 'type_func_name_keyword':
                ahead = lex.lexer()
                if ahead and ahead[1] == ':':
                    state = 4
                else:
                    token = ahead
                    continue
        elif state == 1:
            if token[1] == ';':
                state = 0
            elif token[1] != '|':
                keys['reserved_keyword'].append(token[1])
        elif state == 2:
            if token[1] == ';':
                state = 0
            elif token[1] != '|':
                keys['unreserved_keyword'].append(token[1])
        elif state == 3:
            if token[1] == ';':
                state = 0
            elif token[1] != '|':
                keys['col_name_keyword'].append(token[1])
        elif state == 4:
            if token[1] == ';':
                state = 0
            elif token[1] != '|':
                keys['type_func_name_keyword'].append(token[1])
        token = lex.lexer()
    return keys


def generateKwList(keys):
    keywords = []
    for value in keys:
        for key in keys[value]:
            keywords.append((key, value.upper()))
    new_keywords = sorted(keywords, lambda x, y: cmp(x[0], y[0]))
    ret = '''/**\n* key word list\n*/\n'''
    for tup in new_keywords:
        ret += "KEYWORD(\"%s\",%s,%s)\n" % (
            tup[0].lower(), tup[0].upper(), tup[1])
    return ret


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage: keyword.py <yacc> <kwlist header>"
        sys.exit(1)
    keys = scanYYFile(sys.argv[1])
    if keys:
        content = generateKwList(keys)
        try:
            output = open(sys.argv[2], 'w')
            output.write(content)
            output.close()
        except IOError:
            print "Open write file failed"
