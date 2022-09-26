#include "heuristics.hpp"

#define DEEPSEARCH false

int main() {
    std::string pwd(std::filesystem::current_path());
    std::string path(pwd + "/instances/");
    std::ofstream result_file("results");
    std::ostream* io[2] = {&std::cout, &result_file};
    std::cout.precision(6);

    double *U(nullptr);
    int m(-1), n(-1), zInit(-1), zBest(-1);
    int *C(nullptr), *A(nullptr), *x(nullptr), *xbest(nullptr), *column(nullptr);
    double tt(0.f), t1(0.f), t2(0.f); INIT_TIMER();

    m_print(io, "Etudiants : MERCIER et PICHON\n");
    std::set<std::string> fnames = getfname(path, io);
    for(auto instance : fnames) {
        // Load one numerical instance
        std::tie(m, n, C, A, U) = loadSPP(path + instance);
        m_print(io, "\n\nInstance : ", instance, "\n\n");

        zInit = 0; zBest = 0;

        m_print(io,
                "Construction gloutonne d'une solution admissible\n");
        TIMED(t1, std::tie(x, zInit, column) = GreedyConstruction(io, m, n, C, A, U));
        m_print(io, "  ", t1, " seconds\n");
        isFeasible(m, n, C, A, x, io, column);
        m_print(io, "z(xInit) = ", zInit, "\n\n");

        m_print(io,
                "Amelioration par recherche locale de type ",
                DEEPSEARCH ? "plus profonde " : "",
                "descente");
        TIMED(t2,
            std::tie(xbest, zBest) = GreedyImprovement(io, m, n, C, A,
                x, zInit, DEEPSEARCH, column));
        m_print(io, "\n  ", t2, " seconds\n");
        isFeasible(m, n, C, A, xbest, io, column);
        m_print(io, "z(xBest) = ", zBest, "\n");

        tt += t1+t2;

        /* MOST IMPORTANT SECTION */
        if(x) delete[] x, x = nullptr;
        if(xbest) delete[] xbest, xbest = nullptr;
        if(column) delete[] column, column = nullptr;
        freeSPP(C, A, U);
    }

    m_print(io, "\nTotal solving time : ", tt, " seconds\n");

    result_file.close();
    return 0;
}
