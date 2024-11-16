#include <iostream>
#include <pthread.h>
#include <gmp.h>
#include <vector>

struct ThreadData {
    int start;
    int end;
    mpz_t partial_product;
};

void* compute_partial_factorial(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    mpz_init_set_ui(data->partial_product, 1);

    for(int i = data->start; i <= data->end; ++i){
        mpz_mul_ui(data->partial_product, data->partial_product, i);
    }

    pthread_exit(nullptr);
}

int main() {
    const int n = 1000000;
    const int num_threads = 4;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    int numbers_per_thread = n / num_threads;
    int remainder = n % num_threads;
    int current = 1;

    for(int i = 0; i < num_threads; ++i){
        thread_data[i].start = current;
        thread_data[i].end = current + numbers_per_thread - 1;
        if(i == num_threads -1){
            thread_data[i].end += remainder;
        }
        current = thread_data[i].end + 1;

        int rc = pthread_create(&threads[i], nullptr, compute_partial_factorial, static_cast<void*>(&thread_data[i]));
        if(rc){
            std::cerr << "Erro ao criar thread, código de retorno: " << rc << std::endl;
            return 1;
        }
    }

    for(int i = 0; i < num_threads; ++i){
        pthread_join(threads[i], nullptr);
    }

    mpz_t factorial;
    mpz_init_set_ui(factorial, 1);

    for(int i = 0; i < num_threads; ++i){
        mpz_mul(factorial, factorial, thread_data[i].partial_product);
        mpz_clear(thread_data[i].partial_product);
    }

    size_t num_digits = mpz_sizeinbase(factorial, 10);
    std::cout << "Número de dígitos de " << n << "! é " << num_digits << std::endl;

    char* factorial_str = mpz_get_str(nullptr, 10, factorial);

    std::cout << "Primeiros 100 dígitos de " << n << "!:\n";
    for(int i = 0; i < 100 && i < num_digits; ++i){
        std::cout << factorial_str[i];
    }
    std::cout << std::endl;

    std::cout << "Últimos 100 dígitos de " << n << "!:\n";
    for(int i = num_digits - 100; i < num_digits; ++i){
        std::cout << factorial_str[i];
    }
    std::cout << std::endl;

    mpz_clear(factorial);
    free(factorial_str);

    return 0;
}
