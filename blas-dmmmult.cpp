/* inspired by 
 * https://stackoverflow.com/questions/23324480
 */

#include <cblas.h>
#include <iostream> // HPC can into CPP!
#include <random>
#include <chrono>

void mat_mult(double* A, double* B, double* C, int Ax, int Ay, int Bx) {
    for(int x = 0; x < Bx; x++) {
        for(int y = 0; y < Ay; y++) {
            double res = 0;
            for(int i = 0; i < Ax; i++) {
                res += A[y * Ax + i] * B[i * Bx + x];
            }
            C[y * Bx + x] = res;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "invocation: " <<argv[0]<<" matrix_size " << std::endl;
        return 1;
    }
    const long long n{std::stoi(std::string{argv[1]})};
    std::mt19937_64 rnd;
    std::uniform_real_distribution<double> doubleDist{0, 1};
    
    double* A = new double[n*n];
    double* B = new double[n*n];
    double* C = new double[n*n];
    double* Cown = new double[n*n];
    // double* A = (double*) malloc(sizeof(double)*n*n);
    // double* B = (double*) malloc(sizeof(double)*n*n);
    // double* C = (double*) malloc(sizeof(double)*n*n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i*n + j] = doubleDist(rnd);
            B[i*n + j] = doubleDist(rnd);
            C[i*n + j] = 0;
            Cown[i*n + j] = 0;
        }
    }

    auto startTime = std::chrono::steady_clock::now();
    // this segfaults for matrix size 5000 and more
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                n, n, n,
                1.0, A, n, B, n, 0.0, C, n);
    auto finishTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed{finishTime - startTime};
    std::cout << "CBLAS elapsed time: "<< elapsed.count() << "[s]" << std::endl;

    startTime = std::chrono::steady_clock::now();
    // this segfaults for matrix size 5000 and more
    mat_mult(A, B, Cown, n, n, n);
    finishTime = std::chrono::steady_clock::now();
    elapsed = finishTime - startTime;
    std::cout << "Own elapsed time: "<< elapsed.count() << "[s]" << std::endl;
}

