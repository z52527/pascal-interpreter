INTEGER, PLUS, EOF, MINUS = 'INTEGER', 'PLUS', 'EOF', 'MINUS'


class Token(object):
    def __init__(self, type, val):
        self.type = type
        self.val = val
    
    def __str__(self):
        return 'Token({type}, {val})'.format(
            type = self.type,
            val = repr(self.val)
        )
        
    def __repr__(self):
        return self.__str__()


class Interpreter(object):
    def __init__(self, text):
        self.text = text
        self.pos = 0
        self.current_token = None
        self.current_char = self.text[self.pos]

    def error(self):
        raise Exception("Error parsing input")

    def advance(self):
        self.pos += 1
        if self.pos > len(self.text) - 1:
            self.current_char = None  # Indicates end of input
        else:
            self.current_char = self.text[self.pos]

    def skip_whitespace(self):
        while self.current_char is not None and self.current_char.isspace():
            self.advance()
    def integer(self):
        result = ''
        while self.current_char is not None and self.current_char.isdigit():
            result += self.current_char
            self.advance()
        return int(result)

    def get_next_token(self):
        #text = self.text
        #if self.pos > len(text) - 1:
        #    return Token(EOF, None)
        #current_char = text[self.pos]
        #if current_char.isdigit():
        #    self.pos += 1
        #    token = Token(INTEGER, int(current_char))
        #    return token
        #if current_char == "+":
        #    token = Token(PLUS, current_char)
        #    self.pos += 1
        #    return token
        #return self.error()
        while self.current_char is not None:

            if self.current_char.isspace():
                self.skip_whitespace()
                continue

            if self.current_char.isdigit():
                return Token(INTEGER, self.integer())

            if self.current_char == "+":
                self.advance()
                return Token(PLUS, '+')
 
            if self.current_char == "-":
                self.advance()
                return Token(MINUS, '-')           
        
    def eat(self, token_type):
        if self.current_token.type == token_type:
            self.current_token = self.get_next_token()
        else:
            self.error()
            
    def expr(self):
        self.current_token = self.get_next_token()
        left = self.current_token
        self.eat(INTEGER)
        op = self.current_token
        if op.type == PLUS:
            self.eat(PLUS)
        else:
            self.eat(MINUS)
        
        right = self.current_token
        self.eat(INTEGER)
        if op.type == PLUS:
            result = left.val + right.val
        else:
            result = left.val - right.val
        return result

def main():
    while True:
        try:
            text = input("calc> ")
        except EOFError:
            break
        if not text:
            continue
        interpreter = Interpreter(text)
        ans = interpreter.expr()
        print(ans)
        
if __name__ == '__main__':
    main()


