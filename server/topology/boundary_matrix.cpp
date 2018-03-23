#include "boundary_matrix.h"
#include <algorithm>


BMCell::BMCell() {
    first = 0;
    second = 0;
}
///////////////////////////////////////////////////////////////////////////////

BMCol::BMCol() {
    header = BMCell(0, 0);
    faces = std::vector<BMCell>();
}

///////////////////////////////////////////////////////////////////////////////

void BMCol::print() {
    header.print();
    std::cerr << "|";
    for(auto & e: faces) {
        e.print();
        std::cerr << " ";
    }
    std::cerr << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
BMatrix::BMatrix() {
  std::vector<BMCol> cols;
  cols.push_back(BMCol());
  this->cols = cols;
}

BMatrix::BMatrix(std::vector<BMCol> &_cols) {
    this->cols = _cols;
}

void BMatrix::append(const BMatrix &other) {
    // TODO use move_iterator for better performance
    BMCell zero(0,0);
    for(auto& e : other.cols) {
        if(e.header == zero) continue;
        this->cols.push_back(e);
    }
}

void BMatrix::sort() {
    std::sort(cols.begin(), cols.end());
}

void BMatrix::print() {
    for(auto & c: cols) {
        c.print();
    }
}

std::vector<int> BMatrix::getAllSimplicis() {
  // XXX should not contain the inserted 0
  // TODO more efficient implementation
  std::vector<int> simplicis;
  BMCell zero(0,0);
  for(auto &c : cols) {
    if(c.header == zero) continue;
    simplicis.push_back(c.header.first);
  }
  return simplicis;
}

void BMatrix::setAllComplexID(int id) {
  // don't change BMCell(0,0)
  for(int i = 0; i < cols.size(); i ++) {
    if(cols[i].header.first == 0 && cols[i].header.second == 0) {
    } else {
      cols[i].header.second = id;
    }
    for(auto &face : cols[i].faces) {
      if(face.first == 0 && face.second ==0) {
      } else {
        face.second = id;
      }
    }
  }
}
