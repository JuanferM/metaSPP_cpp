#include "heuristics.hpp"

std::tuple<int*, int, int*> GreedyConstruction(
        std::ostream* io[],
        int m,
        int n,
        const int* C,
        const int* A,
        const double* U) {
    bool valid = true;
    int i(0), j(0), s(0), *x = new int[n], *column = new int[m];
    for(i = 0; i < n; i++) x[i] = 0;

    // Indices of utilities in utilities decreasing order
    size_t *u_order = argsort(n, U); // DON'T FORGET TO DELETE
    // We set the variable with the greatest utility to 1
    x[u_order[0]] = 1;
    // Selecting that variable means that we must select the
    // corresponding column in the matrix A and check if the
    // constraints are still verified
    for(j = 0; j < m; j++) {
        column[j] = A[INDEX(u_order[0], j)];
        s += column[j];
    }

    // Repeat the same process with each utility until constraints
    // are eventually violated
    i = 1;
    while(s != m && i < n) {
        for(j = 0, valid = true; j < m && valid; j++)
            valid = column[j] + A[INDEX(u_order[i], j)] <= 1;
        for(j = 0, s = 0; j < m && valid; s += column[j], j++)
            column[j] += A[INDEX(u_order[i], j)];
        x[u_order[i++]] = valid;
    }

    delete[] u_order;
    return std::make_tuple(x, dot(n, x, C), column);
}

std::tuple<int*, int> GreedyImprovement(
        std::ostream* io[],
        int m,
        int n,
        const int* C,
        const int* A,
        const int* x,
        int zInit,
        bool deep,
        int* column) {
    int i(2), t(1), nz(zInit), *nx = new int[n];
    char names[3][4] = { "0-1", "1-1", "2-1" };
    bool printed[3] = { false },
        (*f[3])(int, int, const int*, const int*, int*, int*, bool, int*) = {
            zero_oneExchange,
            one_oneExchange,
            two_oneExchange
        };

    // nx is a deep copy of x at first. With each k-p exchange we search
    // the neighborhood of nx. If a neighbor with a better value for z is
    // found then we replace nx by that neighbor and we keep applying the
    // same k-p exchange on nx. If the k-p exchange fail to find a neighbor that
    // improves z then we select another k-p exchange and we repeat the process.
    memcpy(nx, x, sizeof(int) * n);
    while(i >= 0 && ((t = f[i](m, n, C, A, nx, &nz, deep, column)) || !t)) {
        if(!printed[i])
            m_print(io, "\n > ", names[i], " : "), printed[i] = true;
        if(t) m_print(io, "x"); else i--;
    }

    return std::make_tuple(nx, nz);
}
