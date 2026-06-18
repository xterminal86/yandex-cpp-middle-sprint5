#include <gtest/gtest.h>
#include <random>
#include <print>

#include "shape_utils.hpp"
#include "convex_hull.hpp"
#include "visualization.hpp"

using namespace geometry;
using namespace geometry::utils;
using namespace geometry::convex_hull;
using namespace geometry::visualization;

TEST(ConvexHullTest, SimpleCheck)
{
  std::vector<Point2D> points =
  {
    { 1, 1 }, { 2, 1 }, { 3, 3 }, { 0, 3 }
  };

  GrahamScanResult convexHull = GrahamScan(points);

  Polygon p(convexHull.value());

  std::vector<Shape> shapes = { p };

  Draw(shapes);
}

TEST(ConvexHullTest, Random)
{
  std::mt19937 rng(0x123456);
  std::uniform_int_distribution<> distr(1, 10);

  std::vector<Point2D> points;

  std::string ptsCircles;

  for (size_t i = 0; i < 10; i++)
  {
    int x = distr(rng);
    int y = distr(rng);

    std::string s = std::format("circle {} {} 0.1; ", x, y);
    ptsCircles += s;

    points.push_back({ (double)x, (double)y });
  }

  std::vector<Shape> shapes = ParseShapes(ptsCircles);

  GrahamScanResult convexHull = GrahamScan(points);

  Polygon hull(convexHull.value());

  shapes.push_back(hull);

  Draw(shapes);
}
