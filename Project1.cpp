#include <iostream>
#include <thread>
#include <chrono>

int main() {
    const size_t N = 1000000;
    const int THREADS = 4;

    int* data = new int[N];

    unsigned x = 123456789u;
    for (size_t i = 0; i < N; ++i) {
        x = x * 1103515245u + 12345u;
        data[i] = int(x % 100u) + 1;
    }

    auto start1 = std::chrono::high_resolution_clock::now();
    long long sum1 = 0;
    for (size_t i = 0; i < N; ++i) sum1 += data[i];
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t1 = end1 - start1;

    std::cout << "Sum (single thread): " << sum1 << std::endl;
    std::cout << "Time (1 thread): " << t1.count() << " s\n\n";

    auto start2 = std::chrono::high_resolution_clock::now();

    std::thread threads[THREADS];
    long long partial[THREADS] = {0};
    size_t chunk = N / THREADS;

    for (int i = 0; i < THREADS; ++i) {
        size_t begin = static_cast<size_t>(i) * chunk;
        size_t end = (i == THREADS - 1) ? N : begin + chunk;
        threads[i] = std::thread([&](int idx, size_t b, size_t e) {
            long long s = 0;
            for (size_t j = b; j < e; ++j) s += data[j];
            partial[idx] = s;
        }, i, begin, end);
    }

    for (int i = 0; i < THREADS; ++i)
        threads[i].join();

    long long sum2 = 0;
    for (int i = 0; i < THREADS; ++i)
        sum2 += partial[i];

    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t2 = end2 - start2;

    std::cout << "Sum (4 threads): " << sum2 << std::endl;
    std::cout << "Time (4 threads): " << t2.count() << " s\n\n";
    std::cout << "Speedup: " << t1.count() / t2.count() << "x\n";

    delete[] data;
    return 0;
}
