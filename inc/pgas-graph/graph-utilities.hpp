#ifndef GRAPH_UTILITIES_HPP
#define GRAPH_UTILITIES_HPP

// STL
#include <chrono>
#include <iostream>
#include <random>

// Boost
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/program_options.hpp>
#include <boost/random/linear_congruential.hpp>

// Usings
namespace po = boost::program_options;

namespace GraphUtilities
{
using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using SWGen = boost::small_world_iterator<boost::minstd_rand, GraphType>;

po::options_description createProgramOptions();

GraphType generateSmallWorld(int size, int degree);

int driver(int argc, char *argv[]);

} // namespace GraphUtilities

#endif // GRAPH_UTILITIES_HPP