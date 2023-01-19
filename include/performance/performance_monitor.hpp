#ifndef PERFORMANCE_MONITOR_HPP
#define PERFORMANCE_MONITOR_HPP

#include "papi.h"
#include <stdio.h>
#include <stdlib.h>

namespace Performance
{

/* Got from PAPI examples */
#define ERROR_RETURN(retval)                                                                       \
    {                                                                                              \
        fprintf(stderr, "Error %d %s:line %d: \n", retval, __FILE__, __LINE__);                    \
        exit(retval);                                                                              \
    }

/*!
 * \brief This class acts as an wrapper for PAPI.
 */
class PerformanceMonitor final
{
public:
    PerformanceMonitor();
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(PerformanceMonitor&&) = delete;
    ~PerformanceMonitor();

    /*!
     * \brief Start performance monitoring.
     */
    void Start();

    /*!
     * \brief Read counter values collected so far.
     */
    void Read();

    /*!
     * \brief Finish performance monitoring.
     */
    void Finish();

    /*!
     * \brief Report event counter values.
     */
    void Report();

private:
    static const int NUM_EVENTS{2};
    int m_eventSet{PAPI_NULL};
    long long m_values[NUM_EVENTS];
};
} // namespace Performance

#endif // PERFORMANCE_MONITOR_HPP
