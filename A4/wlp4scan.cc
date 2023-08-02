#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <map>
#include <sstream>
#include <set>
#include <unordered_set>

using namespace std;

/* An exception class thrown when an error is encountered while scanning.
 */
class ScanningFailure {
    std::string message;

  public:
    ScanningFailure(std::string message): message(message) {}

    // Returns the message associated with the exception.
    const std::string &what() const {
      return message;
    }
};

class Token {
    public:

    enum Kind {
        ID = 0,     // a letter followed by zero/more letters/digits, but not equal to keywords
        NUM,        // valid int
        LPAREN,     // (
        RPAREN,     // )
        LBRACE,     // {
        RBRACE,     // }
        RETURN,     // return 
        IF,         // if
        ELSE,       // else
        WHILE,      // while
        PRINTLN,    // println
        WAIN,       // wain
        BECOMES,    // =
        INT,        // int
        EQ,         // ==
        NE,         // !=
        LT,         // <
        GT,         // >
        LE,         // <=
        GE,         // >=
        PLUS,       // +
        MINUS,      // -
        STAR,       // *
        SLASH,      // /
        PCT,        // %
        COMMA,      // ,
        SEMI,       // ;
        NEW,        // new
        DELETE,     // delete
        LBRACK,     // [
        RBRACK,     // ]
        AMP,        // &
        NIL,        // NULL
        SPACE,      // (ascii 32), not considered as output token
        // TAB,        // (ascii 9), not considered as output token
        // NEWLINE,    // (ascii 10), not considered as output token
        COMMENT     // string // followed by the chars up to and including the next NEWLINE, not considered as output token
    };

    private:

    Kind kind;
    std::string lexeme;

    public:

    // Constructor
    Token(Kind kind, std::string lexeme): kind(kind), lexeme(std::move(lexeme)) {
      if (kind == NUM) {
        if (!checkRange(this->lexeme)) {
          throw ScanningFailure("ERROR: Number out of range: " + this->lexeme);
        }
      }
    }

    Kind getKind() const { return kind; }

    const std::string& getLexeme() const { return lexeme; }

    bool checkRange(std::string& num) {
      long long base = (1ll << 31) - 1;
      long long x = 0;
      if (num.size() > 12) return false;
      for (int i = 0; i < num.size(); i ++) {
        x = 10 * x + (long long)(num[i] - '0');
      }
      return x <= base;
    }

};

std::ostream &operator<<(std::ostream &out, const Token &tok) {
    switch (tok.getKind()) {
        case Token::ID:         out << "ID";        break;
        case Token::NUM:        out << "NUM";       break;
        case Token::LPAREN:     out << "LPAREN";    break;   
        case Token::RPAREN:     out << "RPAREN";    break;
        case Token::LBRACE:     out << "LBRACE";    break;
        case Token::RBRACE:     out << "RBRACE";    break;
        case Token::RETURN:     out << "RETURN";    break;
        case Token::IF:         out << "IF";        break;
        case Token::ELSE:       out << "ELSE";      break;
        case Token::WHILE:      out << "WHILE";     break;
        case Token::PRINTLN:    out << "PRINTLN";   break;
        case Token::WAIN:       out << "WAIN";      break;
        case Token::BECOMES:    out << "BECOMES";   break;
        case Token::INT:        out << "INT";       break;
        case Token::EQ:         out << "EQ";        break;
        case Token::NE:         out << "NE";        break;
        case Token::LT:         out << "LT";        break;
        case Token::GT:         out << "GT";        break;
        case Token::LE:         out << "LE";        break;
        case Token::GE:         out << "GE";        break;
        case Token::PLUS:       out << "PLUS";      break;
        case Token::MINUS:      out << "MINUS";     break;
        case Token::STAR:       out << "STAR";      break;
        case Token::SLASH:      out << "SLASH";     break;
        case Token::PCT:        out << "PCT";       break;
        case Token::COMMA:      out << "COMMA";     break;
        case Token::SEMI:       out << "SEMI";      break;
        case Token::NEW:        out << "NEW";       break;
        case Token::DELETE:     out << "DELETE";    break;
        case Token::LBRACK:     out << "LBRACK";    break;
        case Token::RBRACK:     out << "RBRACK";    break;
        case Token::AMP:        out << "AMP";       break;
        case Token::NIL:        out << "NULL";      break;
        case Token::SPACE:      out << "SPACE";     break;
        // case Token::TAB:        out << "TAB";       break;
        // case Token::NEWLINE:    out << "NEWLINE";   break;
        case Token::COMMENT:    out << "COMMENT";   break;
    }
    out << " " << tok.getLexeme();
    return out;
}

