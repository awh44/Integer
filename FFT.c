#include <stdio.h>
#include <math.h>
#include <stdlib.h>

const long double two_pi = 2 * M_PI;

typedef struct
{
	long double real;
	long double imag;
} Complex;

Complex complex_add(Complex a, Complex b)
{
	Complex result;
	result.real = a.real + b.real;
	result.imag = a.imag + b.imag;
	return result;
}

Complex complex_multiply(Complex a, Complex b)
{
	Complex result;
	result.real = a.real * b.real - a.imag * b.imag;
	result.imag = a.real * b.imag + a.imag * b.real;
	return result;
}

Complex *FFT(int n, int *x, int start, int step_size)
{
	Complex *result = malloc(n * sizeof *result);
	if (n == 1)
	{
		result[0].real = x[start];
		return;
	}

	int n_over_2 = n / 2;
	int ss_times_2 = step_size * 2;
	Complex *u = FFT(n_over_2, x, start, ss_times_2),
			*v = FFT(n_over_2, x, start + step_size, ss_times_2);

	long double mult_val = two_pi / n;
	int j;
	for (j = 0; j < n; j++)
	{
		Complex tau;
		long double mult_val_j = mult_val * j;
		tau.real = cosl(mult_val_j);
		tau.imag = sinl(mult_val_j);
		int j_mod_n_over_2 = j % n_over_2;
		result[j] = complex_add(u[j_mod_n_over_2], complex_multiply(tau, v[j_mod_n_over_2]));
	}

	free(u);
	free(v);
	return result;
}

int main()
{
	int inp[] = {0, 6, -5, 1};
	Complex *result = FFT(4, inp, 0, 1);

	int i;
	for (i = 0; i < 4; i++)
	{
		printf("%Lf + %Lfi\n", result[i].real, result[i].imag);
	}

	free(result);
	return 0;
}
