#include <upcxx/upcxx.hpp>

#include <pgas-graph/pgas-graph.h>

namespace PGASGraph {

  void logMsg(const std::string& msg) {
#ifdef DEBUG_LOG_MSG
    upcxx::rpc(0, [](const auto& r, const auto& msg) {
      std::cout << "[" << r << "/" << upcxx::rank_n() << "] " << msg
        << std::endl;
      }, upcxx::rank_me(), msg).then([](){});
#endif
  }

  void logMsg(const std::string& msg, const size_t rank) {
#ifdef DEBUG_LOG_MSG
    if (upcxx::rank_me() == rank) {
      std::cout << "[" << upcxx::rank_me() << "/" << upcxx::rank_n() << "] "
        << msg << std::endl;
    }
#endif
  }

} // namespace PGASGraph
