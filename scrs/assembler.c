#include "assembler.h"

ll* keyWords;
ll* labels;

char *input_filename = NULL;
char *output_binary = NULL;
char *output_ascii = NULL;
_Bool verbose = false;

int main(int argc, char** argv) {
	int c;
	while ((c = getopt (argc, argv, "i:b:t:v")) != -1) {
		switch (c)
		{
			case 'i':
				input_filename = optarg;
				break;
			case 'b':
				output_binary = optarg;
				break;
			case 't':
				output_ascii = optarg;
				break;
			case 'v':
				verbose = true;
				break;
			case '?':
				if (optopt == 'i' || optopt == 'b' || optopt == 't') {
					printf("Option -%c requires an argument.\n", optopt);
				} else {
					printf("Unknown option character %c", optopt);
				}
				return -1;
				break;
		}
	}

	init_keywords();
	uint16_t* bytes;
	int num = parse_file(input_filename, &bytes);

	if(output_binary != NULL) {
		write_binary(bytes, num);
	}

	if(output_ascii != NULL) {
		write_ascii(bytes, num);
	}

	free(bytes);
	destroy_list(labels);
	destroy_list(keyWords);
	return 0;
}


void write_binary(uint16_t* bytes, int num) {
	FILE* fp = fopen(output_binary, "wb");
	if(fp == NULL) {
		printf("Error opening file %s.\n", output_binary);
		exit(-1);
	}

	size_t bytes_written = fwrite(bytes, num, sizeof(uint16_t), fp);
	if(bytes_written != num*sizeof(uint16_t)) {
		printf("Number of bytes written to file %s not correct\n", output_binary);
	}

	fclose(fp);
}

void write_ascii(uint16_t* bytes, int num) {
	FILE* fp = fopen(output_ascii, "w");
	if(fp == NULL) {
		printf("Error opening file %s.\n", output_ascii);
		exit(-1);
	}

	for(int i = 0; i < num; i++) {
		fprintf(fp, "0x%04x\n", bytes[i]);
	}

	fclose(fp);
}

int parse_file(const char* filepath, uint16_t** bytes) {
	line* lineList = NULL;
	int num = load_file(&lineList, filepath);
	num = parse_labels(lineList, num);
	*bytes = parse_inst(lineList, num);
	free(lineList);
	destroy_list(labels);

	return num;
}

void init_keywords() {
	keyWords = create_list("CLS", 0);
	keyWords = append_to_list(keyWords, "RET", 0);
	keyWords = append_to_list(keyWords, "SYS", 0);
	keyWords = append_to_list(keyWords, "JP", 0);
	keyWords = append_to_list(keyWords, "CALL", 0);
	keyWords = append_to_list(keyWords, "SE", 0);
	keyWords = append_to_list(keyWords, "SNE", 0);
	keyWords = append_to_list(keyWords, "LD", 0);
	keyWords = append_to_list(keyWords, "ADD", 0);
	keyWords = append_to_list(keyWords, "OR", 0);
	keyWords = append_to_list(keyWords, "AND", 0);
	keyWords = append_to_list(keyWords, "XOR", 0);
	keyWords = append_to_list(keyWords, "SUB", 0);
	keyWords = append_to_list(keyWords, "SHR", 0);
	keyWords = append_to_list(keyWords, "SUBN", 0);
	keyWords = append_to_list(keyWords, "SHL", 0);
	keyWords = append_to_list(keyWords, "RND", 0);
	keyWords = append_to_list(keyWords, "DRW", 0);
	keyWords = append_to_list(keyWords, "SKP", 0);
	keyWords = append_to_list(keyWords, "SKNP", 0);
}

int load_file(line** lineList, const char* filepath) {
	FILE* fp = fopen(filepath, "r");
	if(fp == NULL) {
		printf("File %s could not be opend\n", filepath);
		return -1;
	}

	// Get file lenght
	int file_lenght = 0;
	char* tmp = NULL;
	size_t bytes_read;
	for(file_lenght; getline(&tmp, &bytes_read, fp) != -1; file_lenght++);
	free(tmp);
	rewind(fp);

	// Parse file using parse_line
	*lineList = malloc(sizeof(line)*file_lenght);

	for(int i = 0; getline(&tmp, &bytes_read, fp) != -1; i++) {
		int content = 0;
		for(int j = 0; tmp[j] != '\0'; j++) {
			tmp[j] = toupper(tmp[j]);
			if(!isspace(tmp[j])) {
				content++;
			}
		}

		// Remove empty lines
		if(content == 0) {
			i--;
			file_lenght--;
		} else {
			line l = parse_line(tmp);
			if(l.num > 0) {
				(*lineList)[i] = l;
			} else {
				i--;
				file_lenght--;
			}
		}

	}
	free(tmp);
	fclose(fp);
	return file_lenght;
}

