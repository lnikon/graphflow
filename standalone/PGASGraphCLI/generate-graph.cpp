// PGASGraph
#include <pgasgraph/graph-utilities.hpp>
#include <pgasgraph/pgasgraph.h>

// Usings
namespace po = boost::program_options;

int main(int argc, char *argv[]) {
  // Parse command line options.
  auto optionsDesc = GraphUtilities::createProgramOptions();
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optionsDesc), vm);
  if (vm.count("help")) {
    std::cout << optionsDesc << std::endl;
    return 1;
  }

  // Get vertex count.
  size_t totalNumberVertices = 256;
  size_t degree = 20;
  if (vm.count("vertex-count")) {
    totalNumberVertices = vm["vertex-count"].as<int>();
  }

  // Generate graph.
  auto g = GraphUtilities::generateSmallWorld(totalNumberVertices, degree);

  // Write graph.
  if (vm.count("output")) {
    const auto path = vm["output"].as<std::string>();
    std::ofstream outputStream;
    outputStream.open(path, std::fstream::in | std::fstream::out);
    if (!outputStream.is_open()) {
      throw std::runtime_error("Unable to open: " + path);
    }

    boost::write_graphviz(outputStream, g);
  }

  return 0;
};
