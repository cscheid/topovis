#include <algorithm>
#include <iterator>

#include "cover.h"
#include "string"

Cover::Cover() {
}

Cover::Cover(const SimplicialComplex &sc, std::map<int,int> &vertex_map) {
    // Get the simplex ID mapping
    this->SimplexIDMap = sc.get_simplex_map();

    this->globalComplex = sc;

    // TODO we can only keep the indices for each subcomplex to reduce memory
    // usage

    // Calculate base sub-complexes
    std::map<int, std::vector<Simplex> > subcomplex_map;

    for(auto it = sc.allSimplicis.begin(); it != sc.allSimplicis.end(); it ++) {
        int dest = vertex_map[it->min_vertex()];

        if(subcomplex_map.find(dest) == subcomplex_map.end()) {
            subcomplex_map[dest] = std::vector<Simplex>();
        }
        subcomplex_map[dest].push_back(*it);
    }

    for(auto it = subcomplex_map.begin(); it != subcomplex_map.end(); it ++) {
        std::set<int> k;
        k.insert(it->first);

        subComplexes[k] = SimplicialComplex(it->second, true);
        subcomplex_IDs[k] = std::vector<int>();

        for(auto& e : subComplexes[k].allSimplicis) {
            subcomplex_IDs[k].push_back(SimplexIDMap[e.id()]);
        }
    }

    // calculate intersection
    auto base_set = subcomplex_IDs;
    auto prev_set = subcomplex_IDs;
    while( prev_set.size() > 0) {
        std::map<std::set<int>, std::vector<int> > next_set;

        for(auto & prev : prev_set) {
            for(auto & base : base_set) {
                std::set<int> intersection_id;
                std::set_union(prev.first.begin(), prev.first.end(),
                               base.first.begin(), base.first.end(),
                               std::inserter(intersection_id, intersection_id.end()));
                if(prev.first.count(*base.first.begin()) > 0) {
                    // this means base complex is already in intersection
                    continue;
                }

                if( next_set.count(intersection_id) != 0){
                    // we've already computed this
                    continue;
                }

                std::vector<int> intersection_complex;
                std::set_intersection(prev.second.begin(),
                                      prev.second.end(),
                                      base.second.begin(),
                                      base.second.end(),
                                      std::back_inserter(intersection_complex));
                if(intersection_complex.size() > 0) {
                    next_set[intersection_id] = intersection_complex;
                }
            }
        }
        subcomplex_IDs.insert(next_set.begin(), next_set.end());
        prev_set = next_set;
    }

    //for(auto &c : subcomplex_IDs) {
        //int cid = 0;
        //for(auto &e : c.first) {
            //cid += e;
            //cid *= 10;
            //std::cerr << e;
        //}
        //cid /= 10;
        //IntersectionIDMap[c.first] = cid;
    //}
    for(auto &e : subcomplex_IDs) {
        IntersectionIDMap[e.first] = IntersectionIDMap.size()+1;
    }

    // print
    //std::cerr << "blowup complex size: " << subcomplex_IDs.size() << std::endl;
    //for(auto &e : subcomplex_IDs) {
        //auto index_set = e.first;
        //IntersectionIDMap[index_set] = IntersectionIDMap.size()+1;

        //for(auto &n : index_set) {
            //std::cerr << n;
        //}
        //std::cerr << " | ";
        //auto IDs = e.second;
        //for(auto &v : IDs) {
            //std::cerr << v << " ";
        //}
        //std::cerr << std::endl;
    //}
}

Cover::Cover(const SimplicialComplex &sc, std::vector<int> &q, std::map<int, BMatrix> &topocubes) {
    // Get the simplex ID mapping
    this->SimplexIDMap = sc.get_simplex_map();

    this->globalComplex = sc;

    // TODO we can only keep the indices for each subcomplex to reduce memory
    // usage

    // Get base sub-complexes
    for(auto & i : q) {
      std::set<int> cid;
      cid.insert(i);
      this->subcomplex_IDs[cid] = topocubes[i].getAllSimplicis();
    }

    // calculate intersection
    auto base_set = subcomplex_IDs;
    auto prev_set = subcomplex_IDs;
    while( prev_set.size() > 0) {
        std::map<std::set<int>, std::vector<int> > next_set;

        for(auto & prev : prev_set) {
            for(auto & base : base_set) {
                std::set<int> intersection_id;
                std::set_union(prev.first.begin(), prev.first.end(),
                               base.first.begin(), base.first.end(),
                               std::inserter(intersection_id, intersection_id.end()));
                if(prev.first.count(*base.first.begin()) > 0) {
                    // this means base complex is already in intersection
                    continue;
                }

                if( next_set.count(intersection_id) != 0){
                    // we've already computed this
                    continue;
                }

                std::vector<int> intersection_complex;
                std::set_intersection(prev.second.begin(),
                                      prev.second.end(),
                                      base.second.begin(),
                                      base.second.end(),
                                      std::back_inserter(intersection_complex));
                if(intersection_complex.size() > 0) {
                    next_set[intersection_id] = intersection_complex;
                }
            }
        }
        subcomplex_IDs.insert(next_set.begin(), next_set.end());
        prev_set = next_set;
    }

    for(auto &e : subcomplex_IDs) {
        IntersectionIDMap[e.first] = IntersectionIDMap.size()+1;
    }
}
