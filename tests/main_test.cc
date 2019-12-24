#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif

#include "acutest.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#define TINYMESHUTILS_HALF_EDGE_IMPLEMENTATION
#include "../half-edge.hh"

#include <iostream>

static void test_half_edge() {
  std::vector<uint32_t> indices{0, 1, 2, 3, 1, 4, 2};
  std::vector<uint32_t> counts{4, 3};

  std::vector<tinymeshutils::Edge> edges;
  std::vector<tinymeshutils::HalfEdge> halfedges;
  std::vector<int64_t> vertex_start_halfedge_indices;
  std::string err;

  bool ret = tinymeshutils::BuildHalfEdge(indices, counts, &edges, &halfedges,
                                          &vertex_start_halfedge_indices, &err);
  TEST_CHECK(true == ret);
  std::cerr << err << "\n";
}

static void test_invalid_face() {
  std::vector<uint32_t> indices{0, 1, 2, 3};
  std::vector<uint32_t> counts{2, 3};

  std::vector<tinymeshutils::Edge> edges;
  std::vector<tinymeshutils::HalfEdge> halfedges;
  std::vector<int64_t> vertex_start_halfedge_indices;
  std::string err;

  bool ret = tinymeshutils::BuildHalfEdge(indices, counts, &edges, &halfedges,
                                          &vertex_start_halfedge_indices, &err);
  TEST_CHECK(false == ret);
  std::cerr << err << "\n";
}

static void test_invalid_topology() {
  std::vector<uint32_t> indices{0, 1, 2, 0, 1, 2};
  std::vector<uint32_t> counts{3, 3};

  std::vector<tinymeshutils::Edge> edges;
  std::vector<tinymeshutils::HalfEdge> halfedges;
  std::vector<int64_t> vertex_start_halfedge_indices;
  std::string err;

  bool ret = tinymeshutils::BuildHalfEdge(indices, counts, &edges, &halfedges,
                                          &vertex_start_halfedge_indices, &err);
  TEST_CHECK(false == ret);
  std::cerr << err << "\n";
}

TEST_LIST = {{"test_half_edge", test_half_edge},
             {"test_invalid_face", test_invalid_face},
             {"test_invalid_topology", test_invalid_topology},
             {nullptr, nullptr}};
