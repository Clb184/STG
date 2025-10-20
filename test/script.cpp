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
	TT_COLON = 1 << 10,
};

enum KEYWORDS {
	KW_OTHER = 1 << 1,
	KW_PROCEDURE = 1 << 2,
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
	VMC_LALLOC, // Allocate space for 
	VMC_ENTER, // Prepare local space for call
	VMC_LEAVE, // Delete arguments for calls

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

	// Game itself commands
	VMC_SETPOS,
	VMC_MOVEPOS,
	VMC_MOVEDIR,

	// Debug commands
	VMC_PRINTSTACK,
	VMC_PRINTSTACKSIZE,
	VMC_PRINTLOCALS,
	VMC_PRINTLOCALSSIZE,
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
	{"nop",      {VMC_NOP, 0}},
	{"nop2",     {VMC_NOP2, 1}},

	{"call",      {VMC_CALL, 1}},
	{"return",    {VMC_RETURN, 0}},
	{"kill",      {VMC_KILL, 0}},

	{"lalloc",     {VMC_LALLOC, 1}},
	{"enter",		{VMC_ENTER, 1}},
	{"leave",		{VMC_LEAVE, 0}},

	{"pushc",     {VMC_PUSHC, 1}},

	{"pushr",     {VMC_PUSHR, 1}},
	{"pushl",     {VMC_PUSHL, 1}},
	{"pushg",     {VMC_PUSHG, 1}},

	{"popr",      {VMC_POPR, 1}},
	{"popl",      {VMC_POPL, 1}},
	{"popg",      {VMC_POPG, 1}},

	{"setr",      {VMC_SETR, 2}},
	{"setl",      {VMC_SETL, 2}},
	{"setg",      {VMC_SETG, 2}},

	{"addi",      {VMC_ADDI, 0}},
	{"subi",      {VMC_SUBI, 0}},
	{"muli",      {VMC_MULI, 0}},
	{"divi",      {VMC_DIVI, 0}},
	{"modi",      {VMC_MODI, 0}},
	{"negi",      {VMC_NEGI, 0}},
	{"f2i",       {VMC_F2I, 0}},

	{"addf",      {VMC_ADDF, 0}},
	{"subf",      {VMC_SUBF, 0}},
	{"mulf",      {VMC_MULF, 0}},
	{"divf",      {VMC_DIVF, 0}},
	{"modf",      {VMC_MODF, 0}},
	{"negf",      {VMC_NEGF, 0}},
	{"i2f",       {VMC_I2F, 0}},

	{"setpos",    {VMC_SETPOS, 2}},
	{"movepos",   {VMC_MOVEPOS, 4}},
	{"movedir",   {VMC_MOVEDIR, 3}},

	{"printstack",			{VMC_PRINTSTACK, 0}},
	{"printstacksize",		{VMC_PRINTSTACKSIZE, 0}},
	{"printlocals",			{VMC_PRINTLOCALS, 0}},
	{"printlocalssize",		{VMC_PRINTLOCALSSIZE, 0}},
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
	bool on_level = calc_stack.stack_pointer < max_calcstack;
	if (on_level) {
		calc_stack.stack[calc_stack.stack_pointer].integer = val;
		calc_stack.stack_pointer++;
	}
	else {
		printf("Stack Overflow\n");
		assert(on_level);
	}
}

