#include "librarySPP.hpp"

std::set<std::string> getfname(
        std::string pathtofolder,
        std::ostream** IO) {
    std::set<std::string> files;
    // Get all files from folder
    std::filesystem::directory_iterator dir(pathtofolder);

    if(!IO) std::cout << "Loading instances..." << std::endl;
    else m_print(IO, "Loading instances...\n");
    for(const auto &file : dir) {
        std::filesystem::path p(file.path());
        // Get filename
        std::string f(p.filename().c_str());

        // Not a hidden file
        if(f[0] != '.') {
            files.insert(f);
            if(!IO) std::cout << "fname = " << f << std::endl;
            else m_print(IO, "fname = ", f, "\n");
        }
    }

    if(!IO) std::cout << "DONE!" << std::endl;
    else m_print(IO, "DONE!\n");

    return files;
}

std::tuple<int, int, int*, char*, float*> loadSPP(std::string fname)
{
    std::ifstream f(fname);
    std::string line("");
    std::stringstream ss("");
    int m(-1), n(-1), *C(nullptr), i(0), j(0);
    char *A(nullptr); float *U(nullptr);

    try {
        if(f.is_open()) {
            // Read m (number of constraints) and n (number of variables)
            f >> m >> n; f.ignore();
            // Creates C, U and A. Init U and A elements to zero.
            C = new int[n], U = new float[n], A = new char[n*m];
            for(i = 0; i < n*m; i++) { A[i] = 0; if(i < n) U[i] = 0; }
            // Read the n coefficiens of the objective function and init C
            getline(f, line); ss.str(line); ss.clear(); while(ss >> C[j++]);
            // Read the m constraints and reconstruct matrix A
            for(j = 0; j < m; j++){
                // Read number of not null elements on constraint i (not used)
                getline(f, line);
                // Read indices of not null elements on constraint i
                getline(f, line); ss.str(line); ss.clear();
                while(ss >> i) A[INDEX(i-1, j)] = 1, U[i-1] += 1;
            }
            f.close();
        } else throw std::runtime_error("Couldn't open file " + fname);
    } catch(std::exception const& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }

    for(i = 0; i < n; i++) U[i] = C[i]/U[i];
    return std::make_tuple(m, n, C, A, U);
}

bool isFeasible(
        int m,
        int n,
        const int *C,
        const char *A,
        const char *x,
        std::ostream** IO,
        const char* extColumn,
        bool verbose) {
    bool feasible = true;
    int i(0), j(0), z(0), sum_xi(0);
    char *column(nullptr);
    if(!extColumn) {
        column = new char[m];
        for(j = 0; j < m; j++) column[j] = 0;
    } else column = (char*)extColumn;

    for(i = 0; i < n && feasible; i++) {
        // If variable i is selected then we add the i-th column of
        // matrix A to the variable column
        for(j = 0; x[i] && j < m && feasible; j++) {
            if(!extColumn) column[j] += A[INDEX(i, j)];
            // If an element of column is strictly greater than 1 then the
            // constraints are violated and x is not feasible
            feasible = feasible && column[j] >= 0 && column[j] <= 1;
        }
        sum_xi += x[i], z += x[i] * C[i];
    }

    if(verbose) {
        m_assert(feasible, "No feasible solution detected");
        if(!IO)
            std::cout << "Feasible : yes | Σ(x_i) = " << sum_xi
                << " ; z(x) = " << z << std::endl;
        else
            m_print(IO, "Feasible : yes | Σ(x_i) = ", sum_xi, " ; z(x) = ", z, "\n");
    }

    if(!extColumn) delete[] column;
    return feasible;
}

void freeSPP(int *C, char *A, float *U) {
    if(C) delete[] C, C = nullptr;
    if(A) delete[] A, A = nullptr;
    if(U) delete[] U, U = nullptr;
}
