// PGASGraph
#include <pgasgraph/graph-utilities.hpp>
#include <pgasgraph/pgasgraph.h>

// STL
#include <chrono>
#include <iostream>
#include <random>

// Boost
#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

int main() {
	using namespace boost;

	using Graph = adjacency_list<vecS, vecS, undirectedS,
			 property<vertex_distance_t, int>,
			 property<edge_weight_t, int>>;

	using E = std::pair<int, int>;

	const int num_nodes = 1024;
	const int percentage = 10;

	Graph g{GraphUtilities::generateRandomConnectedGraph<Graph>(
			num_nodes, percentage)};

	// Assign weights to the edges.
	auto edges = boost::edges(g);
	int weight{0};
	// for (auto it = edges.first; it != edges.second; ++it) {
	// 	// g[*it] = weight++;
	// }

	std::vector<graph_traits<Graph>::vertex_descriptor> p(num_vertices(g));
	prim_minimum_spanning_tree(g, &p[0]);
	for (std::size_t i = 0; i != p.size(); ++i) {
		if (p[i] != i) {
			std::cout << "parent[" << i << "] = " << p[i] << std::endl;
		} else {
			std::cout << "parent[" << i << "] = no parent" << std::endl;
		}
	}

	return EXIT_SUCCESS;
}
