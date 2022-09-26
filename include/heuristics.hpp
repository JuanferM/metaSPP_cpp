#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "movements.hpp"

#include <string>
#include <cstring>

// Greedy construction of a feasible solution
std::tuple<int*, int, int*> GreedyConstruction(
        std::ostream* io[],
        int m,
        int n,
        const int* C,
        const int* A,
        const double* U);

// Greedy improvement of a feasible solution through (deep) local search
std::tuple<int*, int> GreedyImprovement(
        std::ostream* io[],
        int m,
        int n,
        const int* C,
        const int* A,
        const int* x,
        int zInit,
        bool deep = true,
        int* column = nullptr);

#endif /* end of include guard: HEURISTICS_H */
