#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

Complex compute_single_complex_mod_n(Complex a, int n)
{
	Complex result = { ((uint64_t) a.real) % n, ((uint64_t) a.imag) % n };
	return result;
}

Complex *compute_complex_mod_n(int n, Complex *x)
{
	Complex *result = malloc(n * sizeof *result);
	int i;
	for (i = 0; i < n; i++)
	{
		result[i] = compute_single_complex_mod_n(x[i], n);
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
	Complex *fft_result = FFT_worker(n, conjugates, 0, 1);
	Complex *ret_val = compute_conjugates_over_n(n, fft_result);
	
	free(conjugates);
	free(fft_result);
	
	return ret_val;
}

void print_complex_number(Complex x)
{
	printf("%.20Lf + %.20Lfi\n", x.real, x.imag);
}

void print_complex_numbers(int n, Complex *x)
{
	int i;
	for (i = 0; i < n; i++)
	{
		print_complex_number(x[i]);
	}
}

int8_t get_bit_i(uint32_t a, int bit)
{
	return (a >> bit) & 1;
}

int64_t multiply(uint32_t a, uint32_t b)
{
	Complex *a_bits = calloc(32, sizeof *a_bits);
	Complex *b_bits = calloc(32, sizeof *b_bits);
	
	int i;
	for (i = 0; i < 32; i++)
	{
		a_bits[i].real = get_bit_i(a, i);
		b_bits[i].real = get_bit_i(b, i);
	}

	Complex *a_fft = FFT(32, a_bits);
	Complex *b_fft = FFT(32, b_bits);
	Complex *mult_results = calloc(32, sizeof *mult_results);

	for (i = 0; i < 32; i++)
	{
		mult_results[i] = complex_multiply(a_fft[i], b_fft[i]);
	}

	Complex *result_fft = inverse_FFT(32, mult_results);
	print_complex_numbers(32, result_fft);

	uint64_t result = 0,
			 and_val = 1;
	i = 0;
	while (i < 32)//and_val != 0)
	{/*
		if (result_fft[i].real >= 1.0L)
		{
			printf("%d: or'ing. result_fft[i].real = %Lf\n", i, result_fft[i].real);
			result |= and_val;
		}
		*/
		//and_val <<= 1;
		//printf("i = %d: %lu\n", i, (uint64_t) result_fft[i].real);
		result |= ((uint64_t) (result_fft[i].real + 0.5)) << i;
		i++;
	}

	free(a_bits);
	free(b_bits);
	free(mult_results);
	free(result_fft);
	
	return result;
}

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		printf("Please enter two numbers to multiply.\n");
		return 1;
	}

	int64_t value = multiply(atoi(argv[1]), atoi(argv[2]));
	printf("%ld\n", value);

	return 0;

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
