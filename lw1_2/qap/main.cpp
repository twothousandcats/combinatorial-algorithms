#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cassert>
#include <numeric>

using namespace std;
using namespace std::chrono;

constexpr int MAX_CALC_COUNT = 11;

constexpr int MAX_ARG_COUNT = 2;
constexpr int EXE_POSITION = 0;
constexpr int INPUT_F_POSITION = 1;

constexpr auto *OPT_PLACEMENT = "Оптимальное назначение: \n";
constexpr auto *WARNING_LEFT = "Программа используется с 1 входным файлом: ";
constexpr auto *WARNING_RIGHT = " <input_file>\n";
constexpr auto *WARNING_FILE = "Не удалось открыть файл: ";

constexpr double AGE_EARTH_SECONDS = 4.54e9 * 365.25 * 24 * 3600; // ~1.43e17 сек
constexpr double AGE_UNIVERSE_SECONDS = 13.8e9 * 365.25 * 24 * 3600; // ~4.35e17 сек

void readMatrix(
    ifstream &input,
    vector<vector<int> > &matrix,
    const int matrixCount
) {
    matrix.assign(matrixCount, vector<int>(matrixCount));
    for (int i = 0; i < matrixCount; ++i) {
        for (int j = 0; j < matrixCount; ++j) {
            input >> matrix[i][j];
        }
    }
}

// Вычисляет целевую функцию R для заданной перестановки p
long long computeCost(
    const vector<vector<int> > &flow,
    const vector<vector<int> > &dist,
    const vector<int> &permutation
) {
    const size_t permutationLength = permutation.size();
    long long total = 0;
    for (int i = 0; i < permutationLength; ++i) {
        for (int j = 0; j < permutationLength; ++j) {
            total += static_cast<long long>(flow[i][j]) * dist[permutation[i]][permutation[j]];
        }
    }
    return total;
}

// Полный перебор
pair<long long, vector<int> > solveQAPBruteForce(
    const vector<vector<int> > &flow,
    const vector<vector<int> > &dist
) {
    const size_t flowLength = flow.size();
    vector<int> permutation(flowLength);
    iota(permutation.begin(), permutation.end(), 0);

    long long bestCost = LLONG_MAX;
    vector<int> bestPerm;
    do {
        if (const long long cost = computeCost(flow, dist, permutation); cost < bestCost) {
            bestCost = cost;
            bestPerm = permutation;
        }
    } while (ranges::next_permutation(permutation).found);

    return {bestCost, bestPerm};
}

/**
 * Выводит перестановку в консоль
 * @param perm
 */
void printAssignment(
    const vector<int> &perm
) {
    cout << OPT_PLACEMENT;
    for (int i = 0; i < perm.size(); ++i) {
        cout << "Предприятие " << i << " -> место " << perm[i] << "\n";
    }
}

/**
 * Генерирует DOT-файл в формате неориентированного графа:
 * graph G {
 *   i -- j [label="d"];
 * }
 * где d = dist[perm[i]][perm[j]], если flow[i][j] > 0
 */
void generateDot(
    const vector<vector<int> > &flow,
    const vector<vector<int> > &dist,
    const vector<int> &premutation,
    const string &filename
) {
    ofstream dot(filename);
    dot << "graph G {\n";

    const size_t premutationLength = premutation.size();
    for (int i = 0; i < premutationLength; ++i) {
        for (int j = i + 1; j < premutationLength; ++j) {
            if (flow[i][j] > 0 || flow[j][i] > 0) {
                int d = dist[premutation[i]][premutation[j]];
                dot << "  " << i << " -- " << j << " [label=\"" << d << "\"];\n";
            }
        }
    }

    dot << "}\n";
}

/**
 * Преобразует секунды
 * @param seconds
 * @return
 */
string formatTime(
    const double seconds
) {
    if (seconds < 1e-6) return "<1 мкс";
    if (seconds < 1e-3) return to_string(seconds * 1e6).substr(0, 5) + " мкс";
    if (seconds < 1.0) return to_string(seconds * 1e3).substr(0, 5) + " мс";
    return to_string(seconds).substr(0, 6) + " с";
}

/**
 * Сравнивает время с возрастом Земли/Вселенной
 * @param seconds
 */
void compareWithCosmicTime(
    const double seconds
) {
    cout << "\nСравнение со временем:\n";
    cout << "Возраст Земли: ~1.43e17 сек\n";
    cout << "Возраст Вселенной: ~4.35e17 сек\n";

    if (seconds > AGE_UNIVERSE_SECONDS) {
        cout << "Алгоритм для этого N занял бы БОЛЬШЕ времени, чем возраст Вселенной!\n";
    } else if (seconds > AGE_EARTH_SECONDS) {
        cout << "Алгоритм для этого N занял бы больше времени, чем возраст Земли.\n";
    } else {
        cout << "Время выполнения меньше возраста Земли.\n";
    }
}

void calcFact(
    const int matrixCount
) {
    cout << "N = " << matrixCount << " слишком велико для полного перебора.\n";
    // Оценка времени по факториалу
    double estimateSeconds = 1e-9; // 1 нс на одну перестановку
    for (int i = 1; i <= matrixCount; ++i) {
        estimateSeconds *= i;
    }
    cout << "Оценка времени: " << formatTime(estimateSeconds) << "\n";
    compareWithCosmicTime(estimateSeconds);
}

int main(
    const int argCount,
    char *argValues[]
) {
    if (argCount != MAX_ARG_COUNT) {
        cerr << WARNING_LEFT << argValues[EXE_POSITION] << WARNING_RIGHT;

        return 1;
    }

    ifstream fin(argValues[INPUT_F_POSITION]);
    if (!fin.is_open()) {
        cerr << WARNING_FILE << argValues[INPUT_F_POSITION] << "\n";
        return 1;
    }

    int matrixCount;
    fin >> matrixCount;

    if (matrixCount > MAX_CALC_COUNT) {
        calcFact(matrixCount);

        return 0;
    }

    vector<vector<int> > flow(matrixCount, vector<int>(matrixCount));
    vector<vector<int> > dist(matrixCount, vector<int>(matrixCount));

    readMatrix(fin, flow, matrixCount);
    readMatrix(fin, dist, matrixCount);
    fin.close();

    cout << "Решение квадратичной задачи о назначениях\n";
    cout << "N = " << matrixCount << "\n";

    const auto start = high_resolution_clock::now();
    auto [bestCost, bestPerm] = solveQAPBruteForce(flow, dist);
    const auto end = high_resolution_clock::now();

    const double duration = duration_cast<std::chrono::duration<double> >(end - start).count();

    cout << "Минимальные затраты R = " << bestCost << "\n"; // "стоимость" х расстояние
    printAssignment(bestPerm);
    cout << "Время выполнения: " << formatTime(duration) << "\n";

    // Сохраняем граф назначений
    const string dotFile = string(argValues[INPUT_F_POSITION]) + ".dot";
    generateDot(flow, dist, bestPerm, dotFile);
    cout << "Граф назначений сохранён в: " << dotFile << "\n";

    return 0;
}
