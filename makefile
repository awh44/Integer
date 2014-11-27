.PHONY: view-results FFT clean

view-results: results.test
	less results.test

results.test: main.out
	@echo running
	@./main.out > results.test

main.out: main.c Integer.c Integer.h
	@gcc -omain.out main.c Integer.c

FFT: FFT.out
	@./FFT.out $(FIRST) $(SECOND)

FFT.out: FFT.c
	@gcc -oFFT.out FFT.c -lm

clean:
	@rm -r *.out
