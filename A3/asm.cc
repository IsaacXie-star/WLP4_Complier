#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "scanner.h"

/*
 * C++ Starter code for CS241 A3
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */

int stkPtr = 0;

class SymbolTable {
  std::map<std::string, int> mapping; // label to address

public:
  bool exist(std::string label) {
    if (mapping.find(label) != mapping.end()) {
      return true;
    }
    return false;
  }

  bool add(std::string label, int addr) {
    if (exist(label)) {
      return false;
    }
    mapping[label] = addr;
    return true;
  }

  int getAddr(std::string label) {
    if (!exist(label)) return -1;
    return mapping[label];
  }
};

void printBinary(int instr) {
  unsigned char c = instr >> 24;
  std::cout << c;
  c = instr >> 16; std::cout << c;
  c = instr >> 8; std::cout << c;
  c = instr; std::cout << c;
}

bool checkReg(int64_t reg) {
  if (reg < 0 || reg > 31) return false;
  return true;
}

bool checkImmedDec(int64_t i) {
  if (i < -32768 || i > 32767) return false;
  return true;
}

bool checkWordDec(int64_t i) {
  if (i < 0 && i < -2147483648) return false;
  if (i >= 0 && i > 4294967295) return false;
  return true;
}

bool checkImmedHex(int64_t i) {
  if (i < 0 || i > 65535) return false;
  return true;
}

bool checkWordHex(int64_t i) {
  if (i < 0 || i > 4294967295) return false;
  return true;
}

