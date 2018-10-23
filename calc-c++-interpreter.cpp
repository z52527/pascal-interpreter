#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;
const int NumType = 0;
const int OperatorType = 1;
const int LeftBracket = 2;
const int RightBracket = 3;
const int ExprType = 4;

class Visitor;
class PrintVistor;
class Token;
class NumToken;
class OperatorToken;
class BracketToken;
class ExprToken;

// 瀛楃涓茶浆鎹负鏁板瓧
float helpStringToNum(const std::string &text)
{
    float ret;
    std::stringstream ss;
    ss << text;
    ss >> ret;
    return ret;
}

// 鐢ㄤ簬閬嶅巻鐨勮闂€呮帴鍙�
class Visitor
{
    public:
        virtual void apply(Token *token) = 0;
};

// 鏍囪瘑绗︽娊璞�
class Token
{
    public:
        const std::string text;     // 鍘熸枃鍓湰
        const int type;             // 鏍囪瘑绗︾被鍨嬶紝鏂逛究鍔ㄦ€佺被鍨嬭浆鎹�

        virtual void visit(Visitor *visitor);
    protected:
        Token(std::string text, const int type)
            :text(text), type(type) {}

        virtual ~Token() {}
};

// 閬嶅巻榛樿琛屼负锛氳璁块棶鑰呯湅鍒拌嚜宸�
void Token::visit(Visitor *visitor)
{
    visitor->apply(this);
};

// 鏁板瓧鏍囪瘑绗�
class NumToken : public Token
{
    public:
        const float value;

        NumToken(std::string text)
            :Token(text, NumType)
            ,value(helpStringToNum(text))
        { }
};

// 杩愮畻绗︽爣璇嗙
class OperatorToken : public Token
{
    public:
        const char op;

        OperatorToken(const char op)
            :Token(std::string(1, op), OperatorType)
            ,op(op) { }
};

// 鎷彿鏍囪瘑绗�
class BracketToken : public Token
{
    public:
        BracketToken(const char op)
            :Token(std::string(1, op), op == '(' ? LeftBracket : RightBracket)
        { }
};

// 琛ㄨ揪寮忔爣璇嗙
class ExprToken : public Token
{
    private:
        NumToken *num;
        OperatorToken *op;
        ExprToken *lhs, *rhs;
        bool bracket;

        float calc() 
        {
            if (op)
            {
                float l(lhs->value), r(rhs->value);
                switch (op->op)
                {
                    case '+': return l + r;
                    case '-': return l - r;
                    case '*': return l * r;
                    case '/': return l / r;
                }
            }
            else
                return num->value;
            return 0;
        }

    public:
        const float value;

        ExprToken(NumToken *num)
            :Token(num->text, ExprType), num(num), 
            op(NULL), lhs(NULL), rhs(NULL), value(calc()) {}

        ExprToken(ExprToken *lhs, OperatorToken *op_, ExprToken *rhs)
            :Token(lhs->text + op_->text + rhs->text, ExprType),
            num(NULL), op(op_), lhs(lhs), rhs(rhs), value(calc()) {}

        ExprToken(BracketToken *lb, ExprToken *expr, BracketToken *rb)
            :Token(lb->text + expr->text + rb->text, ExprType),
            num(NULL), op(NULL), lhs(expr), rhs(NULL),
            value(expr->value) 
        {
            bracket = true;
        }

        virtual void visit(Visitor *visitor)
        {
            //if (lhs) lhs->visit(visitor);
            //if (rhs) rhs->visit(visitor);
            visitor->apply(this);
        }

        virtual ~ExprToken()
        {
            delete num;
            delete op;
            delete lhs;
            delete rhs;
        }
};

// 鎵撳嵃璁块棶鑰咃細杈撳嚭杩愮畻杩囩▼
class PrintVistor : public Visitor
{
    private:
        int line;
    public:
        PrintVistor(): line(0) {}

        virtual void apply(Token *token)
        {
            if (token->type == ExprType)
            {
                ExprToken *expr = dynamic_cast<ExprToken *>(token);
                int flag = 0;
                if(token->text[0]!='('&&token->text.size() <= 3){cout << "(";std::cout << token->text;cout << ") = ";}
                else std::cout << token->text << " = ";
                if (expr)
                    std::cout << expr->value;
                else
                    std::cout << "NaN";
                
                std::cout << std::endl;
            }
        }
};

// 璇嶆眹鍒嗘瀽锛屼粠瀛楃涓茬敓鎴愬熀纭€鏍囪瘑绗�
std::vector<Token *> parse(std::string text)
{
    std::vector<Token *> vec;
    for (int i = 0; i < text.length(); ++i)
    {
        switch (text[i])
        {
            case ' ':
                break;
            case '+':
            case '-':
            case '*':
            case '/':
                vec.push_back(new OperatorToken(text[i]));
                break;
            case '(':
            case ')':
                vec.push_back(new BracketToken(text[i]));
                break;
            default:
            {
                int r;
                for (r = i; r < text.length() && text[r] >= '0' 
                        && text[r] <= '9'; ++r);
                vec.push_back(new NumToken(text.substr(i, r - i)));
                i = r - 1;
            }
        }
    }
    return vec;
}

