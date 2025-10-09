#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <map>
#include <string>

#define ERROR_EXIT(s) printf(s);\
return false

enum TK_TYPE {
	TT_KEYWORD = 1 << 0,
	TT_INT = 1 << 1,
	TT_FLOAT = 1 << 2,
	TT_BRACKET_OPEN = 1 << 3,
	TT_BRACKET_CLOSE = 1 << 4,
	TT_PARENTHESIS_OPEN = 1 << 5,
	TT_PARENTHESIS_CLOSE = 1 << 6,
	TT_COMMA = 1 << 7,
	TT_IDENTIFIER = 1 << 8,
	TT_OPERATOR = 1 << 9,
};

enum KEYWORDS {
	KW_OTHER = 1 << 1,
	KW_FUNCTION = 1 << 2,
	KW_INT = 1 << 3,
	KW_FLOAT = 1 << 4,
	KW_TIMES = 1 << 5,
	KW_IF = 1 << 6,
	KW_WHILE = 1 << 7,
	KW_RETURN = 1 << 8,
};

enum VM_COMMAND {
	VMC_NULL, // Do nothing, literally
	VMC_NOP, // Do nothing for 1 frame
	VMC_NOP2, // Do nothing for x frames

	// Stack operations
	VMC_ENTER, // Move stack base pointer up to new routine
	VMC_STACKMOVE, // Move stack pointer
	VMC_LEAVE, // Move stack base pointer down to previous routine
	VMC_PUSHC, // Push constant

	VMC_PUSHR, // Push register
	VMC_PUSHL, // Push local variable
	VMC_PUSHG, // Push global variable

	VMC_POPR, // Pop to register
	VMC_POPL, // Pop to local variable
	VMC_POPG, // Pop to global variable

	VMC_MOVR, // Move constant to register
	VMC_MOVL, // Move constant to local variable
	VMC_MOVG, // Move constant to global variable

	VMC_ADDI, // r + r
	VMC_SUBI, // r - r
	VMC_MULI, // r * r
	VMC_DIVI, // r / r
	VMC_MODI, // r % r
	VMC_NEGI, // -r
	VMC_F2I, // ftoi(r) 

	VMC_SETPOS,
	VMC_MOVEPOS,
	VMC_MOVEDIR,
};

union number_t {
	float real;
	int integer;
};

struct command_struct_t {
	VM_COMMAND cmd;
	const char* cmd_name;
	std::vector<number_t> parameters;
};

struct std::vector<command_struct_t> commands = {
	{VMC_NOP, "Wait", {} },
	{VMC_NOP2, "WaitEx", {} },

	{VMC_ENTER, "Enter"},
	{VMC_STACKMOVE, "StackMove"},
	{VMC_LEAVE, "Leave"},

	{VMC_PUSHC, "PushC"},

	{VMC_PUSHR, "PushR"},
	{VMC_PUSHL, "PushL"},
	{VMC_PUSHG, "PushG"},

	{VMC_POPR, "PopR"},
	{VMC_POPL, "PopL"},
	{VMC_POPG, "PopG"},

	{VMC_MOVR, "MovR"},
	{VMC_MOVL, "MovL"},
	{VMC_MOVG, "MovG"},

	{VMC_ADDI, "AddI"},
	{VMC_SUBI, "SubI"},
	{VMC_MULI, "MulI"},
	{VMC_DIVI, "DivI"},
	{VMC_MODI, "ModI"},
	{VMC_NEGI, "NegI"},
	{VMC_F2I, "F2I"},

	{VMC_SETPOS, "SetPos"},
	{VMC_MOVEPOS, "MovePos"},
	{VMC_MOVEDIR, "MoveDir"},
};

const int max_stack = 128;
const int call_depth = 8;

// Virtual machine
struct vm_t {
	int* register_data; // A pointer to the registers data

	number_t a, b, c, d; // Acumulator, Base address?, Counter, Data

