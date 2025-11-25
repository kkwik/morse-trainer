#include "morse_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct morse_entry {
  const char *sequence;
  int score;
};

static struct morse_table *table = NULL;
static struct morse_entry **entries = NULL;
static int entry_count = 0;
static size_t max_sequence_length = 0;
static int total_score = 0;

struct morse_entry *init_entry(char *seq) {
  struct morse_entry *entry = malloc(sizeof(struct morse_entry));
  entry->sequence = malloc(strlen(seq) * sizeof(char));
  strcpy((char *)entry->sequence, seq);
  entry->score = DEFAULT_SCORE;
  return entry;
}

void uninit_entry(struct morse_entry *entry) {
  free((char *)(entry->sequence));
  free(entry);
}

int get_entry_count() { return entry_count; }
size_t get_max_sequence_length() { return max_sequence_length; }
int get_total_score() { return total_score; }
bool contains(int sym) {
  return sym > 0 && sym < MORSE_TABLE_BUFFER_SIZE && entries[sym] != NULL;
}
int get_score(int sym) {
  if (contains(sym)) {
    return entries[sym]->score;
  }
  return -1; // TODO: This is not a great failure indicator
}
void inc_score(int sym) {
  if (contains(sym)) {
    if (entries[sym]->score < MAX_SCORE) {
      entries[sym]->score++;
      total_score++;
    }
  }
}
void dec_score(int sym) {
  if (contains(sym)) {
    if (entries[sym]->score > 0) {
      entries[sym]->score--;
      total_score--;
    }
  }
}
const char *get_sequence(int sym) {
  if (contains(sym)) {
    return entries[sym]->sequence;
  }
  return NULL;
}

struct morse_table *init_morse_table() {
  if (table != NULL) {
    return table;
  }

  // An associative array for morse symbols, horrendously done
  // Indexing into morse can be done with the character of interest
  // The size of the array is of course idiotically oversized
  entries = calloc(MORSE_TABLE_BUFFER_SIZE, sizeof(struct morse_entry *));
  entries['!'] = init_entry("-.-.--");
  entries['"'] = init_entry(".-..-.");
  entries['&'] = init_entry(".-...");
  entries['\''] = init_entry(".----.");
  entries['('] = init_entry("-.--.");
  entries[')'] = init_entry("-.--.-");
  entries['+'] = init_entry(".-.-.");
  entries[','] = init_entry("--..--");
  entries['-'] = init_entry("-....-");
  entries['.'] = init_entry(".-.-.-");
  entries['/'] = init_entry("-..-.");
  entries['0'] = init_entry("-----");
  entries['1'] = init_entry(".----");
  entries['2'] = init_entry("..---");
  entries['3'] = init_entry("...--");
  entries['4'] = init_entry("....-");
  entries['5'] = init_entry(".....");
  entries['6'] = init_entry("-....");
  entries['7'] = init_entry("--...");
  entries['8'] = init_entry("---..");
  entries['9'] = init_entry("----.");
  entries[':'] = init_entry("---...");
  entries['='] = init_entry("-...-");
  entries['?'] = init_entry("..--..");
  entries['@'] = init_entry(".--.-.");
  entries['A'] = init_entry(".-");
  entries['B'] = init_entry("-...");
  entries['C'] = init_entry("-.-.");
  entries['D'] = init_entry("-..");
  entries['E'] = init_entry(".");
  entries['F'] = init_entry("..-.");
  entries['G'] = init_entry("--.");
  entries['H'] = init_entry("....");
  entries['I'] = init_entry("..");
  entries['J'] = init_entry(".---");
  entries['K'] = init_entry("-.-");
  entries['L'] = init_entry(".-..");
  entries['M'] = init_entry("--");
  entries['N'] = init_entry("-.");
  entries['O'] = init_entry("---");
  entries['P'] = init_entry(".--.");
  entries['Q'] = init_entry("--.-");
  entries['R'] = init_entry(".-.");
  entries['S'] = init_entry("...");
  entries['T'] = init_entry("-");
  entries['U'] = init_entry("..-");
  entries['V'] = init_entry("...-");
  entries['W'] = init_entry(".--");
  entries['X'] = init_entry("-..-");
  entries['Y'] = init_entry("-.--");
  entries['Z'] = init_entry("--..");

  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (entries[i]) {
      entry_count++;

      // Yes we could just do entry_count * DEFAULT_SCORE
      // but for testing setting manual values was useful
      // and that approach doesn't work in that scenario
      total_score += entries[i]->score;

      size_t entry_length = strlen(entries[i]->sequence);
      if (entry_length > max_sequence_length) {
        max_sequence_length = entry_length;
      }
    }
  }

  table = malloc(sizeof(struct morse_table));
  if (table == NULL) {
    return NULL;
  }
  table->get_entry_count = get_entry_count;
  table->get_max_sequence_length = get_max_sequence_length;
  table->get_total_score = get_total_score;
  table->contains = contains;
  table->get_score = get_score;
  table->inc_score = inc_score;
  table->dec_score = dec_score;
  table->get_sequence = get_sequence;

  return table;
}

void uninit_morse_table() {
  // Free structs first
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (entries[i]) {
      uninit_entry(entries[i]);
    }
  }

  free(entries);
  free(table);
}
