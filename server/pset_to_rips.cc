#include <iostream>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <typeinfo>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "mongoose.h"
#include "json.hpp"

#include "geometry/covertree.h"

#include "topology/persistence.h"
#include "topology/sparse_rips_filtration.h"
#include "topology/rips_filtration.h"
#include "topology/simplicial_complex.h"

#include "topocubes.h"
#include "csv.h"

using json = nlohmann::json;

template <unsigned count> Points read_points(std::string filePath);

/******************************************************************************/
// ugly template specializations

template <>
Points read_points<2>(std::string filePath)
{
  io::CSVReader<2> in(filePath);
  double x, y;
  Points ps;
  while (in.read_row(x, y)) {
    vector<double> p {x, y};
    ps.push_back(p);
  }
  std::cerr << "Done, " << ps.size() << std::endl;
  return ps;
}

template <>
Points read_points<3>(std::string filePath)
{
  io::CSVReader<3> in(filePath);
  double x, y, z;
  Points ps;
  while (in.read_row(x, y, z)) {
    vector<double> p {x, y, z};
    ps.push_back(p);
  }
  std::cerr << "Done, " << ps.size() << std::endl;
  return ps;
}

template <>
Points read_points<4>(std::string filePath)
{
  io::CSVReader<4> in(filePath);
  double x, y, z, w;
  Points ps;
  while (in.read_row(x, y, z, w)) {
    vector<double> p {x, y, z, w};
    ps.push_back(p);
  }
  std::cerr << "Done, " << ps.size() << std::endl;
  return ps;
}

template <>
Points read_points<5>(std::string filePath)
{
  io::CSVReader<5> in(filePath);
  double x, y, z, w, w2;
  Points ps;
  while (in.read_row(x, y, z, w, w2)) {
    vector<double> p {x, y, z, w, w2};
    ps.push_back(p);
  }
  std::cerr << "Done, " << ps.size() << std::endl;
  return ps;
}

/******************************************************************************/

template <unsigned count>
void convert_rips(std::string filePath)
{
  Points ps = read_points<count>(filePath);
 
  Filtration *filtration = new RipsFiltration(ps, count);
  filtration->build_filtration();
  for (size_t i=0; i<filtration->filtration_size(); ++i) {
    Simplex c = filtration->get_simplex(i);
    std::vector<int> v = c.as_vector();
    std::copy(v.begin(), v.end(), ostream_iterator<int>(std::cout, " "));
    std::cout << c.get_simplex_distance() << "\n";
  }
  delete filtration;
}

template <unsigned count>
void convert_sparse_rips(std::string filePath, double max_d)
{
  Points ps = read_points<count>(filePath);
 
  Filtration *filtration = new SparseRipsFiltration(ps, count, max_d);
  filtration->build_filtration();
  for (size_t i=0; i<filtration->filtration_size(); ++i) {
    Simplex c = filtration->get_simplex(i);
    std::vector<int> v = c.as_vector();
    std::copy(v.begin(), v.end(), ostream_iterator<int>(std::cout, " "));
    std::cout << c.get_simplex_distance() << "\n";
  }
  delete filtration;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Error, expected at least two parameters." << std::endl;
    exit(1);
  }
  std::string filePath(argv[1]);
  int dim = atoi(argv[2]);
  bool sparse = false;
  double max_d = 0.0;
  if (argc > 3) {
    sparse = string(argv[4]) == "-s";
    max_d = atof(argv[3]);
  }

  if (!sparse) {
    switch (dim) {
    case 2: convert_rips<2>(filePath); break;
    case 3: convert_rips<3>(filePath); break;
    case 4: convert_rips<4>(filePath); break;
    case 5: convert_rips<5>(filePath); break;
    default:
      std::cerr << "cannot handle point sets of dimension " << dim << std::endl;
      exit(1);
    }
  } else {
    switch (dim) {
    case 2: convert_sparse_rips<2>(filePath, max_d); break;
    case 3: convert_sparse_rips<3>(filePath, max_d); break;
    case 4: convert_sparse_rips<4>(filePath, max_d); break;
    case 5: convert_sparse_rips<5>(filePath, max_d); break;
    default:
      std::cerr << "cannot handle point sets of dimension " << dim << std::endl;
      exit(1);
    }
  }
}
