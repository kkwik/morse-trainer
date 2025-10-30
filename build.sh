rm main 2> /dev/null # Ignore error message
gcc -g -o main *.c -Wall -Wextra -Werror -Wpedantic -lpthread -lm -lncurses