line parse_line(const char* scr) {
	line ret;
	char char_line[NAME_MAX*4];
	int j = 0;
	for(int i = 0; scr[i] != '\0' && scr[i] != ';'; i++) {
		if(scr[i] == '\n') {
			char_line[j] = '\0';
			j++;
		} else if(scr[i] != '\t') {
			char_line[j] = scr[i];
			j++;
		}
	}
	strcpy(ret.original_line, char_line);

	char* c_tok = strtok(char_line, " ,");

	int i;
	for(i = 0; c_tok != NULL; i++) {
		strncpy(ret.tokens[i], c_tok, NAME_MAX);
		c_tok = strtok(NULL, " ,");
	}
	ret.num = i;

	return ret;
}

int parse_labels(line* lineList, int num) {
	int start_addr = 0x200;
	int bytes_pr_line = 2;
	int current_addr = start_addr;

	for(int i = 0; i < num; i++) {
		if(!list_contains(keyWords, lineList[i].tokens[0])) {
			labels = append_to_list(labels, lineList[i].tokens[0],
					current_addr);
			for(int n = 0; n < lineList[i].num-1; n++) {
				strncpy(lineList[i].tokens[n], lineList[i].tokens[n+1], NAME_MAX);
			}
			lineList[i].num--;
		}
		if(lineList[i].num > 0) {
			current_addr += bytes_pr_line;
		} else {
			num--;
			for(int j = i; j < num; j++) {
				lineList[j] = lineList[j+1];
			}
		}
	}

	return num;
}

