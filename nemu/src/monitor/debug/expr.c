#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
  /* TODO: Add more token types */
enum {
  TK_NOTYPE = 256, 
  TK_DEC, 
  TK_HEX, 
  TK_REG,
  TK_OR, 
  TK_AND, 
  TK_EQ, 
  TK_NEQ,
  TK_ADD, 
  TK_MIN, 
  TK_MUL, 
  TK_DIV,
  TK_NOT, 
  TK_POI, 
  TK_NEG,
  TK_LP, 
  TK_RP
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},
  {"0x[0-9a-fA-F]+", TK_HEX}, // hex
  {"[1-9][0-9]*|0", TK_DEC}, // dec
  {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip|ax|cx|dx|bx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh)", TK_REG}, // register
  {"\\+", TK_ADD},      // plus
  {"-", TK_MIN},        // minus
  {"\\*", TK_MUL},      // multiply
  {"/", TK_DIV},        // divide
  {"\\(", TK_LP},       // (
  {"\\)", TK_RP},       // )
  {"!=", TK_NEQ},       // not equal
  {"==", TK_EQ},        // equal
  {"&&", TK_AND},       // and
  {"\\|\\|", TK_OR},        // or
  {"!", TK_NOT},        // not

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: {
            break;
          }  
          case TK_ADD: {
            Token t;
            t.type = TK_ADD;
            t.str[0] = '+';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_MIN: {
            Token t;
            t.type = TK_MIN;
            t.str[0] = '-';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_MUL: {
            Token t;
            t.type = TK_MUL;
            t.str[0] = '*';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_DIV: {
            Token t;
            t.type = TK_DIV;
            t.str[0] = '/';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_LP: {
            Token t;
            t.type = TK_LP;
            t.str[0] = '(';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_RP: {
            Token t;
            t.type = TK_RP;
            t.str[0] = ')';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_DEC: {
            Token t;
            t.type = TK_DEC;
            int j;
            for (j = 0; j < substr_len; j++) {
              t.str[j] = *(substr_start + j);
            }
            t.str[j] = '\0';
            tokens[nr_token] = t;
            nr_token++;
            break;
          }
          case TK_HEX: {
            Token t;
            t.type = TK_HEX;
            int j;
            for (j = 0; j < substr_len; j++) {
              t.str[j] = *(substr_start + j);
            }
            t.str[j] = '\0';
            tokens[nr_token] = t;
            nr_token++;
            break;
          }
          case TK_REG: {
            Token t;
            t.type = TK_REG;
            int j;
            for (j = 0; j < substr_len; j++) {
              t.str[j] = *(substr_start + j);
            }
            t.str[j] = '\0';
            tokens[nr_token] = t;
            nr_token++;
            break;
          }
          case TK_NOT: {
            Token t;
            t.type = TK_NOT;
            t.str[0] = '!';
            t.str[1] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_EQ: {
            Token t;
            t.type = TK_EQ;
            t.str[0] = '=';
            t.str[1] = '=';
            t.str[2] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_NEQ: {
            Token t;
            t.type = TK_NEQ;
            t.str[0] = '!';
            t.str[1] = '=';
            t.str[2] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_AND: {
            Token t;
            t.type = TK_AND;
            t.str[0] = '&';
            t.str[1] = '&';
            t.str[2] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }
          case TK_OR: {
            Token t;
            t.type = TK_OR;
            t.str[0] = '|';
            t.str[1] = '|';
            t.str[2] = '\0';
            tokens[nr_token] = t;
            nr_token += 1;
            break;
          }        
          default: TODO();//NOT NEEDED
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int start, int end) {
  // Check if the first token is not '(' or the last token is not ')'
  if (tokens[start].type != TK_LP || tokens[end].type != TK_RP) {
    return false;
  }

  int balance = 0; // Balance counter for parentheses
  for (int i = start + 1; i < end; i++) {
    if (tokens[i].type == TK_LP) balance++;
    else if (tokens[i].type == TK_RP) {
      if (balance == 0) return false; // Extra closing parenthesis
      balance--;
    }
  }

  if (balance != 0) {
    printf("Parentheses are unbalanced\n");
    return false;
  }
  return true;
}


int find_right_parenthese(int start, int end) {
  int depth = 1; // Depth of nested parentheses
  for (int i = start; i <= end; i++) {
    if (tokens[i].type == TK_LP) depth++;
    else if (tokens[i].type == TK_RP) {
      depth--;
      if (depth == 0) return i; // Found the matching right parenthesis
    }
  }

  printf("Failed to find a matching right parenthesis\n");
  assert(false); // This should never happen if parentheses are balanced
  return -1;
}


// Compare the precedence of two operators
bool compare_priority(int op1, int op2) {
  // Equal precedence for equality and inequality operators
  if ((op1 == TK_EQ && op2 == TK_NEQ) || (op1 == TK_NEQ && op2 == TK_EQ)) {
    return true;
  }

  // Same for addition gdband subtraction
  if ((op1 == TK_ADD && op2 == TK_MIN) || (op1 == TK_MIN && op2 == TK_ADD)) {
    return true;
  }

  // And for multiplication and division
  if ((op1 == TK_MUL && op2 == TK_DIV) || (op1 == TK_DIV && op2 == TK_MUL)) {
    return true;
  }

  // Special case for unary operators
  if (op1 == op2) {
    return !(op1 == TK_NEG || op1 == TK_POI || op1 == TK_NOT);
  }

  // General case based on operator types
  return op1 > op2;
}


int find_operator(int p, int q) {
  int len = (q - p) + 1;
  int t = p;
  int count = 0;
  int loc = p;
  int operand = 0;
  while(t <= q && count <= len) {
    if (tokens[t].type == TK_LP) {
      t = find_right_parenthese(t+1, q);
    }
    else if (tokens[t].type == TK_RP) {
      printf("Wrong.\n");
	    assert(0);
   	}
    else if(tokens[t].type != TK_DEC && tokens[t].type != TK_HEX 
    && tokens[t].type != TK_REG && tokens[t].type != TK_NOTYPE) {
      if(count == 0) {
        loc = t;
        count++;
        operand = tokens[t].type;
      }
      else {
        if (compare_priority(operand, tokens[t].type)) {
          loc = t;
          operand = tokens[t].type;
        }
	  }
	}
    t++;
  }
  return loc;
}


int eval(int p, int q) {
  if (p > q) {
   //printf("Bad expression!\n");
   //printf("start: %d;end: %d.\n",p,q);
   return 0;
  }
  else if (p == q) {
    int res = 0;
    if (tokens[p].type == TK_HEX) {
      res = strtol(tokens[p].str, NULL, 16);
    }
    else if (tokens[p].type == TK_DEC) {
      res = atoi(tokens[p].str);
    }
    else if (tokens[p].type == TK_REG) {
      char tmp[4] = {tokens[p].str[1], tokens[p].str[2], tokens[p].str[3],'\0'};
      for(int i = 0; i < 8; i++) {
        if (!strcmp(tmp, regsl[i])) {return cpu.gpr[i]._32;}
      }
      char tmp1[3] = {tokens[p].str[1], tokens[p].str[2],'\0'};
      for(int i = 0; i < 8; i++) {
        if (!strcmp(tmp1, regsw[i])) {return cpu.gpr[i]._16;}
      }
      for(int i = 0; i < 8; i++) {
        if (!strcmp(tmp1, regsb[i])) {return cpu.gpr[i%4]._8[i/4];}
      }
      if (!strcmp(tmp, "eip")) {return cpu.eip;}
      else { 
        printf("Unknown register.\n"); 
        assert(0);
      }
    }

    return res;
  }
  else if (check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1);
  }
  else {
    int op = find_operator(p, q);// the position of dominant operator in the token expression
   	printf("dom op:%d.\n",op);
    int val1 = eval(p, op - 1);
    printf("val1:%d.\n",val1);
    int val2 = eval(op + 1, q);
    printf("val2:%d.\n",val2);
    switch (tokens[op].type) {
      case TK_ADD: return val1 + val2;
      case TK_MIN: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV: return val1 / val2;
      case TK_AND: {
        if (val1 != 0 && val2 != 0) {
          return true;
        }
        else {
          return false;
        }
      }
      case TK_OR: {
        if (val1 != 0 || val2 != 0) {
          //printf("1");
          return true;
        }
        else {
          //printf("2");
          return false;
        }
      }
      case TK_EQ: {
        if (val1 == val2){
          return true;
        }
        else {
          return false;
        }
      }
      case TK_NEQ: {
        if (val1 != val2) {
          return true;
        }
        else {
          return false;
        }
      }
      case TK_NEG: { return -1 * val2; }
      case TK_POI: { return vaddr_read(val2, 4); }
      case TK_NOT: { return !val2; }
      default: assert(0);
    }
  }
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == TK_MIN && (i == 0 || (tokens[i - 1].type != TK_DEC && tokens[i - 1].type  != TK_HEX && tokens[i - 1].type  != TK_REG && tokens[i - 1].type  != TK_RP))) {
        // diffrentiate the negative and minus
	   	tokens[i].type = TK_NEG;
    }
    else if (tokens[i].type == TK_MUL && (i == 0 || (tokens[i - 1].type != TK_DEC && tokens[i - 1].type  != TK_HEX && tokens[i - 1].type  != TK_REG && tokens[i - 1].type != TK_RP))) {
        // diffrentiate the pointer and multiply
		tokens[i].type = TK_POI;
    }
  }

  *success = true;
  //printf("Num of tokens:%d\n",nr_token);
  return eval(0, nr_token-1);
}
