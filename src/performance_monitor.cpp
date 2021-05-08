#include "performance/performance_monitor.hpp"

namespace Performance {
PerformanceMonitor::PerformanceMonitor() {
  int retval;
  if ((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
    ERROR_RETURN(retval);
  }

  if ((retval = PAPI_create_eventset(&m_eventSet)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }

  if ((retval = PAPI_add_event(m_eventSet, PAPI_TOT_CYC)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }

  if ((retval = PAPI_add_event(m_eventSet, PAPI_TOT_INS)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }
}

PerformanceMonitor::~PerformanceMonitor() { PAPI_shutdown(); }

void PerformanceMonitor::Start() {
  int retval;
  if ((retval = PAPI_start(m_eventSet)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }
}

void PerformanceMonitor::Read() {
  int retval;
  if ((retval = PAPI_read(m_eventSet, m_values)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }
}

void PerformanceMonitor::Finish() {
  int retval = -1;
  if ((retval = PAPI_stop(m_eventSet, m_values)) != PAPI_OK) {
    ERROR_RETURN(retval);
  }
}

void PerformanceMonitor::Report() {
  printf("The total instructions executed for the first loop are %lld \n",
         m_values[0]);
  printf("The total cycles executed for the first loop are %lld \n",
         m_values[1]);
}

} // namespace Performance
