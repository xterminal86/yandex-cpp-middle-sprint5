#include <gtest/gtest.h>
#include <random>

#include "triangulation.hpp"
#include "shape_utils.hpp"
#include "convex_hull.hpp"
#include "visualization.hpp"

using namespace geometry;
using namespace geometry::triangulation;
using namespace geometry::utils;
using namespace geometry::convex_hull;
using namespace geometry::visualization;

TEST(TriangulationTest, SimpleCheck)
{
  std::mt19937 rng(0x123456);
  std::uniform_int_distribution<> distr(1, 10);

  std::vector<Point2D> points;

  std::string ptsCircles;

  for (size_t i = 0; i < 10; i++)
  {
    int x = distr(rng);
    int y = distr(rng);

    points.push_back({ (double)x, (double)y });
  }

  DelaunayResult res = DelaunayTriangulation(points);
  ASSERT_TRUE(res.has_value());
  Draw(res.value());
}
