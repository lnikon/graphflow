#include <iostream>
#include <cmath>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "<program-name> <vertex-count>:unsigned-long-long <percentage>:double" << std::endl;
    return 1; 
  }

  const unsigned long long vertexCount {std::stoull(argv[1])};
  const double percentage {std::stod(argv[2])};
  const unsigned long long edgeCount {((vertexCount * vertexCount) / 2) * (percentage / 100)};
  std::cout << "vertexCount=" << vertexCount 
        << ",\npercentage=" << percentage 
        << ",\nedgeCount=" << edgeCount << std::endl; 

  return 0;
}
