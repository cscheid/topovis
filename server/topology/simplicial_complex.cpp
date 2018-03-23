#include "simplicial_complex.h"


SimplicialComplex::SimplicialComplex() {

}

SimplicialComplex::SimplicialComplex
(std::vector<Simplex> &faces, bool from_faces, double** distances) {
    if(from_faces) {
        // FIXME the ordered is WRONG after building complex from faces
        //std::set<Simplex, global_compare> face_set(faces.begin(), faces.end());
        std::set<Simplex, lex_compare> face_set;

        std::vector<Simplex> queue = faces;
        while(queue.size() > 0) {
          auto e = queue.back();
          face_set.insert(e);
          queue.pop_back();

          if(e.dim() > 0) {
            std::vector<Simplex> face_to_add = e.faces(distances);
            for(auto it_inner = face_to_add.begin();
                it_inner != face_to_add.end(); it_inner ++) {
              face_set.insert(*it_inner);
              queue.push_back(*it_inner);
            }
          }
        }

        // TODO use move_iterator for better performance
        allSimplicis = std::vector<Simplex>(face_set.begin(), face_set.end());
        global_compare c;
        //std::sort(allSimplicis.begin(), allSimplicis.end(), c);
    } else {
        allSimplicis = faces;
    }
}

std::map<std::string, int> SimplicialComplex::get_simplex_map() const {
    std::map<std::string, int> map;
    for(auto it = allSimplicis.begin(); it != allSimplicis.end(); it ++) {
        map[it->id()] = map.size()+1;
    }
    return map;
}

void SimplicialComplex::print() const {
    std::cerr << "{ ";
    for(int i = 0; i < this->allSimplicis.size(); i ++) {
        if(i > 0) { std::cerr << ", "; }
        std::cerr << "{" << this->allSimplicis[i] << "}";
    }
    std::cerr << " }" << std::endl;
}

std::vector<Simplex> SimplicialComplex::cofacesOf(Simplex s) {
  // TODO need special data structure so finding cofaces could be faster
  //std::cerr << "calculating cofaces" << std::endl;
  std::vector<Simplex> cofaces;
  cofaces.push_back(s);
  for(auto & e: this->allSimplicis) {
    //std::cerr << "simplex: " << e << std::endl;
    if(e.dim() > s.dim()) {
      auto faces = e.faces();
      //for(auto & f : faces) {
        ////std::cerr << "face: " << f << std::endl;
        //if(f.as_vector() == s.as_vector()) {
          //cofaces.push_back(e);
        //}
      //}
      if(std::find(faces.begin(), faces.end(), s) != faces.end()) {
        cofaces.push_back(e);
      }
    }
  }
  return cofaces;
}

void SimplicialComplex::recalculate_distances(double** distances) {
  for(auto &e : allSimplicis) {
    if(e.get_simplex_distance() == -1) {
      e.compute_simplex_distance(distances);
    }
  }
}

void SimplicialComplex::deleteSimplex(Simplex &s) {
  for(auto it = allSimplicis.begin(); it != allSimplicis.end(); it ++) {
    if (*it == s) {
      allSimplicis.erase(it);
      return;
    }
  }
}
