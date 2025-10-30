#include "morse_trainer.h"
#include "morse_player.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct morse_entry {
  char *code;
  int score;
};

// An associative array for morse symbols, horrendously done
// Indexing into morse can be done with the character of interest
// The size of the array is of course idiotically oversized
struct morse_entry g_morse_lookup[MORSE_TABLE_LENGTH] = {
    ['!'] = {.code = "-.-.--", .score = 1},
    ['"'] = {.code = ".-..-.", .score = 1},
    ['&'] = {.code = ".-...", .score = 1},
    ['\''] = {.code = ".----.", .score = 1},
    ['('] = {.code = "-.--.", .score = 1},
    [')'] = {.code = "-.--.-", .score = 1},
    ['+'] = {.code = ".-.-.", .score = 1},
    [','] = {.code = "--..--", .score = 1},
    ['-'] = {.code = "-....-", .score = 1},
    ['.'] = {.code = ".-.-.-", .score = 1},
    ['/'] = {.code = "-..-.", .score = 1},
    ['0'] = {.code = "-----", .score = 1},
    ['1'] = {.code = ".----", .score = 1},
    ['2'] = {.code = "..---", .score = 1},
    ['3'] = {.code = "...--", .score = 1},
    ['4'] = {.code = "....-", .score = 1},
    ['5'] = {.code = ".....", .score = 1},
    ['6'] = {.code = "-....", .score = 1},
    ['7'] = {.code = "--...", .score = 1},
    ['8'] = {.code = "---..", .score = 1},
    ['9'] = {.code = "----.", .score = 1},
    [':'] = {.code = "---...", .score = 1},
    ['='] = {.code = "-...-", .score = 1},
    ['?'] = {.code = "..--..", .score = 1},
    ['@'] = {.code = ".--.-.", .score = 1},
    ['A'] = {.code = ".-", .score = 1},
    ['B'] = {.code = "-...", .score = 1},
    ['C'] = {.code = "-.-.", .score = 1},
    ['D'] = {.code = "-..", .score = 1},
    ['E'] = {.code = ".", .score = 1},
    ['F'] = {.code = "..-.", .score = 1},
    ['G'] = {.code = "--.", .score = 1},
    ['H'] = {.code = "....", .score = 1},
    ['I'] = {.code = "..", .score = 1},
    ['J'] = {.code = ".---", .score = 1},
    ['K'] = {.code = "-.-", .score = 1},
    ['L'] = {.code = ".-..", .score = 1},
    ['M'] = {.code = "--", .score = 1},
    ['N'] = {.code = "-.", .score = 1},
    ['O'] = {.code = "---", .score = 1},
    ['P'] = {.code = ".--.", .score = 1},
    ['Q'] = {.code = "--.-", .score = 1},
    ['R'] = {.code = ".-.", .score = 1},
    ['S'] = {.code = "...", .score = 1},
    ['T'] = {.code = "-", .score = 1},
    ['U'] = {.code = "..-", .score = 1},
    ['V'] = {.code = "...-", .score = 1},
    ['W'] = {.code = ".--", .score = 1},
    ['X'] = {.code = "-..-", .score = 1},
    ['Y'] = {.code = "-.--", .score = 1},
    ['Z'] = {.code = "--..", .score = 1},
};

size_t max_code_length() {
  size_t max_morse_length = 0;
  for (int i = 0; i < MORSE_TABLE_LENGTH; i++) {
    char *symbol = g_morse_lookup[i].code;
    if (symbol) {
      size_t codeLength = strlen(symbol);
      max_morse_length =
          codeLength > max_morse_length ? codeLength : max_morse_length;
    }
  }
  return max_morse_length;
}
char current_char;

char sanitize_key_input(char in) {
#pragma GCC diagnostic warning "-Wtype-limits"
  if (in < 0 || in >= MORSE_TABLE_LENGTH) {
    return ' '; // This check might be useless, gcc seems to think so
  }

  if (in > 'a' && in < 'z') {
    return in - 32; // Capitalize
  }

  // We'll assume this is safe (it probably isn't)
  if (g_morse_lookup[(int)in].code) {
    return in;
  }

  return 0; // Catch fallthrough
}

void decrement_score(char ch) {
  if (g_morse_lookup[(int)ch].score > 1) {
    g_morse_lookup[(int)current_char].score--;
  }
}

void increment_score(char ch) {
  if (g_morse_lookup[(int)ch].score < 100) {
    g_morse_lookup[(int)current_char].score++;
  }
}

void trainer_start() { player_setup(max_code_length()); }

void trainer_stop() { player_teardown(); }

void trainer_next() {
  // TODO:get random char following score distribution

  current_char = 'F';
}

void trainer_play() {
  struct player_config config = {.amp = 0.2, .hz = 800, .wpm = 25};
  play_morse_char(config, g_morse_lookup[(int)current_char].code);
}

bool trainer_guess(char ch) {
  ch = sanitize_key_input(ch);
  if (ch == 0) {
    return false;
  }

  bool correct;
  if (current_char == ch) {
    decrement_score(ch);
    correct = true;
  } else {
    increment_score(ch);
    increment_score(current_char);
    correct = false;
  }

  current_char = 0;
  return correct;
}
