#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "movements.hpp"

#include <string>
#include <cstring>

// Greedy construction of a feasible solution
std::tuple<char*, int, char*> GreedyConstruction(
    std::ostream* io[],
    int m,
    int n,
    const int* C,
    const char* A,
    const float* U);

// Greedy improvement of a feasible solution through (deep) local search
std::tuple<char*, int> GreedyImprovement(
    std::ostream* io[],
    int m,
    int n,
    const int* C,
    const char* A,
    const char* x,
    int zInit,
    bool deep = true,
    char* column = nullptr);

#endif /* end of include guard: HEURISTICS_H */
