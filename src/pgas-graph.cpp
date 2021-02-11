#include <pgas-graph/pgas-graph.h>

namespace PGASGraph {

void logMsg(const std::string &msg) {
#ifdef DEBUG_LOG_MSG
  std::cout << "[" << upcxx::rank_me() << "/" << upcxx::rank_n() << "] " << msg
            << "\n";
#endif
}

} // PGASGraph