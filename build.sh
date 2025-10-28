
rm main 2> /dev/null # Ignore error message
gcc -o main main.c -Wall -Wextra -Werror -Wpedantic -lpthread -lm -lncurses
