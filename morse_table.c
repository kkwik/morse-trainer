#include "morse_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Assigned in the process of init'ing
static int table_entry_count = 0;
static size_t table_max_code_length = 0;
static struct morse_entry **morse_table = NULL;

struct morse_entry *init_entry(char *seq) {
  // Compute and set maximum code length
  size_t seq_len = strlen(seq);
  if (seq_len > table_max_code_length) {
    table_max_code_length = seq_len;
  }

  struct morse_entry *entry = malloc(sizeof(struct morse_entry));
  entry->code = malloc(strlen(seq) * sizeof(char));
  strcpy((char *)entry->code, seq);
  entry->score = DEFAULT_SCORE;
  return entry;
}

void uninit_entry(struct morse_entry *entry) {
  free((char *)(entry->code));
  free(entry);
}

// An associative array for morse symbols, horrendously done
// Indexing into morse can be done with the character of interest
// The size of the array is of course idiotically oversized
struct morse_entry **init_morse_table() {
  if (morse_table != NULL) {
    return morse_table;
  }

  morse_table = calloc(MORSE_TABLE_BUFFER_SIZE, sizeof(struct morse_entry *));

  morse_table['!'] = init_entry("-.-.--");
  morse_table['"'] = init_entry(".-..-.");
  morse_table['&'] = init_entry(".-...");
  morse_table['\''] = init_entry(".----.");
  morse_table['('] = init_entry("-.--.");
  morse_table[')'] = init_entry("-.--.-");
  morse_table['+'] = init_entry(".-.-.");
  morse_table[','] = init_entry("--..--");
  morse_table['-'] = init_entry("-....-");
  morse_table['.'] = init_entry(".-.-.-");
  morse_table['/'] = init_entry("-..-.");
  morse_table['0'] = init_entry("-----");
  morse_table['1'] = init_entry(".----");
  morse_table['2'] = init_entry("..---");
  morse_table['3'] = init_entry("...--");
  morse_table['4'] = init_entry("....-");
  morse_table['5'] = init_entry(".....");
  morse_table['6'] = init_entry("-....");
  morse_table['7'] = init_entry("--...");
  morse_table['8'] = init_entry("---..");
  morse_table['9'] = init_entry("----.");
  morse_table[':'] = init_entry("---...");
  morse_table['='] = init_entry("-...-");
  morse_table['?'] = init_entry("..--..");
  morse_table['@'] = init_entry(".--.-.");
  morse_table['A'] = init_entry(".-");
  morse_table['B'] = init_entry("-...");
  morse_table['C'] = init_entry("-.-.");
  morse_table['D'] = init_entry("-..");
  morse_table['E'] = init_entry(".");
  morse_table['F'] = init_entry("..-.");
  morse_table['G'] = init_entry("--.");
  morse_table['H'] = init_entry("....");
  morse_table['I'] = init_entry("..");
  morse_table['J'] = init_entry(".---");
  morse_table['K'] = init_entry("-.-");
  morse_table['L'] = init_entry(".-..");
  morse_table['M'] = init_entry("--");
  morse_table['N'] = init_entry("-.");
  morse_table['O'] = init_entry("---");
  morse_table['P'] = init_entry(".--.");
  morse_table['Q'] = init_entry("--.-");
  morse_table['R'] = init_entry(".-.");
  morse_table['S'] = init_entry("...");
  morse_table['T'] = init_entry("-");
  morse_table['U'] = init_entry("..-");
  morse_table['V'] = init_entry("...-");
  morse_table['W'] = init_entry(".--");
  morse_table['X'] = init_entry("-..-");
  morse_table['Y'] = init_entry("-.--");
  morse_table['Z'] = init_entry("--..");

  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (morse_table[i]) {
      table_entry_count++;
    }
  }
  return morse_table;
}

void uninit_morse_table() {
	// Free structs first
	for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
		if (morse_table[i]) {
			uninit_entry(morse_table[i]);
		}
	}

	free(morse_table);
}

int get_table_entry_count() { return table_entry_count; }

size_t get_table_max_code_length() { return table_max_code_length; }

