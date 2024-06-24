#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <mutex>

const int MAX_THREADS = 10; // Maksymalna liczba wątków

std::mutex result_mutex; // Mutex do synchronizacji dostępu do wyniku
int threads_finished = 0; // Liczba wątków, które zakończyły obliczenia
double result = 0.0; // Wynik obliczeń

// Funkcja obliczająca fragment sumy g_n dla danego zakresu
void calculate_partial_sum(int start, int end, int n) {
    double partial_sum = 0.0;
    for (int k = start; k <= end; ++k) {
        partial_sum += 1.0 / k;
    }
    partial_sum -= log(n);

    // Dodanie częściowej sumy do wyniku (zabezpieczone mutexem)
    {
        std::lock_guard<std::mutex> lock(result_mutex);
        result += partial_sum;
        threads_finished++;
    }
}

// Funkcja klienta, tworząca i uruchamiająca wątki obliczeniowe
void client(int n, int p) {
    // Obliczanie ilości elementów na każdy wątek
    int elements_per_thread = n / p;

    // Tworzenie wektorów przechowujących wątki
    std::vector<std::thread> threads;

    // Rozpoczęcie obliczeń przez każdy wątek
    for (int i = 0; i < p; ++i) {
        int start = i * elements_per_thread + 1;
        int end = (i == p - 1) ? n : (i + 1) * elements_per_thread;

        // Uruchomienie nowego wątku
        threads.emplace_back(calculate_partial_sum, start, end, n);
    }

    // Oczekiwanie na zakończenie obliczeń przez wszystkie wątki
    for (auto& t : threads) {
        t.join();
    }
}
int main() {
    int n, p; // Liczba elementów sumy i liczba procesów

    do {
        std::cout << "Podaj liczbe elementow sumy (n): ";
        std::cin >> n;
        if (n == 0000) break; // Jeśli n = 0000, zakończ program
        std::cout << "Podaj liczbe procesow (p): ";
        std::cin >> p;

        // Uruchomienie klienta
        client(n, p);

        // Wyświetlenie wyniku
        std::cout << "Wynik sumy g_n: " << result << std::endl;

        // Resetowanie wyniku
        result = 0.0;
        threads_finished = 0;
    } while (true);

    return 0;
}