	int stack[max_stack]; // A stack to do operations in
	int frame_pointer; // The start of the stack in current call
	int stack_pointer; // The current size of the stack

	char* call_stack[call_depth]; // A stack to store return addresses
	int return_pointer; // Where should we go back when returning from call
};

struct token_t {
	std::string val;
	KEYWORDS keyword_id;
	TK_TYPE token_type;
	number_t number_val;
	int line;
};

struct subroutine_t {
	int size;
};

std::vector<token_t> tokens;
std::map<std::string, subroutine_t> function_names;
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
			while (c = *data) {
				if (IsAlphaNum(c)) {
					str.push_back(c);
					data++;
				}
				else {
					break;
				}
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

bool TranformBlockData(size_t* idx) {
	size_t size = tokens.size();
	size_t i = *idx;
	for (; i < size; ) {
		switch (tokens[i].token_type) {
		default: i++; break;
		case TT_IDENTIFIER: {
				for (auto& cmd : commands) {
					if(std::string(cmd.cmd_name) == tokens[i].val)
						printf("Found %s\n", cmd.cmd_name);
				}
				i++;
			}
			break;
		case TT_BRACKET_CLOSE: *idx = i; return true;
		}
	}
}

bool Transform() {
	size_t size = tokens.size();
	bool on_function = false;
	bool on_parameter_declare = false;
	std::string func_name = "";
	subroutine_t subr = { 0 };
	int next_token = TT_KEYWORD;
	int next_keyword = KW_FUNCTION;
	for (size_t i = 0; i < size; ) {
		if (!(next_token & tokens[i].token_type)) {
			ERROR_EXIT("This token was not expected at this time\n");
		}
		switch (tokens[i].token_type) {
		case TT_KEYWORD:
			if (!(next_keyword & tokens[i].keyword_id)) {
				ERROR_EXIT("This keyword was not expected at this time\n");
			}
			switch (tokens[i].keyword_id) {
			case KW_FUNCTION:
				if (on_function) {
					ERROR_EXIT("Can't declare a function inside another\n");
				}
				if (i + 1 < size && tokens[i + 1].token_type == TT_IDENTIFIER) { // Detect an identifier
					on_function = true;
					func_name = tokens[i + 1].val;
					i += 2;
					next_token = TT_PARENTHESIS_OPEN | TT_BRACKET_OPEN;
				}
				break;
			case KW_INT:
			case KW_FLOAT:
				if (on_parameter_declare) {
					if (i + 1 < size && (tokens[i + 1].token_type & TT_IDENTIFIER)) {
						next_token = TT_COMMA | TT_PARENTHESIS_CLOSE;
						i += 2;
					}
					else {
						ERROR_EXIT("Missing identifier maybe\n");
					}

				}
				else {
					ERROR_EXIT("Int or float declaration must be put inside brackets\n");
				}
				break;
			}
			break;
		case TT_BRACKET_OPEN:
			i++;
			if (false == TranformBlockData(&i)) return false;
			next_token = TT_BRACKET_CLOSE;
			subr = { 0 };

			break;
		case TT_BRACKET_CLOSE:
			next_token = TT_KEYWORD;
			next_keyword = KW_FUNCTION;
			on_function = false;
			i++;
			function_names.insert({std::move(func_name), std::move(subr)});
			break;
		case TT_PARENTHESIS_OPEN:
			next_token = TT_KEYWORD | TT_PARENTHESIS_CLOSE;
			next_keyword = KW_INT | KW_FLOAT;
			on_parameter_declare = true;
			i++;
			break;
		case TT_PARENTHESIS_CLOSE:
			next_token = TT_BRACKET_OPEN;
			on_parameter_declare = false;
			i++;
			break;
		default:
			i++;
			break;
		}
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
	printf("\n");
	printf("There are %d functions:\n", function_names.size());
	for (auto& s : function_names) {
		printf("Subroutine: %s\n", s.first.c_str());
	}
}