class Wlp4DFA {
  public:
    enum State {
      // States that are also kinds
      ID = 0,
      NUM,
      RPAREN,
      LPAREN,
      LBRACE,
      RBRACE,
      RETURN,
      IF,
      ELSE,
      WHILE,
      PRINTLN,
      WAIN,
      BECOMES,
      INT,
      EQ,
      NE,
      LT,
      GT,
      LE,
      GE,
      PLUS,
      MINUS,
      STAR,
      SLASH,
      PCT,
      COMMA,
      SEMI,
      NEW,
      DELETE,
      LBRACK,
      RBRACK,
      AMP,
      NIL,
      SPACE,
      // TAB,
      // NEWLINE,
      COMMENT,

      // States that are not also kinds
      FAIL,
      START,
      NOT,          // !
      WAINWHILE1,   // w
      WAIN2,        // a
      WAIN3,        // i
      INTIF1,       // i
      INT2,         // n
      ELSE1,        // e
      ELSE2,        // l
      ELSE3,        // s
      WHILE2,       // h
      WHILE3,       // i
      WHILE4,       // l
      PRINTLN1,     // p
      PRINTLN2,     // r
      PRINTLN3,     // i
      PRINTLN4,     // n
      PRINTLN5,     // t
      PRINTLN6,     // l
      RETURN1,      // r
      RETURN2,      // e
      RETURN3,      // t
      RETURN4,      // u
      RETURN5,      // r 
      NULL1,        // N
      NULL2,        // U
      NULL3,        // L
      NEW1,         // n
      NEW2,         // e
      DELETE1,      // d
      DELETE2,      // e
      DELETE3,      // l
      DELETE4,      // e
      DELETE5,      // t
      SINGLEDIG,    // single digit

      // Hack to let this be used easily in arrays. This should always be the
      // final element in the enum, and should always point to the previous
      // element.

      LARGEST_STATE = SINGLEDIG
    };

  private:
    /* A set of all accepting states for the DFA.
     * Currently non-accepting states are not actually present anywhere
     * in memory, but a list can be found in the constructor.
     */
    std::set<State> acceptingStates;

    /*
     * The transition function for the DFA, stored as a map.
     */

    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;

