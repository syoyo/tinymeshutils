

# Tiny Mesh processing utilities in C++11

Dependency-free, header-only tiny mesh processing utilities in C++11.

## Supported platform

* [x] Windows
* [x] Linux
* [ ] macOS(not tested, but should work)

## Features

* half-edge.hh : Compute half edge structure from polygons.
  * option: `TINYMESHUTILS_USE_ROBINMAP` Use robin-map, which would give better performance.

### Half-edge

```
bool BuildHalfEdge(const std::vector<uint32_t> &face_vert_indices,
                   const std::vector<uint32_t> &face_vert_counts,
                   std::vector<Edge> *edges, std::vector<HalfEdge> *halfedges,
                   std::string *err);
```

Example use is

```

// Define this in *one* .cc
#define TINYMESHUTILS_HALF_EDGE_IMPLEMENTATION
#include "half-edge.hh"

std::vector<uint32_t> face_vert_indices = ...;
std::vector<uint32_t> face_vert_counts = ...;
std::vector<tinymeshutils::Edge> edges; // array of edges(computed from `face_vert_counts` and `face_vert_indices`)
std::vector<tinymeshutils::HalfEdge> halfedges; // array of half-edges
std::vector<size_t> vertex_start_halfedge_indices; // starting half-edge index for each vertex.

std::string err; // error message

if (!BuildHalfEdge(face_vert_indices,
                   face_vert_counts,
                   &edge, &halfedges, &vertex_start_halfedge_indices,
                   &err)) {
  if (!err.empty())) {
    std::cerr << err << "\n";
  }
  return false;
}

// ok

//
// Get the starting half-edge for a given vertex index vid
// Since i'th halfedge corresnponds i'th vertex, you can use vertex index to look up half-edge
//

const tinymeshutils::HalfEdge &he = halfedges[vid];

//
// Get the starting half-edge for a given face fid
//

// Assume you'll built face_vert offset.
std::vector<uint32_t> face_vert_offset = ...;

uint32_t vid = face_vert_indices[face_vert_offset[fid]];
const tinymeshutils::HalfEdge &he = halfedges[vid];

```

#### TODO

* [ ] Add API with user supplied edge information.


## License

MIT license.

### Third party license

acutest(for unit test) : MIT license https://github.com/mity/ac utest
