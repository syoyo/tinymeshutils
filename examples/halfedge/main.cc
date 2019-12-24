#include <cstdio>
#include <cstdlib>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define TINYMESHUTILS_HALF_EDGE_IMPLEMENTATION
#include "half-edge.hh"

#include <iostream>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Need input.obj\n";
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              filename.c_str(), /* basepath */ "",
                              /* triangulate */ false);

  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cerr << "ERR: " << err << std::endl;
  }

  if (!ret) {
    printf("Failed to load/parse .obj.\n");
    return EXIT_FAILURE;
  }

  // Create single face indices.
  std::vector<uint32_t> face_vert_indices;
  std::vector<uint32_t> face_vert_counts;

  for (size_t s = 0; s < shapes.size(); s++) {
    for (size_t i = 0; i < shapes[s].mesh.indices.size(); i++) {
      face_vert_indices.push_back(
          uint32_t(shapes[s].mesh.indices[i].vertex_index));
    }

    for (size_t i = 0; i < shapes[s].mesh.num_face_vertices.size(); i++) {
      face_vert_counts.push_back(shapes[s].mesh.num_face_vertices[i]);
    }
  }

  size_t n = (face_vert_indices.size() > 128) ? 128 : face_vert_indices.size();
  for (size_t i = 0; i < n; i++) {
    std::cout << "f[" << i << "] = " << face_vert_indices[i] << "\n";
  }

  n = (face_vert_counts.size() > 128) ? 128 : face_vert_counts.size();
  for (size_t i = 0; i < n; i++) {
    std::cout << "fv[" << i << "] = " << face_vert_counts[i] << "\n";
  }

  std::vector<tinymeshutils::Edge> edges;
  std::vector<tinymeshutils::HalfEdge> halfedges;
  std::vector<int64_t> vertex_start_halfedge_indices;

  ret = tinymeshutils::BuildHalfEdge(face_vert_indices, face_vert_counts,
                                     &edges, &halfedges,
                                     &vertex_start_halfedge_indices, &err);

  if (!err.empty()) {
    std::cerr << "HE ERR: " << err << std::endl;
  }

  if (!ret) {
    std::cerr << "Failed to construct half-edge structure for a given mesh.\n";
    return EXIT_FAILURE;
  }

  std::cout << "===================\n";

  n = (vertex_start_halfedge_indices.size() > 128) ? 128 : vertex_start_halfedge_indices.size();
  for (size_t i = 0; i < n; i++) {
    std::cout << "v[" << i << "].halfedge_index = " << vertex_start_halfedge_indices[i] << "\n";
  }

  n = (halfedges.size() > 128) ? 128 : halfedges.size();
  for (size_t i = 0; i < n; i++) {
    std::cout << "halfedge[" << i << "].next = " << halfedges[i].next_halfedge << "\n";
  }

  // build offset table
  std::vector<uint32_t> f_offsets(face_vert_counts.size());
  f_offsets[0] = 0;
  for (size_t i = 1; i < face_vert_counts.size(); i++) {
    f_offsets[i] = f_offsets[i - 1] + face_vert_counts[i-1];
  }

  //
  // list up edges for the first face.
  // for cube.obj, this should print 4 half-edges
  //
  {
    // get the first vertex index for a face `face_id`.
    size_t face_id = 0; // Change this as you wish
    size_t v_id = face_vert_indices[f_offsets[face_id] + 0];
    std::cout << "v_id = " << v_id << "\n";
    int64_t he_id = vertex_start_halfedge_indices[v_id];
    if (he_id < 0) {
      std::cerr << "??? vertex does not have a link to half-edge\n";
      return EXIT_FAILURE;
    }
    int64_t halfedge_idx = he_id;

    std::cout << "starting halfedge index = " << he_id << "\n";
    uint32_t loop_max = 1000; // to avoid infinite looop.
    for (size_t i = 0; i < loop_max; i++) {

      const tinymeshutils::HalfEdge &halfedge = halfedges[size_t(halfedge_idx)];

      if ((halfedge.next_halfedge == -1)) {
        std::cout << "end traverse\n";
        break;
      }

      assert(halfedge.edge_index > -1);
      const tinymeshutils::Edge &edge = edges[size_t(halfedge.edge_index)];

      std::cout << "halfedge\n";
      std::cout << "  edge_index " << halfedge.edge_index << "\n";
      std::cout << "    (" << edge.v0 << ", " << edge.v1 << ")\n";
      std::cout << "  opposite_index " << halfedge.opposite_halfedge << "\n";
      std::cout << "  next " << halfedge.next_halfedge << "\n";

      halfedge_idx = halfedge.next_halfedge;
      if (halfedge_idx == he_id) {
        std::cout << "end traverse\n";
        break;
      }
    }
  }

  //
  // walk edges for a vertex.
  // for cube.obj, this should print 6 half-edges(3 edges * 2)
  //
  {
    size_t face_id = 0; // Change this as you wish
    size_t v_id = face_vert_indices[f_offsets[face_id] + 0];
    std::cout << "v_id = " << v_id << "\n";
    int64_t he_id = vertex_start_halfedge_indices[v_id];
    if (he_id < 0) {
      std::cerr << "??? vertex does not have a link to half-edge\n";
      return EXIT_FAILURE;
    }

    int64_t halfedge_idx = he_id;

    std::cout << "starting halfedge index = " << he_id << "\n";
    uint32_t loop_max = 100; // to avoid infinite looop.
    for (size_t i = 0; i < loop_max; i++) {

      tinymeshutils::HalfEdge &halfedge = halfedges[size_t(halfedge_idx)];

      {
        const tinymeshutils::Edge &edge = edges[size_t(halfedge.edge_index)];

        std::cout << "halfedge " << halfedge_idx << "\n";
        std::cout << "  edge_index " << halfedge.edge_index << "\n";
        std::cout << "    (" << edge.v0 << ", " << edge.v1 << ")\n";
        std::cout << "  opposite_index " << halfedge.opposite_halfedge << "\n";
        std::cout << "  next " << halfedge.next_halfedge << "\n";
      }

      if (halfedge.next_halfedge == -1) {
        std::cout << "end traverse\n";
        break;
      }

      assert(halfedge.edge_index > -1);

      if (halfedge.opposite_halfedge == -1) {
        std::cout << "boundary\n";
        std::cout << "end traverse\n";
        break;
      }

      const tinymeshutils::HalfEdge &twin = halfedges[size_t(halfedge.opposite_halfedge)];
      {
        const tinymeshutils::Edge &edge = edges[size_t(twin.edge_index)];

        std::cout << "halfedge(twin) " << halfedge.opposite_halfedge << "\n";
        std::cout << "  edge_index " << twin.edge_index << "\n";
        std::cout << "    (" << edge.v0 << ", " << edge.v1 << ")\n";
        std::cout << "  opposite_index " << twin.opposite_halfedge << "\n";
        std::cout << "  next " << twin.next_halfedge << "\n";
      }

      if (twin.next_halfedge == -1) {
        std::cout << "???\n";
        std::cout << "end traverse\n";
        break;
      }

      halfedge_idx = twin.next_halfedge;
      //halfedge = halfedges[size_t(twin.next_halfedge)];

      if (halfedge_idx == he_id) {
        std::cout << "end traverse\n";
        break;
      }


      if (i == (loop_max - 1)) {
        std::cout << "??? failed to walk\n";
      }
    }
  }

  return EXIT_SUCCESS;
}