uint16_t* parse_inst(line* lineList, int num) {
	uint16_t* bytes = malloc(sizeof(uint16_t)*2*num);
	for(int i = 0; i < num; i++) {

		char opcode[NAME_MAX];
		strcpy(opcode, lineList[i].tokens[0]);

		bytes[i] = 0;
		if(strcmp(opcode, "CLS") == 0) {
			bytes[i] = 0x00E0;
		} else if(strcmp(opcode, "RET") == 0) {
			bytes[i] = 0x00EE;
		} else if(strcmp(opcode, "SYS") == 0) {
			uint16_t addr = get_addr(lineList[i].tokens[1]);
			bytes[i] = 0x0000 + (0x0FFF & addr);
		} else if(strcmp(opcode, "JP") == 0) {
			if(lineList[i].num == 2) {
				uint16_t addr = get_addr(lineList[i].tokens[1]);
				bytes[i] = 0x1000 + (0x0FFF & addr);
			} else if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[1][1] == '0') {
				uint16_t addr = get_addr(lineList[i].tokens[2]);
				bytes[i] = 0xB000 + addr;
			}
		} else if(strcmp(opcode, "CALL") == 0) {
			uint16_t addr = get_addr(lineList[i].tokens[1]);
			bytes[i] = 0x2000 + (0x0FFF & addr);
		} else if(strcmp(opcode, "SE") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[2][0] != 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint16_t kk = get_byte(lineList[i].tokens[2]);
				bytes[i] = 0x3000 + (Vx << 8) + kk;
			} else if(lineList[i].num == 3) {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x5000 + (Vx << 8) + (Vy << 4);
			}
		} else if(strcmp(opcode, "SNE") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] != 'V' ) {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint16_t kk = get_byte(lineList[i].tokens[2]);
				bytes[i] = 0x4000 + (Vx << 8) + kk;
			} else if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x9000 + (Vx << 8) + (Vy << 4);
			}
		} else if(strcmp(opcode, "LD") == 0) {
			if(lineList[i].num == 3) {
				if(lineList[i].tokens[1][0] == 'I' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0xF055 + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[1]);
					uint8_t Vy = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0x8000 + (Vx << 8) + (Vy << 4);
				} else if(lineList[i].tokens[1][0] == 'I') {
					uint16_t addr = get_addr(lineList[i].tokens[2]);
					bytes[i] = 0xA000 + addr;
				} else if(lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'D') {
					uint8_t Vx = get_reg(lineList[i].tokens[1]);
					bytes[i] = 0xF007 + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'K') {
					uint8_t Vx = get_reg(lineList[i].tokens[1]);
					bytes[i] = 0xF00A + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'D' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0xF015 + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'S' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0xF018 + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'F' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0xF029 + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'B' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0xF033 + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'I') {
					uint8_t Vx = get_reg(lineList[i].tokens[1]);
					bytes[i] = 0xF065 + (Vx << 8);
				} else if(lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] != 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[1]);
					uint16_t kk = get_byte(lineList[i].tokens[2]);
					bytes[i] = 0x6000 + (Vx << 8) + kk;
				}
			}

		} else if(strcmp(opcode, "ADD") == 0) {
			if(lineList[i].num == 3) {
				if(lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] != 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[1]);
					uint16_t kk = get_byte(lineList[i].tokens[2]);
					bytes[i] = 0x7000 + (Vx << 8) + kk;
				} else if(lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[1]);
					uint8_t Vy = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0x8004 + (Vx << 8) + (Vy << 4);
				} else if(lineList[i].tokens[1][0] == 'I' && lineList[i].tokens[2][0] == 'V') {
					uint8_t Vx = get_reg(lineList[i].tokens[2]);
					bytes[i] = 0xF01E + (Vx << 8);
				}

			}

		} else if(strcmp(opcode, "OR") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x8001 + (Vx << 8) + (Vy << 4);
			}

		} else if(strcmp(opcode, "XOR") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x8003 + (Vx << 8) + (Vy << 4);
			}

		} else if(strcmp(opcode, "AND") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x8002 + (Vx << 8) + (Vy << 4);
			}
		} else if(strcmp(opcode, "SUB") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x8005 + (Vx << 8) + (Vy << 4);
			}
		} else if(strcmp(opcode, "SHR") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x8006 + (Vx << 8) + (Vy << 4);
			} else if(lineList[i].num == 2) {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				bytes[i] = 0x8006 + (Vx << 8);
			}
		} else if(strcmp(opcode, "SUBN") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x8007 + (Vx << 8) + (Vy << 4);
			}
		} else if(strcmp(opcode, "SHL") == 0) {
			if(lineList[i].num == 3 && lineList[i].tokens[1][0] == 'V' && lineList[i].tokens[2][0] == 'V') {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				uint8_t Vy = get_reg(lineList[i].tokens[2]);
				bytes[i] = 0x800E + (Vx << 8) + (Vy << 4);
			} else if(lineList[i].num == 2) {
				uint8_t Vx = get_reg(lineList[i].tokens[1]);
				bytes[i] = 0x800E + (Vx << 8);
			}
		} else if(strcmp(opcode, "RND") == 0) {
			uint8_t Vx = get_reg(lineList[i].tokens[1]);
			uint8_t byte = get_byte(lineList[i].tokens[2]);
			bytes[i] = 0xC000 + (Vx << 8) + byte;
		} else if(strcmp(opcode, "DRW") == 0) {
			uint8_t Vx = get_reg(lineList[i].tokens[1]);
			uint8_t Vy = get_reg(lineList[i].tokens[2]);
			uint8_t n = get_nibble(lineList[i].tokens[3]);
			bytes[i] = 0xD000 + (Vx << 8) + (Vy << 4) + n;

		} else if(strcmp(opcode, "SKP") == 0) {
			uint8_t Vx = get_reg(lineList[i].tokens[1]);
			bytes[i] = 0xE09E + (Vx << 8);
		} else if(strcmp(opcode, "SKNP") == 0) {
			uint8_t Vx = get_reg(lineList[i].tokens[1]);
			bytes[i] = 0xE0A1 + (Vx << 8);
		} else if(strcmp(opcode, ".FILL") == 0) {
			bytes[i] = strtol(lineList[i].tokens[1], NULL, 0);
		} else {
			printf("Unimplemented %s, in line:\n", opcode);
			printf("%s \n", lineList[i].original_line);
		}

	}
	return bytes;
}

uint16_t get_addr(char* s) {
	uint16_t number = 0;
	if(list_contains(labels, s)) {
		number = get_data(labels, s);
	} else {
		number = strtol(s, NULL, 0);
	}
	if(number > 0x0FFF) {
		printf("Error: number (%d) too large\n", number);
		exit(-1);
	}
	return number;
}

uint16_t get_nibble(char* s) {
	uint16_t number = strtol(s, NULL, 0);
	if(number > 0x000F) {
		printf("Error: number (%d) too large\n", number);
		exit(-1);
	}

	return number;
}

uint16_t get_reg(char* s) {
	uint16_t number = strtol(&s[1], NULL, 16);
	if(number > 0x000F || s[0] != 'V') {
		printf("Error: number (%d) too large\n", number);
		exit(-1);
	}

	return number;
}

uint16_t get_byte(char* s) {
	uint16_t number = strtol(s, NULL, 0);
	if(number > 0x00FF) {
		printf("Error: number (%d) too large\n", number);
		exit(-1);
	}
	return number;
}
