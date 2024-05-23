binary := tests
flags := -Wall -Wextra -std=c11 -pedantic -O3

test:
	clang $(flags) -o $(binary) tests.c -g && ./$(binary)
