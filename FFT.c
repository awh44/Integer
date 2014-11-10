#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

const long double TWO_PI = 2 * M_PI;

typedef struct
{
	long double real;
	long double imag;
} Complex;

Complex complex_add(Complex a, Complex b)
{
	Complex result = { a.real + b.real, a.imag + b.imag };
	return result;
}

Complex complex_multiply(Complex a, Complex b)
{
	Complex result = { a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real };
	return result;
}

Complex complex_conjugate_over_n(Complex a, int n)
{
	Complex result = { a.real / n, -a.imag / n };
	return result;
}

Complex complex_conjugate(Complex a)
{
	Complex result = { a.real, -a.imag };
	return result;
}

Complex *compute_conjugates(int n, Complex *x)
{
	Complex *result = malloc(n * sizeof *result);
	int i;
	for (i = 0; i < n; i++)
	{
		result[i] = complex_conjugate(x[i]);
	}

	return result;
}

Complex *compute_conjugates_over_n(int n, Complex *x)
{
	Complex *result = malloc(n * sizeof *result);
	int i;
	for (i = 0; i < n; i++)
	{
		result[i] = complex_conjugate_over_n(x[i], n);
	}

	return result;
}

Complex *FFT_worker(int n, Complex *x, int start, int step_size)
{
	Complex *result = malloc(n * sizeof *result);
	if (n == 1)
	{
		result[0] = x[start];
		return;
	}

	int n_over_2 = n / 2;
	int ss_times_2 = step_size * 2;
	Complex *u = FFT_worker(n_over_2, x, start, ss_times_2);
	Complex *v = FFT_worker(n_over_2, x, start + step_size, ss_times_2);

	long double mult_val = TWO_PI / n;
	int j;
	for (j = 0; j < n; j++)
	{
		long double mult_val_j = mult_val * j;
		Complex tau = { cosl(mult_val_j), sinl(mult_val_j) };
		int j_mod_n_over_2 = j % n_over_2;
		result[j] = complex_add(u[j_mod_n_over_2], complex_multiply(tau, v[j_mod_n_over_2]));
	}

	free(u);
	free(v);
	return result;
}

Complex *FFT(int n, Complex *x)
{
	return FFT_worker(n, x, 0, 1);
}

Complex *inverse_FFT(int n, Complex *x)
{
	Complex *conjugates = compute_conjugates(n, x);
	return compute_conjugates_over_n(n, FFT_worker(n, conjugates, 0, 1));
}

void print_complex_number(Complex x)
{
	printf("%Lf + %Lfi\n", x.real, x.imag);
}

void print_complex_numbers(int n, Complex *x)
{
	int i;
	for (i = 0; i < n; i++)
	{
		print_complex_number(x[i]);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Please enter at least one number of which to compute the FFT.\n");
		return 1;
	}
	
	int num_things = argc - 1;
	Complex *inp = malloc(num_things * sizeof *inp);
	int i;
	for (i = 0; i < num_things; i++)
	{
		inp[i].real = atoi(argv[i + 1]);
		inp[i].imag = 0;
	}

	Complex *resultFFT = FFT(num_things, inp);
	print_complex_numbers(num_things, resultFFT);

	printf("\n\n");

	Complex *resultInverseFFT = inverse_FFT(num_things, resultFFT);
	print_complex_numbers(num_things, resultInverseFFT);

	free(resultFFT);
	free(resultInverseFFT);
	return 0;
}