int buildAddIns(int d, int s, int t) {
  int instr = 0;
  instr |= (1 << 5);
  instr += (d << 11);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildSubIns(int d, int s, int t) {
  int instr = 0;
  instr |= (1 << 1);
  instr |= (1 << 5);
  instr += (d << 11);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildMultIns(int s, int t) {
  int instr = 0;
  instr |= (1 << 3);
  instr |= (1 << 4);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildUnsignMultIns(int s, int t) {
  int instr = 0;
  instr |= 1;
  instr |= (1 << 3);
  instr |= (1 << 4);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildDivIns(int s, int t) {
  int instr = 0;
  instr |= (1 << 1);
  instr |= (1 << 3);
  instr |= (1 << 4);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildUnsignDivIns(int s, int t) {
  int instr = 0;
  instr |= 1;
  instr |= (1 << 1);
  instr |= (1 << 3);
  instr |= (1 << 4);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildMFHRIns(int d) {
  int instr = 0;
  instr |= (1 << 4);
  instr += (d << 11);
  return instr;
}

int buildMFLQIns(int d) {
  int instr = 0;
  instr |= (1 << 1);
  instr |= (1 << 4);
  instr += (d << 11);
  return instr;
}

int buildLIASIns(int d) {
  int instr = 0;
  instr |= (1 << 2);
  instr |= (1 << 4);
  instr += (d << 11);
  return instr;
}

int buildLWIns(int t, int i, int s) {
  int instr = 0;
  instr |= (1 << 26);
  instr |= (1 << 27);
  instr |= (1 << 31);
  instr += (t << 16);
  instr += (s << 21);
  instr += (i & ((1 << 16) - 1));
  return instr;
}

int buildSWIns(int t, int i, int s) {
  int instr = 0;
  instr |= (1 << 26);
  instr |= (1 << 27);
  instr |= (1 << 29);
  instr |= (1 << 31);
  instr += (t << 16);
  instr += (s << 21);
  instr += (i & ((1 << 16) - 1));
  return instr;
}

int buildSLTIns(int d, int s, int t) {
  int instr = 0;
  instr |= (1 << 1);
  instr |= (1 << 3);
  instr |= (1 << 5);
  instr += (d << 11);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildSLTUnsignIns(int d, int s, int t) {
  int instr = 0;
  instr |= 1;
  instr |= (1 << 1);
  instr |= (1 << 3);
  instr |= (1 << 5);
  instr += (d << 11);
  instr += (t << 16);
  instr += (s << 21);
  return instr;
}

int buildBeqIns(int s, int t, int i) {
  int instr = 0;
  instr |= (1 << 28);
  instr += (t << 16);
  instr += (s << 21);
  instr += (i & ((1 << 16) - 1));
  return instr;
}

int buildBneIns(int s, int t, int i) {
  int instr = 0;
  instr |= (1 << 26);
  instr |= (1 << 28);
  instr += (t << 16);
  instr += (s << 21);
  instr += (i & ((1 << 16) - 1));
  return instr;
}

int buildJrIns(int s) {
  int instr = 0;
  instr |= (1 << 3);
  instr += (s << 21);
  return instr;
}

int buildJalrIns(int s) {
  int instr = 0;
  instr |= 1;
  instr |= (1 << 3);
  instr += (s << 21);
  return instr;
}

int main() {
  SymbolTable st;
  std::string line;
  try {
    // first pass, deal with labels
    std::vector<std::vector<Token>> tokenLines;
    std::vector<std::string> insLines;
    while (getline(std::cin, line)) {
      // This example code just prints the scanned tokens on each line.
      std::vector<Token> tokenLine = scan(line);
      if (tokenLine.size() == 0) continue;
      tokenLines.push_back(tokenLine);
      insLines.push_back(line);
      stkPtr += 4; // update stack pointer by 4

      // This code is just an example - you don't have to use a range-based
      // for loop in your actual assembler. You might find it easier to
      // use an index-based loop like this:
      // for(int i=0; i<tokenLine.size(); i++) { ... }
      // for (auto &token : tokenLine) {
      //   std::cerr << token << ' ';
      // }

      // Remove this when you're done playing with the example program!
      // Printing a random newline character as part of your machine code
      // output will cause you to fail the Marmoset tests.
      // std::cerr << std::endl;
      for (size_t i = 0; i < tokenLine.size(); i ++) {
        if (tokenLine[i].getKind() == Token::LABEL) {
          for (size_t j = 0; j < tokenLine.size() && tokenLine[j].getKind() == Token::LABEL; j ++) {
            std::string label = tokenLine[j].getLexeme();
            label = label.substr(0, label.size() - 1);
            if (st.exist(label)) {
              // error handle
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines.back());
            }
            st.add(label, stkPtr - 4);
            i = j;
          }
          for (size_t j = i + 1; j < tokenLine.size(); j ++) {
            if (tokenLine[j].getKind() == Token::LABEL) {
              // error handle
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines.back());
            }
          }
          if (i == tokenLine.size() - 1) {
            // this line only has labels
            stkPtr -= 4;
          }
          break;
        }
      }
    }

    stkPtr = 0;
    int lineIdx = 0;
    for (auto &tokenLine: tokenLines) {
      // Implementation goes below
      stkPtr += 4;
      for (size_t i = 0; i < tokenLine.size(); i ++) {
        auto &token1 = tokenLine[i];
        if (token1.getKind() == Token::WORD) {
          if (tokenLine.size() != i + 2) {
            // handle error
            throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
          }
          auto &token2 = tokenLine[i + 1];
          if (token2.getKind() != Token::INT && token2.getKind() != Token::HEXINT && token2.getKind() != Token::ID) {
            // handle error
            throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
          }
          int64_t num = 0;
          if (token2.getKind() == Token::INT || token2.getKind() == Token::HEXINT) {
            num = token2.toNumber();
            if (((token2.getKind() == Token::INT) && !checkWordDec(num))) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (((token2.getKind() == Token::HEXINT) && !checkWordHex(num))) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
          } else {
            if (st.exist(token2.getLexeme())) {
              num = st.getAddr(token2.getLexeme());
            } else {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
          }
          // translate to binary number
          printBinary(num);
          break;
        } else if (token1.getKind() == Token::ID) {
          std::string instr = token1.getLexeme();
          if (instr == "add") {

            if (tokenLine.size() != i + 6) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j < 6; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 4; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t d = tokenLine[i + 1].toNumber();
            int64_t s = tokenLine[i + 3].toNumber();
            int64_t t = tokenLine[i + 5].toNumber();
            if (!checkReg(d) || !checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int addIns = buildAddIns(d, s, t);
            printBinary(addIns);
            break;

          } else if (instr == "beq") {

            for (size_t j = 1; j <= 3; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 4; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            if (tokenLine[i + 5].getKind() != Token::INT && tokenLine[i + 5].getKind() != Token::HEXINT && tokenLine[i + 5].getKind() != Token::ID) {
              // handle error 
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t s = tokenLine[i + 1].toNumber();
            int64_t t = tokenLine[i + 3].toNumber();
            int64_t inum = 0;
            if (tokenLine[i + 5].getKind() == Token::ID) {
              if (st.exist(tokenLine[i + 5].getLexeme())) {
                inum = (st.getAddr(tokenLine[i + 5].getLexeme()) - stkPtr) / 4;
                if (!checkImmedDec(inum)) {
                  throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
                }
              } else {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            } else {
              inum = tokenLine[i + 5].toNumber();
            }
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (((tokenLine[i + 5].getKind() == Token::INT) && !checkImmedDec(inum))) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (((tokenLine[i + 5].getKind() == Token::HEXINT) && !checkImmedHex(inum))) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int beqIns = buildBeqIns(s, t, inum);
            printBinary(beqIns);
            break;

          } else if (instr == "sub") {
            
            if (tokenLine.size() != i + 6) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j < 6; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 4; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t d = tokenLine[i + 1].toNumber();
            int64_t s = tokenLine[i + 3].toNumber();
            int64_t t = tokenLine[i + 5].toNumber();
            if (!checkReg(d) || !checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int subIns = buildSubIns(d, s, t);
            printBinary(subIns);
            break;

          } else if (instr == "mult") {

            if (tokenLine.size() != i + 4) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j <= 3; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 2; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t s = tokenLine[i + 1].toNumber();
            int64_t t = tokenLine[i + 3].toNumber();
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int multIns = buildMultIns(s, t);
            printBinary(multIns);
            break;

          } else if (instr == "multu") {

            if (tokenLine.size() != i + 4) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j <= 3; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 2; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t s = tokenLine[i + 1].toNumber();
            int64_t t = tokenLine[i + 3].toNumber();
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int unsignmultIns = buildUnsignMultIns(s, t);
            printBinary(unsignmultIns);
            break;

          } else if (instr == "div") {

            if (tokenLine.size() != i + 4) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j <= 3; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 2; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t s = tokenLine[i + 1].toNumber();
            int64_t t = tokenLine[i + 3].toNumber();
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int divIns = buildDivIns(s, t);
            printBinary(divIns);
            break;

          } else if (instr == "divu") {

            if (tokenLine.size() != i + 4) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j <= 3; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 2; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t s = tokenLine[i + 1].toNumber();
            int64_t t = tokenLine[i + 3].toNumber();
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int unsignDivIns = buildUnsignDivIns(s, t);
            printBinary(unsignDivIns);
            break;

          } else if (instr == "mfhi") {

            if (tokenLine.size() != i + 2) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 1].getKind() != Token::REG) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t d = tokenLine[i + 1].toNumber();
            if (!checkReg(d)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int mfhiIns = buildMFHRIns(d);
            printBinary(mfhiIns);
            break;

          } else if (instr == "mflo") {

            if (tokenLine.size() != i + 2) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 1].getKind() != Token::REG) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t d = tokenLine[i + 1].toNumber();
            if (!checkReg(d)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int mfloIns = buildMFLQIns(d);
            printBinary(mfloIns);
            break;

          } else if (instr == "lis") {

            if (tokenLine.size() != i + 2) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 1].getKind() != Token::REG) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t d = tokenLine[i + 1].toNumber();
            if (!checkReg(d)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int lisIns = buildLIASIns(d);
            printBinary(lisIns);
            break;

          } else if (instr == "lw") {

            if (tokenLine.size() != i + 7) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 1].getKind() != Token::REG || tokenLine[i + 5].getKind() != Token::REG) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 4].getKind() != Token::LPAREN || tokenLine[i + 6].getKind() != Token::RPAREN) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 2].getKind() != Token::COMMA) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 3].getKind() != Token::INT && tokenLine[i + 3].getKind() != Token::HEXINT) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t s = tokenLine[i + 5].toNumber();
            int64_t t = tokenLine[i + 1].toNumber();
            int64_t inum = tokenLine[i + 3].toNumber();
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 3].getKind() == Token::INT && !checkImmedDec(inum)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 3].getKind() == Token::HEXINT && !checkImmedHex(inum)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int lwIns = buildLWIns(t, inum, s);
            printBinary(lwIns);
            break;

          } else if (instr == "sw") {

            if (tokenLine.size() != i + 7) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 1].getKind() != Token::REG || tokenLine[i + 5].getKind() != Token::REG) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 4].getKind() != Token::LPAREN || tokenLine[i + 6].getKind() != Token::RPAREN) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 2].getKind() != Token::COMMA) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 3].getKind() != Token::INT && tokenLine[i + 3].getKind() != Token::HEXINT) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t s = tokenLine[i + 5].toNumber();
            int64_t t = tokenLine[i + 1].toNumber();
            int64_t inum = tokenLine[i + 3].toNumber();
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 3].getKind() == Token::INT && !checkImmedDec(inum)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 3].getKind() == Token::HEXINT && !checkImmedHex(inum)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int swIns = buildSWIns(t, inum, s);
            printBinary(swIns);
            break;

          } else if (instr == "slt") {

            if (tokenLine.size() != i + 6) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j < 6; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 4; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t d = tokenLine[i + 1].toNumber();
            int64_t s = tokenLine[i + 3].toNumber();
            int64_t t = tokenLine[i + 5].toNumber();
            if (!checkReg(d) || !checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int sltIns = buildSLTIns(d, s, t);
            printBinary(sltIns);
            break;

          } else if (instr == "sltu") {

            if (tokenLine.size() != i + 6) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            for (size_t j = 1; j < 6; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 4; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            int64_t d = tokenLine[i + 1].toNumber();
            int64_t s = tokenLine[i + 3].toNumber();
            int64_t t = tokenLine[i + 5].toNumber();
            if (!checkReg(d) || !checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int unsignsltIns = buildSLTUnsignIns(d, s, t);
            printBinary(unsignsltIns);
            break;

          } else if (instr == "bne") {

            for (size_t j = 1; j <= 3; j += 2) {
              if (tokenLine[i + j].getKind() != Token::REG) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            for (size_t j = 2; j <= 4; j += 2) {
              if (tokenLine[i + j].getKind() != Token::COMMA) {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            }
            if (tokenLine[i + 5].getKind() != Token::INT && tokenLine[i + 5].getKind() != Token::HEXINT && tokenLine[i + 5].getKind() != Token::ID) {
              // handle error 
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t s = tokenLine[i + 1].toNumber();
            int64_t t = tokenLine[i + 3].toNumber();
            int64_t inum = 0;
            if (tokenLine[i + 5].getKind() == Token::ID) {
              if (st.exist(tokenLine[i + 5].getLexeme())) {
                inum = (st.getAddr(tokenLine[i + 5].getLexeme()) - stkPtr) / 4;
                if (!checkImmedDec(inum)) {
                  throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
                }
              } else {
                // handle error
                throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
              }
            } else {
              inum = tokenLine[i + 5].toNumber();
            }
            if (!checkReg(s) || !checkReg(t)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (((tokenLine[i + 5].getKind() == Token::INT) && !checkImmedDec(inum))) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (((tokenLine[i + 5].getKind() == Token::HEXINT) && !checkImmedHex(inum))) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int bneIns = buildBneIns(s, t, inum);
            printBinary(bneIns);
            break;

          } else if (instr == "jr") {

            if (tokenLine.size() != i + 2) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 1].getKind() != Token::REG) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t s = tokenLine[i + 1].toNumber();
            if (!checkReg(s)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int jrIns = buildJrIns(s);
            printBinary(jrIns);
            break;

          } else if (instr == "jalr") {

            if (tokenLine.size() != i + 2) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            if (tokenLine[i + 1].getKind() != Token::REG) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int64_t s = tokenLine[i + 1].toNumber();
            if (!checkReg(s)) {
              // handle error
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
            int jalrIns = buildJalrIns(s);
            printBinary(jalrIns);
            break;

          } else {
            // error
            throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
          }
        } else if (token1.getKind() == Token::LABEL) {
          for (size_t j = 0; j < tokenLine.size() && tokenLine[j].getKind() == Token::LABEL; j ++) {
            i = j;
          }
          for (size_t j = i + 1; j < tokenLine.size(); j ++) {
            if (tokenLine[j].getKind() == Token::LABEL) {
              // error handle
              throw ScanningFailure(std::string("ERROR: parse error in line: ") + insLines[lineIdx]);
            }
          }
          if (i == tokenLine.size() - 1) {
            // this line only has labels
            stkPtr -= 4;
          }
          continue;
        }
      }
      lineIdx ++;
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }
  // You can add your own catch clause(s) for other kinds of errors.
  // Throwing exceptions and catching them is the recommended way to
  // handle errors and terminate the program cleanly in C++. Do not
  // use the std::exit function, which may leak memory.
  
  return 0;
}