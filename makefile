.PHONY: test FFT clean

test: main.out
	@./main.out

FFT: FFT.out
	@./FFT.out $(FIRST) $(SECOND)

main.out: main.c Integer.c Integer.h
	@gcc -omain.out main.c Integer.c

FFT.out: FFT.c
	@gcc -oFFT.out FFT.c -lm

clean:
	@rm -r *.out
