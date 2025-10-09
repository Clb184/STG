#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <map>
#include <string>
#include <assert.h>

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

	VMC_CALL, // Call a function
	VMC_RETURN, // Return from function
	VMC_KILL, // Kill the object related to the VM

	// Stack operations
	VMC_ENTER, // Move stack base pointer up to new routine
	VMC_STACKMOVE, // Move stack pointer
	VMC_LEAVE, // Move stack base pointer down to previous routine
	VMC_PUSHC, // Push constant
	VMC_STACKCLEAR, // Clear the stack

	VMC_PUSHR, // Push register
	VMC_PUSHL, // Push local variable
	VMC_PUSHG, // Push global variable

	VMC_POPR, // Pop to register
	VMC_POPL, // Pop to local variable
	VMC_POPG, // Pop to global variable

	VMC_SETR, // Set constant to register
	VMC_SETL, // Set constant to local variable
	VMC_SETG, // Set constant to global variable

	VMC_ADDI, // r + r
	VMC_SUBI, // r - r
	VMC_MULI, // r * r
	VMC_DIVI, // r / r
	VMC_MODI, // r % r
	VMC_NEGI, // -r
	VMC_F2I, // ftoi(r) 

	VMC_ADDF, // r + r
	VMC_SUBF, // r - r
	VMC_MULF, // r * r
	VMC_DIVF, // r / r
	VMC_MODF, // r % r
	VMC_NEGF, // -r
	VMC_I2F, // ftoi(r) 

	VMC_SETPOS,
	VMC_MOVEPOS,
	VMC_MOVEDIR,
};

union number_t {
	float real;
	int integer;
	void operator=(int i) {
		integer = i;
	}
	void operator=(float i) {
		real = i;
	}

	operator float() {
		return real;
	}

	operator int() {
		return integer;
	}

};

struct command_struct_t {
	VM_COMMAND cmd;
	int num_args;
};

struct std::map<const std::string, command_struct_t> commands = {
	{"Nop",      {VMC_NOP, 0}},
	{"Nop2",     {VMC_NOP2, 1}},

	{"Call",      {VMC_CALL, 1}},
	{"Return",    {VMC_RETURN, 0}},
	{"Kill",      {VMC_KILL, 0}},

	{"Enter",     {VMC_ENTER, 1}},
	{"StackMove", {VMC_STACKMOVE, 1}},
	{"Leave",     {VMC_LEAVE, }},

	{"PushC",     {VMC_PUSHC, 1}},
	{"StackClear",  {VMC_STACKCLEAR, 1}},

	{"PushR",     {VMC_PUSHR, 1}},
	{"PushL",     {VMC_PUSHL, 1}},
	{"PushG",     {VMC_PUSHG, 1}},

	{"PopR",      {VMC_POPR, 1}},
	{"PopL",      {VMC_POPL, 1}},
	{"PopG",      {VMC_POPG, 1}},

	{"SetR",      {VMC_SETR, 2}},
	{"SetL",      {VMC_SETL, 2}},
	{"SetG",      {VMC_SETG, 2}},

	{"AddI",      {VMC_ADDI, 0}},
	{"SubI",      {VMC_SUBI, 0}},
	{"MulI",      {VMC_MULI, 0}},
	{"DivI",      {VMC_DIVI, 0}},
	{"ModI",      {VMC_MODI, 0}},
	{"NegI",      {VMC_NEGI, 0}},
	{"F2I",       {VMC_F2I, 0}},

	{"AddF",      {VMC_ADDF, 0}},
	{"SubF",      {VMC_SUBF, 0}},
	{"MulF",      {VMC_MULF, 0}},
	{"DivF",      {VMC_DIVF, 0}},
	{"ModF",      {VMC_MODF, 0}},
	{"NegF",      {VMC_NEGF, 0}},
	{"I2F",       {VMC_I2F, 0}},

	{"SetPos",    {VMC_SETPOS, 2}},
	{"MovePos",   {VMC_MOVEPOS, 4}},
	{"MoveDir",   {VMC_MOVEDIR, 3}},
};

const int max_locals = 64;
const int max_calcstack = 128;
const int call_depth = 8;

struct calcstack_t {
	number_t stack[max_calcstack];
	int stack_pointer;
} calc_stack;

void StkInitialize() {
	memset(calc_stack.stack, 0, sizeof(number_t) * max_calcstack);
	calc_stack.stack_pointer = 0;
}

