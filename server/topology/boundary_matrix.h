#ifndef BOUNDARY_MATRIX_H
#define BOUNDARY_MATRIX_H

#include <vector>
#include "simplicial_complex.h"

struct BMCell {
    BMCell();
    BMCell(int f, int s) { first = f; second = s; }

    int first; //global simplex ID
    int second; //complex ID (which this simplex belongs to in the context of a cover)

    void print() { std::cerr << "(" << first << ", " << second << ")"; }

    inline bool operator==(const BMCell& other) const{
        return this->first == other.first && this->second == other.second;
    }

    inline bool operator<(const BMCell& other) const{
        if(this->first != other.first) {
            return this->first < other.first;
        } else {
            return this->second < other.second;
        }
    }
};

struct BMCol {
    BMCol();

    BMCell header;
    std::vector<BMCell> faces;

    void print();

    inline bool operator<(const BMCol& other) const{
        return this->header < other.header;
    }
};

struct BMatrix{
    BMatrix();
    BMatrix(std::vector<BMCol> &cols);

    std::vector<BMCol> cols;

    int size() { return cols.size(); }
    void append(const BMatrix &other);
    void sort();
    void print();

    std::vector<int> getAllSimplicis();
    void setAllComplexID(int id);
};

#endif
