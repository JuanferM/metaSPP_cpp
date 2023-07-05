#include "heuristics.hpp"

#define DEEPSEARCH    true
#define USE_GLPK    false
#define VERBOSE_GLPK  false

int main() {
  std::string pwd(std::filesystem::current_path());
  std::string path(pwd + "/../instances/");
  std::ostream* io[1] = {&std::cout};
  std::cout.precision(6);
  float tt(0.f);

  #if !USE_GLPK
  int m(-1), n(-1), zInit(-1), zBest(-1);
  char *x(nullptr), *xbest(nullptr), *column(nullptr);
  float t1(0.f), t2(0.f); INIT_TIMER();
  std::unique_ptr<int[]> C;
  std::unique_ptr<char[]> A;
  std::unique_ptr<float[]> U;
  #endif

  m_print(*io[0], _CLRd, "Etudiants : MERCIER et PICHON\n", _CLR);
  std::set<std::string> fnames = getfname(path, io);
  for(auto instance : fnames) {
    #if USE_GLPK
      modelSPP(instance, path, io, &tt, VERBOSE_GLPK);
    #else
      // Load one numerical instance
      std::tie(m, n, C, A, U) = loadSPP(path + instance);
      m_print(*io[0], _CLB, "\n\nInstance : ", instance, "\n\n", _CLR);

      zInit = 0; zBest = 0;

      m_print(*io[0], "Construction gloutonne d'une solution admissible\n");
      TIMED(t1, std::tie(x, zInit, column) = GreedyConstruction(io, m, n, C.get(), A.get(), U.get()));
      m_print(*io[0], "  ", t1, " seconds\n");
      isFeasible(m, n, C.get(), A.get(), x, io, column);
      m_print(*io[0], "z(xInit) = ", zInit, "\n\n");

      m_print(*io[0], "Amelioration par recherche locale de type ",
          DEEPSEARCH ? "plus profonde " : "", "descente");
      TIMED(t2,
        std::tie(xbest, zBest) = GreedyImprovement(io, m, n, C.get(), A.get(),
          x, zInit, DEEPSEARCH, column));
      m_print(*io[0], "\n  ", t2, " seconds\n");
      isFeasible(m, n, C.get(), A.get(), xbest, io, column);
      m_print(*io[0], "z(xBest) = ", zBest, "\n");

      tt += t1+t2;

      if(x) delete[] x, x = nullptr;
      if(xbest) delete[] xbest, xbest = nullptr;
      if(column) delete[] column, column = nullptr;
    #endif
  }

  m_print(*io[0], _CLP, "\nTotal solving time : ", tt, " seconds\n", _CLR);

  glp_free_env();
  return 0;
}
