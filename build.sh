rm main 2> /dev/null # Ignore error message
gcc -g -o main main.c ./ui/*.c ./morse/*.c ./miniaudio/*.c -Wall -Wextra -Werror -Wpedantic -lpthread -lm -lncursesw
