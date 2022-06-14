CC=gcc
CFLAGSERROR= -Wall -std=c11 -pedantic -Wextra -Werror
CFLAGS=-std=c11
CS_FILES= scanner_tests.c scanner.c dynamic_string.c
DS_FILES= dynamic_string_tests.c dynamic_string.c
ST_FILES= symtable.c dynamic_string.c
ALL_FILES= code_generator.c dstr_queue.c dynamic_string.c expression.c param_stack.c scanner.c stack.c symtable.c syntax_analysis.c
.PHONY: scanner_tests scanner_run scanner_all dstr_tests dstr_run dstr_all all clean symtable_tests project

project:$(ALL_FILES)
	$(CC) $^ $(CFLAGS) -o $@

project_error:
	$(CC) $^ $(CFLAGSERROR) -o $@

symtable_tests:$(ST_FILES)
	$(CC) $^ $(CFLAGS) -o $@


scanner_tests: $(CS_FILES)
	$(CC) $^ $(CFLAGS) -o $@

scanner_run: scanner_tests
	@ ./scanner_tests < ./code_examples/example1.tl > output.out
	@ diff -s output.out ./code_examples/outputs/example1.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/example2.tl > output.out
	@ diff -s output.out ./code_examples/outputs/example2.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/example3.tl > output.out
	@ diff -s output.out ./code_examples/outputs/example3.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/fun.tl > output.out
	@ diff -s output.out ./code_examples/outputs/fun.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/hello.tl > output.out
	@ diff -s output.out ./code_examples/outputs/hello.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/multiassign.tl > output.out
	@ diff -s output.out ./code_examples/outputs/multiassign.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/substr.tl > output.out
	@ diff -s output.out ./code_examples/outputs/substr.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/visibility.tl > output.out
	@ diff -s output.out ./code_examples/outputs/visibility.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/whitespaces.tl > output.out
	@ diff -s output.out ./code_examples/outputs/whitespaces.ref
	@ rm output.out

	@ ./scanner_tests < ./code_examples/everything.tl > output.out
	@ diff -s output.out ./code_examples/outputs/everything.ref
	@ rm output.out

	@ awk 'NR == 1' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1;
	@ awk 'NR == 2' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 3' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 4' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 5' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 6' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 7' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 8' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 9' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 10' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 11' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 12' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 13' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 14' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 15' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 16' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 17' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 18' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ awk 'NR == 19' ./code_examples/err.tl | ./scanner_tests >> output.out 2>&1
	@ diff -s output.out ./code_examples/outputs/err.ref
	@ rm output.out

scanner_all: scanner_run clean

dstr_tests: $(DS_FILES)
	$(CC) $^ $(CFLAGS) -o $@

dstr_run: dstr_tests
	@ ./dstr_tests > output.out
	@ diff -s output.out ./code_examples/outputs/dstr.ref
	@ rm output.out

dstr_all: dstr_run clean

all:dstr_all scanner_all clean

clean:
	rm -f *_tests
