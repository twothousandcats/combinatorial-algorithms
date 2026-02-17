#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <string>

using namespace std;

int main() {
    int N;
    int minVal, maxVal;

    cout << "Введите размерность матрицы N: ";
    cin >> N;

    if (N <= 0) {
        cerr << "Ошибка: N должно быть положительным целым числом.\n";
        return 1;
    }

    cout << "Введите минимальное значение для элементов матриц: ";
    cin >> minVal;
    cout << "Введите максимальное значение для элементов матриц: ";
    cin >> maxVal;

    if (minVal > maxVal) {
        cerr << "Ошибка: минимальное значение не может быть больше максимального.\n";
        return 1;
    }

    // Инициализация генератора
    unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);
    uniform_int_distribution<int> valueDist(minVal, maxVal);

    // Создание матриц
    vector<vector<int>> flow(N, vector<int>(N, 0));
    vector<vector<int>> dist(N, vector<int>(N, 0));

    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            int f = valueDist(rng);
            int d = valueDist(rng);
            flow[i][j] = flow[j][i] = f;
            dist[i][j] = dist[j][i] = d;
        }
        // Диагональ остаётся 0
    }

    // Запись в файл
    const string outFile = "input.in";
    ofstream out(outFile);
    if (!out.is_open()) {
        cerr << "Не удалось создать файл '" << outFile << "'.\n";
        return 1;
    }

    out << N << "\n";

    // Матрица потоков
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            out << flow[i][j];
            if (j + 1 < N) out << " ";
        }
        out << "\n";
    }

    // Матрица расстояний
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            out << dist[i][j];
            if (j + 1 < N) out << " ";
        }
        out << "\n";
    }

    out.close();
    cout << "\nФайл '" << outFile << "' успешно создан для N = " << N << ".\n";
    cout << "Диапазон значений: [" << minVal << ", " << maxVal << "]\n";

    return 0;
}