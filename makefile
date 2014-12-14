PAGER=less
CC=gcc

.PHONY: view-results FFT clean debug memcheck timing

view-results: results/results.test
	@$(PAGER) results/results.test

results/results.test: build/main.out
	@build/main.out > results/results.test

build/main.out: src/main.c src/Integer.c src/Integer.h
	$(CC) -g -obuild/main.out src/main.c src/Integer.c

debug: build/main.out
	@gdb build/main.out

memcheck: build/main.out
	@valgrind build/main.out


timing: build/timing.out
	@build/timing.out $(ARG)

build/timing.out: src/timing.c src/Integer.c src/Integer.h
	@$(CC) -obuild/timing.out src/timing.c src/Integer.c -lgmp

FFT: build/FFT.out
	@build/FFT.out $(FIRST) $(SECOND)

build/FFT.out: src/FFT.c
	@gcc -obuild/FFT.out src/FFT.c -lm

clean:
	@-rm -rf build/*
