#include "librarySPP.hpp"
#include <string>

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

void modelSPP(
        std::string instance,
        std::string path,
        std::ostream** IO,
        float* tt) {
    int z(-1), m(-1), n(-1), ne(0), j(0), i(0), v(0);
    int *C(nullptr), *ia(nullptr), *ja(nullptr);
    double t(0.f), *ar(nullptr); INIT_TIMER();
    std::ifstream f(path + instance);
    std::string line("");
    std::stringstream ss("");

    /* Load data */
    try {
        if(f.is_open()) {
            // Read m (number of constraints) and n (number of variables)
            f >> m >> n; f.ignore();
            // Creates C, U and A. Init U and A elements to zero.
            C = new int[n], ia = new int[1+m], ja = new int[1+n];
            ar = new double[1+m*n];
            // Read the n coefficiens of the objective function and init C
            getline(f, line); ss.str(line); ss.clear(); while(ss >> C[i++]);
            // Read the m constraints and reconstruct matrix A
            for(i = 0; i < m; i++){
                // Read number of not null elements on constraint i (not used)
                getline(f, line);
                // Read indices of not null elements on constraint i
                getline(f, line); ss.str(line); ss.clear();
                j = 0;
                while(ss >> v) {
                    ia[ne+1] = i+1;
                    ja[ne+1] = j+1;
                    ar[ne+1] = (double)v;
                    ne += 1; j += 1;
                }
            }
            f.close();
        }
    } catch(std::exception const& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }

    m_print(IO, "\nInstance : ", instance, "\n\n");

    /* Create problem */
    glp_prob *lp = glp_create_prob();
    glp_set_prob_name(lp, "Set Packing Problem (SPP)");
    glp_set_obj_dir(lp, GLP_MAX);

    /* Model SPP */
    glp_add_rows(lp, m);
    for(i = 1; i < m+1; i++) {
        glp_set_row_name(lp, i, std::to_string(i).c_str());
        glp_set_row_bnds(lp, i, GLP_DB, 0, 1);
    }

    glp_add_cols(lp, n);
    for(j = 1; j < n+1; j++) {
        glp_set_col_kind(lp, j, GLP_IV);
        glp_set_obj_coef(lp, j, C[j-1]);
        glp_set_col_name(lp, j, std::string("x" + std::to_string(j)).c_str());
        glp_set_col_bnds(lp, j, GLP_DB, 0, 1);
    }

    glp_load_matrix(lp, ne, ia, ja, ar);

    /* Solve with simplex */
    TIMED(t, glp_simplex(lp, NULL)); (*tt) += t;
    z = glp_get_obj_val(lp);
    std::cout << "Résolue en " << t << " secondes. z_opt = " << z << std::endl;

    /* Free problem and arrays */
    glp_delete_prob(lp);
    delete[] C; delete[] ia; delete[] ja; delete[] ar;
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