    /*
     * Converts a state to a kind to allow construction of Tokens from States.
     * Throws an exception if conversion is not possible.
     */
    Token::Kind stateToKind(State s) const {
      switch(s) {
        case ID:         return Token::ID;
        case NUM:        return Token::NUM;
        case RPAREN:     return Token::RPAREN;
        case LPAREN:     return Token::LPAREN;
        case LBRACE:     return Token::LBRACE;
        case RBRACE:     return Token::RBRACE;
        case RETURN:     return Token::RETURN;
        case IF:         return Token::IF;
        case ELSE:       return Token::ELSE;
        case WHILE:      return Token::WHILE;
        case PRINTLN:    return Token::PRINTLN;
        case WAIN:       return Token::WAIN;
        case BECOMES:    return Token::BECOMES;
        case INT:        return Token::INT;
        case EQ:         return Token::EQ;
        case NE:         return Token::NE;
        case LT:         return Token::LT;
        case GT:         return Token::GT;
        case LE:         return Token::LE;
        case GE:         return Token::GE;
        case PLUS:       return Token::PLUS;
        case MINUS:      return Token::MINUS;
        case STAR:       return Token::STAR;
        case SLASH:      return Token::SLASH;
        case PCT:        return Token::PCT;
        case COMMA:      return Token::COMMA;
        case SEMI:       return Token::SEMI;
        case NEW:        return Token::NEW;
        case DELETE:     return Token::DELETE;
        case LBRACK:     return Token::LBRACK;
        case RBRACK:     return Token::RBRACK;
        case AMP:        return Token::AMP;
        case NIL:        return Token::NIL;
        case SPACE:      return Token::SPACE;
        // case TAB:       return Token::TAB;
        // case NEWLINE:   return Token::NEWLINE;
        case COMMENT:    return Token::COMMENT;
        // default: throw ScanningFailure("ERROR: Cannot convert state to kind.");

        case WAINWHILE1: return Token::ID;
        case WAIN2:      return Token::ID;
        case WAIN3:      return Token::ID;
        case INTIF1:     return Token::ID;
        case INT2:       return Token::ID;
        case ELSE1:      return Token::ID;
        case ELSE2:      return Token::ID;
        case ELSE3:      return Token::ID;
        case WHILE2:     return Token::ID;
        case WHILE3:     return Token::ID;
        case WHILE4:     return Token::ID;
        case PRINTLN1:   return Token::ID;
        case PRINTLN2:   return Token::ID;
        case PRINTLN3:   return Token::ID;
        case PRINTLN4:   return Token::ID;
        case PRINTLN5:   return Token::ID;
        case PRINTLN6:   return Token::ID;
        case RETURN1:    return Token::ID;
        case RETURN2:    return Token::ID;
        case RETURN3:    return Token::ID;
        case RETURN4:    return Token::ID;
        case RETURN5:    return Token::ID;
        case NULL1:      return Token::ID;
        case NULL2:      return Token::ID;
        case NULL3:      return Token::ID;
        case NEW1:       return Token::ID;
        case NEW2:       return Token::ID;
        case DELETE1:    return Token::ID;
        case DELETE2:    return Token::ID;
        case DELETE3:    return Token::ID;
        case DELETE4:    return Token::ID;
        case DELETE5:    return Token::ID;
        case SINGLEDIG:  return Token::NUM;
      }
      return Token::ID; // should not reach
    }


  public:
    /* Tokenizes an input string according to the Simplified Maximal Munch
     * scanning algorithm.
     */
    std::vector<Token> simplifiedMaximalMunch(const std::string &input) const {
      std::vector<Token> result;

      State state = start();
      std::string munchedInput;

      // We can't use a range-based for loop effectively here
      // since the iterator doesn't always increment.
      for (std::string::const_iterator inputPosn = input.begin();
           inputPosn != input.end();) {

        State oldState = state;
        state = transition(state, *inputPosn);

        if (!failed(state)) {
          munchedInput += *inputPosn;
          oldState = state;

          ++inputPosn;
        }

        if (inputPosn == input.end() || failed(state)) {
          if (accept(oldState)) {
            result.push_back(Token(stateToKind(oldState), munchedInput));

            munchedInput = "";
            state = start();
          } else {
            if (failed(state)) {
              munchedInput += *inputPosn;
            }
            throw ScanningFailure("ERROR: Simplified maximal munch failed on input: "
                                 + munchedInput);
          }
        }
      }

      return result;
    }

    /* Initializes the accepting states for the DFA.
     */
    Wlp4DFA() {
      acceptingStates = {ID, NUM, LPAREN, RPAREN, LBRACE, RBRACE, RETURN, IF, ELSE,
                          WHILE, PRINTLN, WAIN, BECOMES, INT, EQ, NE, LT, GT, LE, GE,
                          PLUS, MINUS, STAR, SLASH, PCT, COMMA, SEMI, NEW, DELETE,
                          LBRACK, RBRACK, AMP, NIL, SPACE, COMMENT,
                          WAINWHILE1, WAIN2, WAIN3, INTIF1, INT2, ELSE1, ELSE2, ELSE3, 
                          WHILE2, WHILE3, WHILE4, PRINTLN1, PRINTLN2, PRINTLN3, PRINTLN4, 
                          PRINTLN5, PRINTLN6, RETURN1, RETURN2, RETURN3, RETURN4, RETURN5,
                          NULL1, NULL2, NULL3, NEW1, NEW2, DELETE1, DELETE2, DELETE3,
                          DELETE4, DELETE5, SINGLEDIG};
      //Non-accepting states are DOT, MINUS, ZEROX, DOLLARS, START

      // Initialize transitions for the DFA
      for (size_t i = 0; i < transitionFunction.size(); ++i) {
        for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
          transitionFunction[i][j] = FAIL;
        }
      }