void StkPush(int val) {
	if (calc_stack.stack_pointer < max_calcstack) {
		calc_stack.stack[calc_stack.stack_pointer].integer = val;
		calc_stack.stack_pointer++;
	}
	else {
		printf("Stack Overflow\n");
	}
}

int StkPop() {
	if (calc_stack.stack_pointer > 0) {
		calc_stack.stack_pointer--;
		return calc_stack.stack[calc_stack.stack_pointer].integer;
	}
	else {
		printf("Stack Underflow\n");
	}
	return -1;
}

template<typename T, typename function>
void StkDualOp(function fn) {
	int stack_pointer = calc_stack.stack_pointer;
	if (stack_pointer >= 2) {
		stack_pointer-= 2;
		number_t* stack = calc_stack.stack;
		stack[stack_pointer] = (T)fn((T)stack[stack_pointer], (T)stack[stack_pointer + 1]);
		calc_stack.stack_pointer = stack_pointer + 1;
	}
}

template<typename T, typename function>
void StkSingleOp(function fn) {
	int stack_pointer = calc_stack.stack_pointer;
	if (stack_pointer >= 1) {
		stack_pointer--;
		number_t* stack = calc_stack.stack;
		stack[stack_pointer] = (T)fn((T)stack[stack_pointer]);
	}
}

void StkF2I() {
	int stack_pointer = calc_stack.stack_pointer;
	if (stack_pointer >= 1) {
		stack_pointer--;
		number_t* stack = calc_stack.stack;
		stack[stack_pointer].integer = stack[stack_pointer].real;
	}
}

void StkI2F() {
	int stack_pointer = calc_stack.stack_pointer;
	if (stack_pointer >= 1) {
		stack_pointer--;
		calc_stack.stack[stack_pointer].real = calc_stack.stack[stack_pointer].integer;
	}
}

// Virtual machine
struct vm_t {
	int* register_data; // A pointer to the registers data
	int* global_reg_data; // A pointer to global registers data
	char* command_data; // A pointer to the command data

	number_t a, b, c, d; // Acumulator, Base address?, Counter, Data

	number_t locals[max_locals]; // A stack to store local variables and past frame_pointer
	int frame_pointer; // The start of the stack in current call
	int stack_pointer; // The current size of the stack
	int num_locals;

	char* call_stack[call_depth]; // A stack to store return addresses
	int return_pointer; // Where should we go back when returning from call
};

void InitVM(vm_t* vm, char* cmd_data) {
	assert(nullptr != vm);

	memset(vm, 0, sizeof(vm_t));
	vm->command_data = cmd_data;
}

