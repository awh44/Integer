PAGER=less
CC=gcc

.PHONY: view-results FFT clean debug memcheck

view-results: results.test
	@$(PAGER) results.test

results.test: main.out
	@./main.out > results.test

main.out: main.c Integer.c Integer.h
	$(CC) -g -omain.out main.c Integer.c

debug: main.out
	@gdb ./main.out

memcheck: main.out
	@valgrind ./main.out

FFT: FFT.out
	@./FFT.out $(FIRST) $(SECOND)

FFT.out: FFT.c
	@gcc -oFFT.out FFT.c -lm

clean:
	@rm -r *.out
