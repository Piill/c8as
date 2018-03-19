#ifndef _ASSEMBLER
#define _ASSEMBLER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include "linked_list.h"
#define NAME_MAX 254

typedef enum { label, instruction, argument} token_type;

typedef char token[NAME_MAX];

typedef struct _line {
	char original_line[NAME_MAX*4];
	int num;
	token tokens[5];
} line;

void init_keywords();

int load_file(line** lineList, const char* filepath);
void write_binary(uint16_t* bytes, int num);
void write_ascii(uint16_t* bytes, int num);
line parse_line(const char* scr);


int parse_file(const char* filepath, uint16_t** bytes);
int parse_labels(line* lineList, int num);
uint16_t* parse_inst(line* lineList, int num);

uint16_t get_addr(char* s);
uint16_t get_nibble(char* s);
uint16_t get_reg(char* s);
uint16_t get_byte(char* s);

#endif //_ASSEMBLER