void RunVM(vm_t* vm) {
	assert(nullptr != vm);

	// Registers
	number_t a = vm->a, b = vm->b, c = vm->c, d = vm->d;
	number_t* stack = vm->locals;
	int frame_pointer = vm->frame_pointer;
	int stack_pointer = vm->stack_pointer;

	// Commands
	char* cmd = vm->command_data;
cmd_begin:
	switch (*(uint16_t*)cmd) {
	case VMC_NOP:
		cmd += 2;
		break;
	case VMC_NOP2: {
		int wait = StkPop();
		cmd += 2;
	}
		break;

	case VMC_KILL:
		goto cmd_exit;
		break;

	case VMC_ENTER:
		stack_pointer += *((int*)(cmd + 2));
		cmd += 2 + sizeof(int);
		break;
	case VMC_PUSHC:
		StkPush(*((int*)(cmd + 2)));
		cmd += 2 + sizeof(int);
		break;
	case VMC_PUSHL:
		StkPush(stack[frame_pointer + *((int*)(cmd + 2))].integer);
		cmd += 2 + sizeof(int);
		break;
	case VMC_STACKCLEAR:
		cmd += 2;
		break;
	case VMC_SETL: {
		int reg = *((int*)(cmd + 2));
		stack[frame_pointer + reg].integer = *((int*)(cmd + 2 + sizeof(int)));
		cmd += 2 + sizeof(int) * 2;
	}
		break;

	case VMC_ADDI: { StkDualOp<int>([](int a, int b) { return a + b; }); cmd += 2; } break;
	case VMC_SUBI: { StkDualOp<int>([](int a, int b) { return a - b; }); cmd += 2; } break;
	case VMC_MULI: { StkDualOp<int>([](int a, int b) { return a * b; }); cmd += 2; } break;
	case VMC_DIVI: { StkDualOp<int>([](int a, int b) { return a / b; }); cmd += 2; } break;
	case VMC_MODI: { StkDualOp<int>([](int a, int b) { return a % b; }); cmd += 2; } break;
	case VMC_NEGI: { StkSingleOp<int>([](int a) { return -a; }); cmd += 2; } break;
	case VMC_F2I: { StkF2I(); cmd += 2; } break;


	case VMC_ADDF: { StkDualOp<float>([](float a, float b) { return a + b; }); cmd += 2; } break;
	case VMC_SUBF: { StkDualOp<float>([](float a, float b) { return a - b; }); cmd += 2; } break;
	case VMC_MULF: { StkDualOp<float>([](float a, float b) { return a * b; }); cmd += 2; } break;
	case VMC_DIVF: { StkDualOp<float>([](float a, float b) { return a / b; }); cmd += 2; } break;
	case VMC_MODF: { StkDualOp<float>([](float a, float b) { return fmodf(a, b); }); cmd += 2; } break;
	case VMC_NEGF: { StkSingleOp<float>([](float a) { return -a; }); cmd += 2; } break;
	case VMC_I2F: { StkI2F(); cmd += 2; } break;


	case VMC_SETPOS: {
		int p1 = StkPop(), p2 = StkPop();
		number_t x, y;
		x.integer = StkPop();
		y.integer = StkPop();
		cmd += 2;
	}
		break;
	case VMC_MOVEPOS: {
		int time = StkPop(), mode = StkPop(); number_t final_x, final_y;
		final_x = StkPop();
		final_y = StkPop();
		cmd += 2;
	}
		break;
	}
	goto cmd_begin;
cmd_exit:
	vm->a = a; vm->b = b; vm->c = c; vm->d = d;
	vm->frame_pointer = frame_pointer;
	vm->stack_pointer = stack_pointer;
	return;
cmd_error:
	printf("And error has ocurred\n");
	return;
}

struct token_t {
	std::string val;
	KEYWORDS keyword_id;
	TK_TYPE token_type;
	number_t number_val;
	int line;
};

struct token_size_t {
	int value;
	int size;
};

struct subroutine_t {
	int size;
};

std::vector<token_t> tokens;
std::vector<token_size_t> out_tokens;
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
			tokens.push_back({ std::move(str), kwd, kwd & KW_OTHER ? TT_IDENTIFIER : TT_KEYWORD, 0, line });
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
		const token_t& tok = tokens[i];
		switch (tok.token_type) {
		default: i++; break;
		case TT_IDENTIFIER:
			if (commands.find(tok.val) != commands.end()) {
				int cmd_val = commands[tok.val].cmd;
				printf("Found %s (0x%x)\n", tok.val.c_str(), cmd_val);
				out_tokens.push_back({cmd_val, 2});
			}
			i++;
			break;
		case TT_INT: {
			int val = std::stol(tok.val);
			printf("Int value %d\n", val);
			out_tokens.push_back({ val, 4 });
			i++;
		} break;
		case TT_FLOAT: {
			number_t val;
			val.real = std::stod(tok.val);
			printf("Float value %f\n", val.real);
			out_tokens.push_back({ val.integer, 4 });
			i++;
		} break;
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
				if (i + 1 < size && (tokens[i + 1].token_type & TT_IDENTIFIER)) { // Detect an identifier
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

// Command data for testing
char* idata = nullptr;
size_t isize = 0;

void Token2Data() {
	char* data = (char*)calloc(out_tokens.size(), sizeof(int));
	char* offset = data;
	size_t size = 0;
	for (auto& t: out_tokens) {
		switch (t.size) {
		case 2:
			*((int16_t*)offset) = (int16_t)t.value;
			offset += sizeof(int16_t);
			size += sizeof(int16_t);
			break;
		case 4:
			*((int*)offset) = t.value;
			offset += sizeof(int);
			size += sizeof(int);
			break;
		default:
			break;
		}
	}
	idata = (char*)calloc(size + 1, sizeof(char));
	memcpy(idata, data, size);
	free(data);
	isize = size;
}

void PrintTokens() {
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
}

void PrintFunction() {
	printf("There are %d functions:\n", function_names.size());
	for (auto& s : function_names) {
		printf("Subroutine: %s\n", s.first.c_str());
	}
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
				Token2Data();
			}
		}
		free(data);

		vm_t vm;
		InitVM(&vm, idata);
		RunVM(&vm);
	}

}