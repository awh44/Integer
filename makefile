PAGER=less
CC=gcc

.PHONY: view-results FFT clean

view-results: results.test
	@$(PAGER) results.test

results.test: main.out
	@./main.out > results.test

main.out: main.c Integer.c Integer.h
	@$(CC) -omain.out main.c Integer.c

FFT: FFT.out
	@./FFT.out $(FIRST) $(SECOND)

FFT.out: FFT.c
	@gcc -oFFT.out FFT.c -lm

clean:
	@rm -r *.out
