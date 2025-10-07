#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <string>

#define ERROR_EXIT(s) printf(s);\
return false

enum TK_TYPE {
	TT_KEYWORD,
	TT_INT,
	TT_FLOAT,
	TT_BRACKET_OPEN,
	TT_BRACKET_CLOSE,
	TT_PARENTHESIS_OPEN,
	TT_PARENTHESIS_CLOSE,
	TT_COMMA,
	TT_IDENTIFIER,
	TT_OPERATOR,
};

enum KEYWORDS {
	KW_OTHER,
	KW_FUNCTION,
	KW_INT,
	KW_FLOAT,
	KW_TIMES,
	KW_IF,
	KW_WHILE,
	KW_RETURN,
};

union number_t {
	float real;
	int integer;
};

struct token_t {
	std::string val;
	KEYWORDS keyword_id;
	TK_TYPE token_type;
	number_t number_val;
	int line;
};

std::vector<token_t> tokens;
std::vector<std::string> function_names;
const char* name = "test.scpt";


constexpr KEYWORDS GetType(const std::string& str) {
	if (str == "fn") return KW_FUNCTION;
	else if (str == "int") return KW_INT;
	else if (str == "float") return KW_FLOAT;
	else if (str == "times") return KW_TIMES;
	else if (str == "if") return KW_IF;
	else if (str == "while") return KW_WHILE;
	else if (str == "return") return KW_RETURN;
	return KW_OTHER;
}

constexpr bool IsLetterOrUnderscore(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'); }

constexpr bool IsAlphaNum(char c) { return IsLetterOrUnderscore(c) || (c >= '0' && c <= '9'); }

constexpr bool IsNumber(char c) { return (c >= '0' && c <= '9'); }

constexpr bool IsOperator(char c) { return (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '>' || c == '<'); }

constexpr bool IsBlankspace(char c) { return c == ' ' || c == '\t' || c == '\n'; }

struct translation_unit_t {
	int* tokens;
};

bool Tokenize(char* data) {
	char c = '\0';
	int line = 1;
	while (c = *data) {
		if (IsLetterOrUnderscore(c)) { // Identifier
			std::string str = "";
			while (IsAlphaNum(c = *data++)) {
				str.push_back(c);
			}
			KEYWORDS kwd = GetType(str);
			tokens.push_back({ std::move(str), kwd, kwd == KW_OTHER ? TT_IDENTIFIER : TT_KEYWORD, 0, line });
			str = "";
		}
		else if (c >= '0' && c <= '9') { // Number
			std::string str = "";
			bool found_dot = false;
			TK_TYPE nm = TT_INT;
			while (c = *data) {
				if (IsNumber(c)) {
					str.push_back(c);
					data++;
				}
				else if (c == '.') {
					if (found_dot) {
						printf("Line %d", line);
						ERROR_EXIT("Syntax Error");
					}
					else {
						nm = TT_FLOAT;
						str.push_back(c);
						found_dot = true;
						data++;
					}
				}
				else break;
			}
			tokens.push_back({ std::move(str), KW_OTHER, nm,  0, line });
			str = "";
		}
		else if (IsOperator(c)) {
			std::string str;
			str.push_back(c);
			number_t nm;
			nm.integer = c;
			tokens.push_back({ std::move(str), KW_OTHER, TT_OPERATOR, nm, line });
			data++;
		}
		else if (c == '{' || c == '}' || c == '(' || c == ')') {

			TK_TYPE tk;
			switch(c){
			case '{': tk = TT_BRACKET_OPEN; break;
			case '}': tk = TT_BRACKET_CLOSE; break;
			case '(': tk = TT_PARENTHESIS_OPEN; break;
			case ')': tk = TT_PARENTHESIS_CLOSE; break;
			}
			number_t nm;
			nm.integer = c;
			tokens.push_back({ "", KW_OTHER, tk, nm, line});
			data++;
		}
		else if (c == ',') {
			tokens.push_back({ "", KW_OTHER, TT_COMMA, 0, line });
			data++;
		}
		else if (IsBlankspace(c)) {
			if (c == '\n') line++;
			data++;
		}
		else {
			return false;
		}
	}
	return true;
}

bool Transform() {
	size_t size = tokens.size();
	for (auto& t : tokens) {

	}
	return true;
}

int main(int argc, char** argv) {
	if (argc < 1) {
		printf("Not enough arguments\n");
		return -1;
	}

	if (FILE* fp = fopen(name, "r")) {
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		rewind(fp);
		char* data = (char*)calloc(size + 1, sizeof(char));
		fread(data, size, sizeof(char), fp);
		fclose(fp);
		if (Tokenize(data)) {
			if (Transform()) {

			}
		}
		free(data);
	}

	printf("There are %d tokens:\n", tokens.size());
	for (auto& s : tokens) {
		printf("%s : Type - ", s.val.data());
		switch (s.token_type) {
		case TT_KEYWORD: printf("KEYWORD"); break;
		case TT_INT: printf("INT"); break;
		case TT_FLOAT: printf("FLOAT"); break;
		case TT_BRACKET_OPEN: printf("BRACKET_OPEN"); break;
		case TT_BRACKET_CLOSE: printf("BRACKET_CLOSE"); break;
		case TT_PARENTHESIS_OPEN: printf("PARENTHESIS_OPEN"); break;
		case TT_PARENTHESIS_CLOSE: printf("PARENTHESIS_CLOSE"); break;
		case TT_COMMA: printf("COMMA"); break;
		case TT_IDENTIFIER: printf("IDENTIFIER"); break;
		case TT_OPERATOR: printf("OPERATOR"); break;
		}
		printf("\n");
	}
}