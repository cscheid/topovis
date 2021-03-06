#ifndef COVER_H
#define COVER_H

#include "simplicial_complex.h"
#include "boundary_matrix.h"

#include<vector>
#include<map>


class Cover {
	public:
    Cover();
    Cover(const SimplicialComplex &sc, std::map<int,int> &vertex_map);
    Cover(const SimplicialComplex &sc, std::vector<int> &q, std::map<int, BMatrix> &topocubes);

    int subComplexCount() const { return this->subComplexes.size(); }

    /*************** Members ****************/
    SimplicialComplex globalComplex;

    std::map<std::set<int>, SimplicialComplex> subComplexes; // only base complex
    std::map<std::set<int>, std::vector<int> > subcomplex_IDs; // inlcuding intersection

    std::vector<int> intersection;

    // we need this to preserve the order of simplices in subComplexes
    std::map<std::string, int> SimplexIDMap;
    // so we can only store an integer in matrix instead of a set
    std::map<std::set<int>, int> IntersectionIDMap;
};

#endif