      registerTransition(START, "(", LPAREN);
      registerTransition(START, ")", RPAREN);
      registerTransition(START, "{", LBRACE);
      registerTransition(START, "}", RBRACE);
      registerTransition(START, "=", BECOMES);
      registerTransition(START, "!", NOT);
      registerTransition(START, "<", LT);
      registerTransition(START, ">", GT);
      registerTransition(START, "+", PLUS);
      registerTransition(START, "-", MINUS);
      registerTransition(START, "*", STAR);
      registerTransition(START, "/", SLASH);
      registerTransition(START, "%", PCT);
      registerTransition(START, ",", COMMA);
      registerTransition(START, ";", SEMI);
      registerTransition(START, "[", LBRACK);
      registerTransition(START, "]", RBRACK);
      registerTransition(START, "&", AMP);
      registerTransition(START, "0", SINGLEDIG);
      registerTransition(START, "w", WAINWHILE1);
      registerTransition(START, "i", INTIF1);
      registerTransition(START, "e", ELSE1);
      registerTransition(START, "p", PRINTLN1);
      registerTransition(START, "r", RETURN1);
      registerTransition(START, "N", NULL1);
      registerTransition(START, "n", NEW1);
      registerTransition(START, "d", DELETE1);
      registerTransition(START, "abcfghjklmoqstuvxyz", ID);
      registerTransition(START, "ABCDEFGHIJKLMOPQRSTUVWXYZ", ID);
      registerTransition(START, " ", SPACE);
      registerTransition(SLASH, "/", COMMENT);
      registerTransition(START, "123456789", NUM);
      registerTransition(COMMENT, "abcdefghijklmnopqrstuvwxyz", COMMENT);
      registerTransition(COMMENT, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", COMMENT);
      registerTransition(COMMENT, "0123456789", COMMENT);
      registerTransition(COMMENT, " ,.':;/_+-*/!@#$%^&*()[]{}?<>=~|\\", COMMENT);
      registerTransition(BECOMES, "=", EQ);
      registerTransition(NOT, "=", NE);
      registerTransition(LT, "=", LE);
      registerTransition(GT, "=", GE);
      registerTransition(NUM, "0123456789", NUM);
      registerTransition(ID, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(ID, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(ID, "0123456789", ID);
      registerTransition(WAINWHILE1, "a", WAIN2);
      registerTransition(WAINWHILE1, "h", WHILE2);
      registerTransition(WAINWHILE1, "bcdefgijklmnopqrstuvwxyz", ID);
      registerTransition(WAINWHILE1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WAINWHILE1, "0123456789", ID);
      registerTransition(WAIN2, "i", WAIN3);
      registerTransition(WAIN2, "abcdefghjklmnopqrstuvwxyz", ID);
      registerTransition(WAIN2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WAIN2, "0123456789", ID);
      registerTransition(WAIN3, "n", WAIN);
      registerTransition(WAIN3, "abcdefghijklmopqrstuvwxyz", ID);
      registerTransition(WAIN3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WAIN3, "0123456789", ID);
      registerTransition(WHILE2, "i", WHILE3);
      registerTransition(WHILE2, "abcdefghjklmnopqrstuvwxyz", ID);
      registerTransition(WHILE2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WHILE2, "0123456789", ID);
      registerTransition(WHILE3, "l", WHILE4);
      registerTransition(WHILE3, "abcdefghijkmnopqrstuvwxyz", ID);
      registerTransition(WHILE3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WHILE3, "0123456789", ID);
      registerTransition(WHILE4, "e", WHILE);
      registerTransition(WHILE4, "abcdfghijklmnopqrstuvwxyz", ID);
      registerTransition(WHILE4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WHILE4, "0123456789", ID);
      registerTransition(INTIF1, "n", INT2);
      registerTransition(INTIF1, "f", IF);
      registerTransition(INTIF1, "abcdeghijklmopqrstuvwxyz", ID);
      registerTransition(INTIF1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(INTIF1, "0123456789", ID);
      registerTransition(INT2, "t", INT);
      registerTransition(INT2, "abcdefghijklmnopqrsuvwxyz", ID);
      registerTransition(INT2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(INT2, "0123456789", ID);
      registerTransition(ELSE1, "l", ELSE2);
      registerTransition(ELSE1, "abcdefghijkmnopqrstuvwxyz", ID);
      registerTransition(ELSE1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(ELSE1, "0123456789", ID);
      registerTransition(ELSE2, "s", ELSE3);
      registerTransition(ELSE2, "abcdefghijklmnopqrtuvwxyz", ID);
      registerTransition(ELSE2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(ELSE2, "0123456789", ID);
      registerTransition(ELSE3, "e", ELSE);
      registerTransition(ELSE3, "abcdfghijklmnopqrstuvwxyz", ID);
      registerTransition(ELSE3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(ELSE3, "0123456789", ID);
      registerTransition(PRINTLN1, "r", PRINTLN2);
      registerTransition(PRINTLN1, "abcdefghijklmnopqstuvwxyz", ID);
      registerTransition(PRINTLN1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(PRINTLN1, "0123456789", ID);
      registerTransition(PRINTLN2, "i", PRINTLN3);
      registerTransition(PRINTLN2, "abcdefghjklmnopqrstuvwxyz", ID);
      registerTransition(PRINTLN2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(PRINTLN2, "0123456789", ID);
      registerTransition(PRINTLN3, "n", PRINTLN4);
      registerTransition(PRINTLN3, "abcdefghijklmopqrstuvwxyz", ID);
      registerTransition(PRINTLN3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(PRINTLN3, "0123456789", ID);
      registerTransition(PRINTLN4, "t", PRINTLN5);
      registerTransition(PRINTLN4, "abcdefghijklmnopqrsuvwxyz", ID);
      registerTransition(PRINTLN4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(PRINTLN4, "0123456789", ID);
      registerTransition(PRINTLN5, "l", PRINTLN6);
      registerTransition(PRINTLN5, "abcdefghijkmnopqrstuvwxyz", ID);
      registerTransition(PRINTLN5, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(PRINTLN5, "0123456789", ID);
      registerTransition(PRINTLN6, "n", PRINTLN);
      registerTransition(PRINTLN6, "abcdefghijklmopqrstuvwxyz", ID);
      registerTransition(PRINTLN6, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(PRINTLN6, "0123456789", ID);
      registerTransition(RETURN1, "e", RETURN2);
      registerTransition(RETURN1, "abcdfghijklmnopqrstuvwxyz", ID);
      registerTransition(RETURN1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(RETURN1, "0123456789", ID);
      registerTransition(RETURN2, "t", RETURN3);
      registerTransition(RETURN2, "abcdefghijklmnopqrsuvwxyz", ID);
      registerTransition(RETURN2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(RETURN2, "0123456789", ID);
      registerTransition(RETURN3, "u", RETURN4);
      registerTransition(RETURN3, "abcdefghijklmnopqrstvwxyz", ID);
      registerTransition(RETURN3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(RETURN3, "0123456789", ID);
      registerTransition(RETURN4, "r", RETURN5);
      registerTransition(RETURN4, "abcdefghijklmnopqstuvwxyz", ID);
      registerTransition(RETURN4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(RETURN4, "0123456789", ID);
      registerTransition(RETURN5, "n", RETURN);
      registerTransition(RETURN5, "abcdefghijklmopqrstuvwxyz", ID);
      registerTransition(RETURN5, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(RETURN5, "0123456789", ID);
      registerTransition(DELETE1, "e", DELETE2);
      registerTransition(DELETE1, "abcdfghijklmnopqrstuvwxyz", ID);
      registerTransition(DELETE1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(DELETE1, "0123456789", ID);
      registerTransition(DELETE2, "l", DELETE3);
      registerTransition(DELETE2, "abcdefghijkmnopqrstuvwxyz", ID);
      registerTransition(DELETE2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(DELETE2, "0123456789", ID);
      registerTransition(DELETE3, "e", DELETE4);
      registerTransition(DELETE3, "abcdfghijklmnopqrstuvwxyz", ID);
      registerTransition(DELETE3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(DELETE3, "0123456789", ID);
      registerTransition(DELETE4, "t", DELETE5);
      registerTransition(DELETE4, "abcdefghijklmnopqrsuvwxyz", ID);
      registerTransition(DELETE4, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(DELETE4, "0123456789", ID);
      registerTransition(DELETE5, "e", DELETE);
      registerTransition(DELETE5, "abcdfghijklmnopqrstuvwxyz", ID);
      registerTransition(DELETE5, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(DELETE5, "0123456789", ID);
      registerTransition(NULL1, "U", NULL2);
      registerTransition(NULL1, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(NULL1, "ABCDEFGHIJKLMNOPQRSTVWXYZ", ID);
      registerTransition(NULL1, "0123456789", ID);
      registerTransition(NULL2, "L", NULL3);
      registerTransition(NULL2, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(NULL2, "ABCDEFGHIJKMNOPQRSTUVWXYZ", ID);
      registerTransition(NULL2, "0123456789", ID);
      registerTransition(NULL3, "L", NIL);
      registerTransition(NULL3, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(NULL3, "ABCDEFGHIJKMNOPQRSTUVWXYZ", ID);
      registerTransition(NULL3, "0123456789", ID);
      registerTransition(NEW1, "e", NEW2);
      registerTransition(NEW1, "abcdfghijklmnopqrstuvwxyz", ID);
      registerTransition(NEW1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(NEW1, "0123456789", ID);
      registerTransition(NEW2, "w", NEW);
      registerTransition(NEW2, "abcdefghijklmnopqrstuvxyz", ID);
      registerTransition(NEW2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(NEW2, "0123456789", ID);
      registerTransition(WAIN, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(WAIN, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WAIN, "0123456789", ID);
      registerTransition(INT, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(INT, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(INT, "0123456789", ID);
      registerTransition(IF, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(IF, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(IF, "0123456789", ID);
      registerTransition(ELSE, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(ELSE, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(ELSE, "0123456789", ID);
      registerTransition(WHILE, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(WHILE, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(WHILE, "0123456789", ID);
      registerTransition(PRINTLN, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(PRINTLN, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(PRINTLN, "0123456789", ID);
      registerTransition(RETURN, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(RETURN, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(RETURN, "0123456789", ID);
      registerTransition(NIL, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(NIL, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(NIL, "0123456789", ID);
      registerTransition(NEW, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(NEW, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(NEW, "0123456789", ID);
      registerTransition(DELETE, "abcdefghijklmnopqrstuvwxyz", ID);
      registerTransition(DELETE, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", ID);
      registerTransition(DELETE, "0123456789", ID);
    }

    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
        State newState) {
      if (chars == "ALLSTRING") {
        for (char c = 0; c < 128; c ++) {
          if (c != 10) {
            transitionFunction[oldState][c] = newState;
          }
        }
        return;
      }

      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {

      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }

    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }

    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
      return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};


int main() {

    Wlp4DFA dfa = Wlp4DFA();
    std::string line;
    while (std::getline(std::cin, line)) {
      try {
        std::vector<Token> tokens = dfa.simplifiedMaximalMunch(line);
        for (Token& tok : tokens) {
          if ((tok.getKind() == Token::SPACE) || (tok.getKind() == Token::COMMENT)) {
            continue;
          }
          std::cout << tok << std::endl;
        }
      } catch(ScanningFailure& sf) {
        std::cerr << sf.what() << std::endl;
        break;
      }
    }
    return 0;
}