int StkPop() {
	bool on_level = calc_stack.stack_pointer > 0;
	if (on_level) {
		calc_stack.stack_pointer--;
		return calc_stack.stack[calc_stack.stack_pointer].integer;
	}
	else {
		printf("Stack Underflow\n");
		assert(on_level);
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
		calc_stack.stack_pointer--;
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

void InitVM(vm_t* vm, char* cmd_data, void* reg_data, void* glob_data) {
	assert(nullptr != vm);

	memset(vm, 0, sizeof(vm_t));
	vm->command_data = cmd_data;
	vm->register_data = (int*)reg_data;
	vm->global_reg_data = (int*)glob_data;
}

void RunVM(vm_t* vm) {
	assert(nullptr != vm);

	// Registers
	number_t a = vm->a, b = vm->b, c = vm->c, d = vm->d;
	number_t* stack = vm->locals;
	int* register_data = vm->register_data;
	int* global_data = vm->global_reg_data;
	int frame_pointer = vm->frame_pointer;
	int stack_pointer = vm->stack_pointer;

	// Commands
	char* cmd = vm->command_data;
	assert(nullptr != cmd);
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
	case VMC_CALL:
		if (vm->return_pointer < 8) {
			vm->call_stack[vm->return_pointer++] = cmd + 2 + sizeof(int);
			cmd = vm->command_data + *((int*)(cmd + 2));
		}
		else {
			// A CallStack overflow has happened
			goto cmd_error;
		}
		break;
	case VMC_RETURN:
		if (vm->return_pointer > 0) {
			cmd = vm->call_stack[vm->return_pointer - 1];
			vm->return_pointer--;
		}
		else {
			// In this case, just halt the VM and act as kill 
			goto cmd_exit;
		}
		break;
	case VMC_KILL:
		goto cmd_exit;
		break;

	case VMC_LALLOC:
		stack_pointer += *((int*)(cmd + 2));
		cmd += 2 + sizeof(int);
		break;

	case VMC_ENTER: {
		stack[stack_pointer++] = frame_pointer;
		frame_pointer = stack_pointer;
		int regs = *((int*)(cmd + 2));
		stack_pointer += regs;
		for (int i = regs - 1; i >= 0; i--) {
			stack[frame_pointer + i] = StkPop();
		}
		cmd += 2 + sizeof(int);
	}
		break;
	case VMC_LEAVE: 
		stack_pointer = frame_pointer;
		frame_pointer = stack[frame_pointer - 1];
		cmd += 2;
		break;

	// Push operations
	case VMC_PUSHC:
		StkPush(*((int*)(cmd + 2)));
		cmd += 2 + sizeof(int);
		break;
	case VMC_PUSHL:
		StkPush(stack[frame_pointer + *((int*)(cmd + 2))].integer);
		cmd += 2 + sizeof(int);
		break;
	case VMC_PUSHR:
		assert(nullptr != register_data);
		StkPush(register_data[*((int*)(cmd + 2))]);
		cmd += 2 + sizeof(int);
		break;
	case VMC_PUSHG:
		assert(nullptr != global_data);
		StkPush(global_data[*((int*)(cmd + 2))]);
		cmd += 2 + sizeof(int);
		break;

	case VMC_STACKCLEAR:
		cmd += 2;
		break;

	// Set functions
	case VMC_SETL: {
		int reg = *((int*)(cmd + 2));
		stack[frame_pointer + reg].integer = *((int*)(cmd + 2 + sizeof(int)));
		cmd += 2 + sizeof(int) * 2;
	}
	break;
	case VMC_SETR: {
		assert(nullptr != register_data);
		int reg = *((int*)(cmd + 2));
		register_data[reg] = *((int*)(cmd + 2 + sizeof(int)));
		cmd += 2 + sizeof(int) * 2;
	}
	break;
	case VMC_SETG: {
		assert(nullptr != global_data);
		int reg = *((int*)(cmd + 2));
		global_data[reg] = *((int*)(cmd + 2 + sizeof(int)));
		cmd += 2 + sizeof(int) * 2;
	}
				 break;

	// Pop functions
	case VMC_POPL: {
		int reg = *((int*)(cmd + 2));
		stack[frame_pointer + reg].integer = StkPop();
		cmd += 2 + sizeof(int);
	}
	break;
	case VMC_POPR: {
		assert(nullptr != register_data);
		int reg = *((int*)(cmd + 2));
		register_data[reg] = StkPop();
		cmd += 2 + sizeof(int);
	}
	break;
	case VMC_POPG: {
		assert(nullptr != global_data);
		int reg = *((int*)(cmd + 2));
		global_data[reg] = StkPop();
		cmd += 2 + sizeof(int);
	}
				 break;

	// Arithmetic functions (int)
	case VMC_ADDI: { StkDualOp<int>([](int a, int b) { return a + b; }); cmd += 2; } break;
	case VMC_SUBI: { StkDualOp<int>([](int a, int b) { return a - b; }); cmd += 2; } break;
	case VMC_MULI: { StkDualOp<int>([](int a, int b) { return a * b; }); cmd += 2; } break;
	case VMC_DIVI: { StkDualOp<int>([](int a, int b) { return a / b; }); cmd += 2; } break;
	case VMC_MODI: { StkDualOp<int>([](int a, int b) { return a % b; }); cmd += 2; } break;
	case VMC_NEGI: { StkSingleOp<int>([](int a) { return -a; }); cmd += 2; } break;
	case VMC_F2I: { StkF2I(); cmd += 2; } break;

	// Arithmetic functions (float)
	case VMC_ADDF: { StkDualOp<float>([](float a, float b) { return a + b; }); cmd += 2; } break;
	case VMC_SUBF: { StkDualOp<float>([](float a, float b) { return a - b; }); cmd += 2; } break;
	case VMC_MULF: { StkDualOp<float>([](float a, float b) { return a * b; }); cmd += 2; } break;
	case VMC_DIVF: { StkDualOp<float>([](float a, float b) { return a / b; }); cmd += 2; } break;
	case VMC_MODF: { StkDualOp<float>([](float a, float b) { return fmodf(a, b); }); cmd += 2; } break;
	case VMC_NEGF: { StkSingleOp<float>([](float a) { return -a; }); cmd += 2; } break;
	case VMC_I2F: { StkI2F(); cmd += 2; } break;


	case VMC_SETPOS: {
		number_t x, y;
		y = StkPop();
		x = StkPop();
		cmd += 2;
	}
		break;
	case VMC_MOVEPOS: {
		number_t time, mode, final_x, final_y;
		final_y = StkPop();
		final_x = StkPop();
		mode = StkPop();
		time = StkPop();
		cmd += 2;
	}
		break;

	case VMC_PRINTSTACK:
		printf("-------- Stack --------\n");
		for (int i = 0; i < calc_stack.stack_pointer; i++) {
			number_t num = calc_stack.stack[i];
			printf("%###d i: %##########d | f: %##########.3f | x: 0x%########x\n", i, num.integer, num.real, num.integer);
		}
		cmd += 2;
		break;
	case VMC_PRINTSTACKSIZE:
		printf("Stack size: %d\n", calc_stack.stack_pointer);
		cmd += 2;
		break;
	case VMC_PRINTLOCALS:
		printf("-------- Locals --------\n");
		for (int i = frame_pointer; i < stack_pointer; i++) {
			number_t num = stack[i];
			printf("%###d i: %##########d | f: %##########.3f | x: 0x%########x\n", i, num.integer, num.real, num.integer);
		}
		cmd += 2;
		break;
	case VMC_PRINTLOCALSSIZE:
		printf("Number of locals: %d\n", stack_pointer - frame_pointer);
		cmd += 2;
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

enum OTOKEN_TYPE {
	OTT_COMMAND,
	OTT_NUMBER,
	OTT_IDENTIFIER,
};

struct token_size_t {
	int value;
	OTOKEN_TYPE type;
	int extra_info;
};


struct subroutine_data_t {
	size_t size;
	size_t offset_begin;
	int tok_begin;
	int tok_end;
	int num_args;
	bool defined;
};

// How does this work?
// Just adds the function name + its value and references to it
// Labels are also symbols, however, they internally are 'label'@'function',
// since you can't use @ in normal syntax
enum SYMBOL_TYPE{
	ST_FUNCTION,
	ST_LOCAL,
	ST_LABEL,
	ST_CONSTANT,
};

struct symbol_mention_t {
	int value;
	SYMBOL_TYPE type;
	int type_desc;
	std::vector<int> references;
};


std::map<const std::string, symbol_mention_t> symbol_map;

std::vector<token_t> tokens;
std::vector<token_size_t> out_tokens;
std::map<std::string, subroutine_data_t> functions;
const char* name = "test.scpt";

constexpr KEYWORDS GetType(const std::string& str) {
	if (str == "proc") return KW_PROCEDURE;
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

struct func_call_t {
	std::string name;
	std::vector<token_t> values;
};

bool _cdecl TranformBlockData(size_t* idx, size_t* codesz, size_t* offset_file, const std::string& function_name, int* argc) {
	assert(nullptr != idx);
	assert(nullptr != codesz);
	assert(nullptr != argc);

	size_t size = tokens.size();
	size_t i = *idx;
	size_t code_size = 0;
	size_t offset_f = *offset_file;
	int next_type = TT_KEYWORD | TT_IDENTIFIER;
	int next_keyword = KW_FLOAT | KW_INT;
	int func_call_depth = 0;
	int args = *argc;
	bool next_global = false;
	for (; i < size; ) {
		const token_t& tok = tokens[i];
		switch (tok.token_type) {
		default: i++; break;
		case TT_KEYWORD: {
			if (next_keyword & tok.keyword_id) {
				switch (tok.keyword_id) {
				
				// Declaring integer and float
				case KW_FLOAT:
				case KW_INT:
					if (i + 1 < size && tokens[i + 1].token_type == TT_IDENTIFIER) {
						std::string local_name = tokens[i + 1].val + "@" + function_name;
						symbol_map[local_name].type = ST_LOCAL;
						symbol_map[local_name].type_desc = tokens[i + 1].keyword_id;
						symbol_map[local_name].value = args;
						args++;
						i += 2;
						while (i + 1 < size && tokens[i].token_type == TT_COMMA && tokens[i + 1].token_type == TT_IDENTIFIER) { // In case you want to do -> int a, b
							std::string local_name = tokens[i + 1].val + "@" + function_name;
							symbol_map[local_name].type = ST_LOCAL;
							symbol_map[local_name].type_desc = tokens[i + 1].keyword_id;
							symbol_map[local_name].value = args;
							args++;
							i += 2;
						}

						next_type = TT_KEYWORD | TT_IDENTIFIER; 
						next_keyword = KW_FLOAT | KW_INT;
					}
					break;
				}
			}
			else {
				ERROR_EXIT("This keyword was not expected");
			}

		}
			break;
		case TT_IDENTIFIER: {
			if (i + 1 < size && tokens[i + 1].token_type == TT_PARENTHESIS_OPEN) { // Is a call
				bool is_stack_call = false;
				bool is_proc = false;
				bool is_stack_command = false;
				int num_args = 0;
				int cmd_val = 0;
				if (commands.find(tok.val) != commands.end()) { // Your usual command
					cmd_val = commands[tok.val].cmd;
					if (cmd_val >= VMC_SETPOS && cmd_val < VMC_PRINTSTACK) {
						is_stack_call = true;
						is_stack_command = true;
					}
					printf("Found %s (0x%x)\n", tok.val.c_str(), cmd_val);
					if (!is_stack_command) {
						out_tokens.push_back({ cmd_val, OTT_COMMAND, 0 });
						offset_f += 2;
						code_size++;
					}

				}
				else if (functions.find(tok.val) != functions.end()) { // Is a procedure call
					printf("Found procedure call %s\n", tok.val.c_str());
					is_stack_call = true;
					is_proc = true;
				}
				else {
					char buf[512] = "";
					sprintf(buf, "Can't find symbol \"%s\"\n", tok.val.c_str());
					ERROR_EXIT(buf);
				}
				i+= 2;
				while (i < size) {
					TK_TYPE tt = tokens[i].token_type;
					if(tt == TT_PARENTHESIS_CLOSE) goto on_end_func_call;
					switch (tt) {
					case TT_INT: {
						int val = std::stol(tokens[i].val);
						printf("Int value %d\n", val);
						if (is_stack_call) {
							out_tokens.push_back({ VMC_PUSHC, OTT_COMMAND, 0 });
							code_size++;
							offset_f += 2;
						}
						if (is_proc) {
							num_args++;
						}

						out_tokens.push_back({ val, OTT_NUMBER, 0 });
						i++;
						code_size++;
						offset_f += 4;
					}
							   break;
					case TT_FLOAT: {
						number_t val;
						val.real = (float)std::stod(tokens[i].val);
						printf("Float value %f\n", val.real);
						if (is_stack_call) {
							out_tokens.push_back({ VMC_PUSHC, OTT_COMMAND, 0 });
							code_size++;
							offset_f += 2;
						}
						if (is_proc) {
							num_args++;
						}

						out_tokens.push_back({ val.integer, OTT_NUMBER, 0 });
						i++;
						code_size++;
						offset_f += 4;
					}
								 break;
					case TT_IDENTIFIER:
					{
						std::string identifier = tokens[i].val + "@" + function_name;
						bool found_identifier = true;
						if (symbol_map.find(identifier) == symbol_map.end()) { // Look at global scope
							identifier = tokens[i].val;
							if (symbol_map.find(identifier) == symbol_map.end()) { // Else at local
								char buf[512] = "";
								sprintf(buf, "Can't find symbol \"%s\"\n", identifier.c_str());
								ERROR_EXIT(buf);
							}
						}
						printf("Found identifier %s\n", identifier.c_str());
						if (is_stack_call) {
							switch (symbol_map[identifier].type) {
							case ST_LOCAL:
								out_tokens.push_back({ VMC_PUSHL, OTT_COMMAND, 0 });
								code_size++;
								offset_f += 2;
								break;
							case ST_CONSTANT:
								out_tokens.push_back({ VMC_PUSHC, OTT_COMMAND, 0 });
								code_size++;
								offset_f += 2;
								break;
							default:
								ERROR_EXIT("Can't put procedure or label names as parameters on procedure call\n");
							}
						}
						if (is_proc) {
							num_args++;
						}
						symbol_map[identifier].references.push_back(offset_f);
						out_tokens.push_back({ 0, OTT_IDENTIFIER, 0 });
						code_size++;
						offset_f += 4;
						i++;
					}
					break;
					default:
						ERROR_EXIT("Expected int, float or identifier\n");
					}
					if (i < size) {
						if (tokens[i].token_type == TT_COMMA) i++;
					}
				}
			on_end_func_call:
				if (is_proc) {
					out_tokens.push_back({ VMC_ENTER, OTT_COMMAND, 0 });
					code_size++;
					offset_f += 2;
					out_tokens.push_back({ num_args, OTT_NUMBER, 0 });
					code_size++;
					offset_f += 4;
					out_tokens.push_back({ VMC_CALL, OTT_COMMAND, 0 });
					code_size++;
					offset_f += 2;
					symbol_map[tok.val].references.push_back(offset_f);
					out_tokens.push_back({ 0, OTT_IDENTIFIER, 0 });
					code_size++;
					offset_f += 4;
					out_tokens.push_back({ VMC_LEAVE, OTT_COMMAND, 0 });
					code_size++;
					offset_f += 2;
				} else if (is_stack_command) {
					out_tokens.push_back({ cmd_val, OTT_COMMAND, 0 });
					offset_f += 2;
					code_size++;
				}

				i++;
			}
			else {
				ERROR_EXIT("Missing parenthesis (Assign is not implemented)\n");

			}
			break;
		}
		case TT_BRACKET_CLOSE:
			*idx = i; // Tokenized index
			*codesz += code_size; // Position in tokens
			*offset_file = offset_f; // Real position in file
			*argc = args;
			return true;
		}
	}
	
}

size_t out_file_size = 0;

bool Transform() {
	size_t size = tokens.size();
	bool on_function = false;
	bool on_parameter_declare = false;
	std::string func_name = "";
	subroutine_data_t subr = { 0 };
	int next_token = TT_KEYWORD;
	int next_keyword = KW_PROCEDURE;
	size_t code_size = 0;
	size_t tok_pos = 0;
	size_t offset_begin = 0;
	int enter_size = 0;
	int num_args = 0;
	bool know_args = false;
	int expected_args = 0;
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
			case KW_PROCEDURE:
				if (on_function) {
					ERROR_EXIT("Can't declare a function inside another\n");
				}
				if (i + 1 < size && (tokens[i + 1].token_type & TT_IDENTIFIER)) { // Detect an identifier
					on_function = true;
					func_name = tokens[i + 1].val;
					// Check if is not repeating function name
					if (functions.find(func_name) != functions.end()) {
						if (functions[func_name].defined) {
							ERROR_EXIT("Can't redefine a function");
						}
						else {
							know_args = true;
							expected_args = functions[func_name].num_args;
						}
					};
					// Then add reference
					symbol_map[func_name].type = ST_FUNCTION;
					symbol_map[func_name].value = out_file_size;
					i += 2;
					next_token = TT_PARENTHESIS_OPEN | TT_BRACKET_OPEN | TT_KEYWORD;
					next_keyword = KW_PROCEDURE;
				}
				break;
			case KW_INT:
			case KW_FLOAT:
				if (on_parameter_declare) {
					if (i + 1 < size && (tokens[i + 1].token_type & TT_IDENTIFIER)) {
						// Then add reference
						const std::string& val = tokens[i + 1].val + "@" + func_name;
						symbol_map[val].type = ST_LOCAL;
						symbol_map[val].value = enter_size++;
						symbol_map[val].type_desc = (tokens[i].keyword_id == KW_INT) ? KW_INT : KW_FLOAT;
						num_args++;
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
			subr.offset_begin = out_file_size;
			subr.tok_begin = tok_pos;
			subr.num_args = num_args;
			// Add if there are any variables declared
			out_file_size += 2 + 4;
			if (false == TranformBlockData(&i, &subr.size, &out_file_size, func_name, &enter_size)) return false;
			out_tokens.insert(out_tokens.begin() + subr.tok_begin, { enter_size - num_args, OTT_NUMBER, 0 });
			out_tokens.insert(out_tokens.begin() + subr.tok_begin, { VMC_LALLOC, OTT_COMMAND, 0 });
			tok_pos += subr.size + 1 + 2;
			out_tokens.push_back({VMC_RETURN, OTT_COMMAND, 0});
			out_file_size += 2;

			subr.tok_end = tok_pos;
			while (out_file_size % 4) {
				out_file_size++; // Pad with zeros for allignment
			}
			
			next_token = TT_BRACKET_CLOSE;

			break;
		case TT_BRACKET_CLOSE:
			next_token = TT_KEYWORD;
			next_keyword = KW_PROCEDURE;
			on_function = false;
			enter_size = 0;
			i++;
			subr.defined = true;
			functions[func_name] = std::move(subr);
			func_name = "";
			memset(&subr, 0, sizeof(subroutine_data_t));
			subr.defined = false;
			break;
		case TT_PARENTHESIS_OPEN:
			next_token = TT_KEYWORD | TT_PARENTHESIS_CLOSE;
			next_keyword = KW_INT | KW_FLOAT;
			on_parameter_declare = true;
			i++;
			break;
		case TT_PARENTHESIS_CLOSE:
			next_token = TT_BRACKET_OPEN | TT_KEYWORD;
			next_keyword = KW_PROCEDURE;
			on_parameter_declare = false;
			if (i + i < size && tokens[i + 1].token_type == TT_KEYWORD && tokens[i + 1].keyword_id == KW_PROCEDURE) {
				subr.num_args = num_args;
				functions[func_name] = std::move(subr);
				func_name = "";
				on_function = false;
				num_args = 0;
				enter_size = 0;
			}

			if (know_args && !(num_args == expected_args)) {
				char buf[512] = "";
				sprintf(buf, "The amount of arguments for \"%s\" is not the same as declaration (given %d, expected %d)", func_name.c_str(), num_args, expected_args);
				ERROR_EXIT(buf);
			}
			i++;
			break;
		case TT_COMMA:
			next_token = TT_KEYWORD;
			next_keyword = KW_FLOAT | KW_INT;
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
	char* data = (char*)calloc(out_file_size + 2, sizeof(int));
	char* offset = data;
	data[out_file_size] = 0xff;
	data[out_file_size + 1] = 0xff;
	for (auto& t: functions) {
		printf("%s:\n", t.first.c_str());
		size_t begin = t.second.tok_begin;
		size_t end = t.second.tok_end;
		offset = data + t.second.offset_begin;
		for (int i = begin; i < end; i++) {
			switch (out_tokens[i].type) {
			case OTT_COMMAND:
				*((int16_t*)offset) = (int16_t)out_tokens[i].value;
				printf("\n\t%d  ", *(int16_t*)offset);
				offset += sizeof(int16_t);
				break;
			case OTT_IDENTIFIER:
				*((int*)offset) = 0;
				printf("id, ");
				offset += sizeof(int);
				break;
			case OTT_NUMBER:
				*((int*)offset) = out_tokens[i].value;
				printf("%d, ", *(int*)offset);
				offset += sizeof(int);
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
	idata = data;
}

void PutSymbols() {
	for (auto& s : symbol_map) {
		int value = s.second.value;
		for (auto& o : s.second.references) {
			*(int*)(idata + o) = value;
		}
	}
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
		case TT_COLON: printf("COLON"); break;
		}
		printf("\n");
	}
	printf("\n");
}

void PrintFunction() {
	printf("There are %d functions:\n", functions.size());
	for (auto& s : functions) {
		printf("Subroutine: %s\n", s.first.c_str());
	}
}

struct test_t {
	int a;
	float b;
} tst;

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
				PutSymbols();
			}
		}
		free(data);

		vm_t vm;
		InitVM(&vm, idata, &tst, nullptr);
		RunVM(&vm);
	}

}