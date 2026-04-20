#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double f(double x) {
    return x * x;   // f(x) = x^2
}

int main() {
    int n, i;
    double a, b, h, sum = 0.0, result;
    double start, end;

    printf("Enter lower limit (a): ");
    scanf("%lf", &a);

    printf("Enter upper limit (b): ");
    scanf("%lf", &b);

    printf("Enter number of trapezoids (n): ");
    scanf("%d", &n);

    h = (b - a) / n;

    FILE *fp = fopen("trap_output.txt", "w");

    start = omp_get_wtime();

    #pragma omp parallel for reduction(+:sum)
    for (i = 1; i < n; i++) {
        double x = a + i * h;
        int tid = omp_get_thread_num();

        double val = f(x);
        sum += val;

        #pragma omp critical
        {
            fprintf(fp, "i=%d x=%lf f(x)=%lf (Thread %d)\n", i, x, val, tid);
        }
    }

    result = h * ( (f(a) + f(b)) / 2.0 + sum );

    end = omp_get_wtime();

    fclose(fp);

    // Show first 10 steps
    printf("\nFirst 10 intermediate values:\n");
    for (i = 1; i <= n && i <= 10; i++) {
        double x = a + i * h;
        printf("i=%d x=%lf f(x)=%lf\n", i, x, f(x));
    }

    printf("\nFinal Integral Result = %lf\n", result);
    printf("Execution Time = %lf seconds\n", end - start);

    return 0;
}