// 鍒ゆ柇鏄惁涓鸿繍绠楋紝涓斾紭鍏堢骇涓庢寚瀹氫紭鍏堢骇鐩稿悓
bool is_expr(Token *lhs, Token *op, Token *rhs, bool priority = true)
{
    if (lhs->type != ExprType
            || op->type != OperatorType
            || rhs->type != ExprType)
        return false;
    OperatorToken *_op = dynamic_cast<OperatorToken *>(op);
    int tmp =  _op && (!priority ^ (_op->op == '*' || _op->op == '/'));
    return tmp;
}//改动1 ：！

// 鏍规嵁鍥涘垯杩愮畻娉曞垯褰掔害鏍囪瘑绗�
void compile(std::vector<Token *> &vec)
{
    for (int i = 0; i < vec.size(); ++i)
        if (vec[i]->type == NumType)
        {
            NumToken *num = dynamic_cast<NumToken *>(vec[i]);
            if (num)
                vec[i] = new ExprToken(num);
            else
                vec[i] = new ExprToken(new NumToken("0"));
        }
    while (vec.size() > 1)
    {
        bool flag = false;
        // 褰掔害锛歟xpr ::= '(' expr ')'
        for (int i = 0; i < vec.size() - 2; ++i)
        {
            int t1 =vec[i]->type, t2 = vec[i+1]->type, t3 = vec[i+2]->type;
            int tmp = vec[i]->type == LeftBracket && vec[i + 2]->type == RightBracket
                    && vec[i + 1]->type == ExprType;
            if (tmp)
            {
                flag = true;
                ExprToken *expr = new ExprToken(
                        dynamic_cast<BracketToken *>(vec[i]),
                        dynamic_cast<ExprToken *>(vec[i + 1]),
                        dynamic_cast<BracketToken *>(vec[i + 2]));
                vec.erase(vec.begin() + i + 1, vec.begin() + i + 3);
                vec[i] = expr;
            }
            if (flag) break;
        }

        
        if (flag) continue;
        // 褰掔害锛歟xpr ::= expr '*' expr
        // 褰掔害锛歟xpr ::= expr '/' expr
        for (int i = 0; i < vec.size() - 2; ++i)
        {
            int tmp = is_expr(vec[i], vec[i + 1], vec[i + 2], true);
            if (tmp) 
            {
                flag = true;
                ExprToken *expr = new ExprToken(
                        dynamic_cast<ExprToken *>(vec[i]),
                        dynamic_cast<OperatorToken *>(vec[i + 1]),
                        dynamic_cast<ExprToken *>(vec[i + 2]));
                vec.erase(vec.begin() + i + 1, vec.begin() + i + 3);
                vec[i] = expr;
            }
            if (flag) break;
        }
        if (flag) continue;
        // 褰掔害锛歟xpr ::= expr '+' expr
        // 褰掔害锛歟xpr ::= expr '-' expr
        for (int i = 0; i < vec.size() - 2; ++i)
        {
            int tmp = is_expr(vec[i], vec[i + 1], vec[i + 2], false);
            if (tmp)
            {
                if (i > 0 && vec[i - 1]->type == OperatorType
                        && (dynamic_cast<OperatorToken *>(vec[i - 1])->op == '*'
                            || dynamic_cast<OperatorToken *>(vec[i - 1])->op == '/'))
                    continue;
                if (i + 3 < vec.size() && vec[i + 3]->type == OperatorType
                        && (dynamic_cast<OperatorToken *>(vec[i + 3])->op == '*'
                            || dynamic_cast<OperatorToken *>(vec[i + 3])->op == '/'))
                    continue;
                flag = true;
                ExprToken *expr = new ExprToken(
                        dynamic_cast<ExprToken *>(vec[i]),
                        dynamic_cast<OperatorToken *>(vec[i + 1]),
                        dynamic_cast<ExprToken *>(vec[i + 2]));
                vec.erase(vec.begin() + i + 1, vec.begin() + i + 3);
                vec[i] = expr;
            }
            if (flag) break;
        }
        if (!flag) 
        {
            std::cout << "error" << std::endl;
            return ;
        }
    }
    
    PrintVistor v;
    vec[0]->visit(&v);
}



int main(int argc, char *argv[])
{
    //freopen("1.out", "r", stdin);
    std::string s;
    std::cin >> s;
    std::vector<Token *> vec = parse(s);
    compile(vec);
    return 0;
}