#include "morse_table.h"
#include <stdlib.h>
#include <string.h>

size_t max_code_length(struct morse_entry **table) {
  size_t max_morse_length = 0;
  for (int i = 0; i < MORSE_TABLE_LENGTH; i++) {
    if (table[i]) {
      char *symbol = table[i]->code;
      size_t codeLength = strlen(symbol);
      max_morse_length =
          codeLength > max_morse_length ? codeLength : max_morse_length;
    }
  }
  return max_morse_length;
}

struct morse_entry *init_entry(char *seq) {
  struct morse_entry *entry = malloc(sizeof(struct morse_entry));
  entry->code = malloc(strlen(seq) * sizeof(char));
  strcpy(entry->code, seq);
  entry->score = 1;
  return entry;
}

void uninit_entry(struct morse_entry *entry) {
  free(entry->code);
  free(entry);
}

// An associative array for morse symbols, horrendously done
// Indexing into morse can be done with the character of interest
// The size of the array is of course idiotically oversized
struct morse_entry **init_morse_table() {
  struct morse_entry **table;
  table = calloc(MORSE_TABLE_LENGTH, sizeof(struct morse_entry *));

  table['!'] = init_entry("-.-.--");
  table['"'] = init_entry(".-..-.");
  table['&'] = init_entry(".-...");
  table['\''] = init_entry(".----.");
  table['('] = init_entry("-.--.");
  table[')'] = init_entry("-.--.-");
  table['+'] = init_entry(".-.-.");
  table[','] = init_entry("--..--");
  table['-'] = init_entry("-....-");
  table['.'] = init_entry(".-.-.-");
  table['/'] = init_entry("-..-.");
  table['0'] = init_entry("-----");
  table['1'] = init_entry(".----");
  table['2'] = init_entry("..---");
  table['3'] = init_entry("...--");
  table['4'] = init_entry("....-");
  table['5'] = init_entry(".....");
  table['6'] = init_entry("-....");
  table['7'] = init_entry("--...");
  table['8'] = init_entry("---..");
  table['9'] = init_entry("----.");
  table[':'] = init_entry("---...");
  table['='] = init_entry("-...-");
  table['?'] = init_entry("..--..");
  table['@'] = init_entry(".--.-.");
  table['A'] = init_entry(".-");
  table['B'] = init_entry("-...");
  table['C'] = init_entry("-.-.");
  table['D'] = init_entry("-..");
  table['E'] = init_entry(".");
  table['F'] = init_entry("..-.");
  table['G'] = init_entry("--.");
  table['H'] = init_entry("....");
  table['I'] = init_entry("..");
  table['J'] = init_entry(".---");
  table['K'] = init_entry("-.-");
  table['L'] = init_entry(".-..");
  table['M'] = init_entry("--");
  table['N'] = init_entry("-.");
  table['O'] = init_entry("---");
  table['P'] = init_entry(".--.");
  table['Q'] = init_entry("--.-");
  table['R'] = init_entry(".-.");
  table['S'] = init_entry("...");
  table['T'] = init_entry("-");
  table['U'] = init_entry("..-");
  table['V'] = init_entry("...-");
  table['W'] = init_entry(".--");
  table['X'] = init_entry("-..-");
  table['Y'] = init_entry("-.--");
  table['Z'] = init_entry("--..");

  return table;
}

void uninit_morse_table(struct morse_entry **table) {
  // Free structs first
  for (int i = 0; i < MORSE_TABLE_LENGTH; i++) {
    if (table[i]) {
      uninit_entry(table[i]);
    }
  }

  free(table);
}
