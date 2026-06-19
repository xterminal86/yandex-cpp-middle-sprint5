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

void ParseAndDrawHull(const std::vector<Point2D>& points,
                      const std::string& plotTitle = std::string())
{
  std::vector<Shape> shapes;

  std::span<Point2D> ref = const_cast<std::vector<Point2D>&>(points);

  GrahamScanResult convexHull = GrahamScan(ref);
  if (not convexHull)
  {
    std::println("{}:{} - convexHull is null!", __FILE__, __LINE__);
    return;
  }

  Polygon p(convexHull.value());

  shapes.push_back(p);

  std::string ptsCircles;

  for (auto& p : points)
  {
    std::string s = std::format("circle {} {} 0.05; ", p.x, p.y);
    ptsCircles += s;
  }

  std::vector<Shape> pts = ParseShapes(ptsCircles);
  for (auto& i : pts)
  {
    shapes.push_back(i);
  }

  Draw(shapes, "ch", plotTitle);
}

// =============================================================================

void ParseAndDrawHullCorrect(const std::vector<Point2D>& points,
                             const std::string& plotTitle = std::string())
{
  std::vector<Shape> shapes;

  GrahamScanResult convexHull = GrahamScanCorrect(points);
  if (not convexHull)
  {
    std::println("{}:{} - convexHull is null!", __FILE__, __LINE__);
    return;
  }

  Polygon p(convexHull.value());

  shapes.push_back(p);

  std::string ptsCircles;

  for (auto& p : points)
  {
    std::string s = std::format("circle {} {} 0.05; ", p.x, p.y);
    ptsCircles += s;
  }

  std::vector<Shape> pts = ParseShapes(ptsCircles);
  for (auto& i : pts)
  {
    shapes.push_back(i);
  }

  Draw(shapes, "ch", plotTitle);
}

// =============================================================================

std::vector<Point2D> GetRandomPoints(const uint64_t seed)
{
  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<> distr(1, 10);

  std::vector<Point2D> points;

  for (size_t i = 0; i < 10; i++)
  {
    int x = distr(rng);
    int y = distr(rng);

    points.push_back({ (double)x, (double)y });
  }

  return points;
}

// =============================================================================

TEST(ConvexHull, SimpleCheck)
{
  ParseAndDrawHullCorrect(
    {
      { 0, 0 }, { 1, 1 }, { 2, 2 }, { 2, 0 },
      { 3, 1 }, { 4, 0 }, { 4, 2 }, { 3, 3 }
    },
    "Convex Hull simple"
  );
}

// =============================================================================

TEST(ConvexHullTest, Bad)
{
  std::vector<uint64_t> badSeeds =
  {
      0x804acbd6debcb374
    , 0x1a50744f31d83b5e
    , 0x79bfbd338f06b6e1
    , 0x508d849e73634b5a
    , 0x2b8b21d6137137a6
    , 0xa88e419190620f49
    , 0xd85c7036c0d430b6
    , 0x89b937b8ba3fd3a
    , 0x1b1db1cf580c0e12
    , 0xb98ae7fc4d924976
  };

  for (const uint64_t seed : badSeeds)
  {
    std::vector<Point2D> randomPoints = GetRandomPoints(seed);

    std::string seedStr = std::format("bad-{:#x}", seed);

    ParseAndDrawHull(randomPoints, seedStr);
  }
}

// =============================================================================

TEST(ConvexHullTest, Gud)
{
  std::vector<uint64_t> badSeeds =
  {
      0x804acbd6debcb374
    , 0x1a50744f31d83b5e
    , 0x79bfbd338f06b6e1
    , 0x508d849e73634b5a
    , 0x2b8b21d6137137a6
    , 0xa88e419190620f49
    , 0xd85c7036c0d430b6
    , 0x89b937b8ba3fd3a
    , 0x1b1db1cf580c0e12
    , 0xb98ae7fc4d924976
  };

  for (const uint64_t seed : badSeeds)
  {
    std::vector<Point2D> randomPoints = GetRandomPoints(seed);

    std::string seedStr = std::format("good-{:#x}", seed);

    ParseAndDrawHullCorrect(randomPoints, seedStr);
  }
}

// =============================================================================

TEST(ConvexHullTest, Random)
{
  std::vector<uint64_t> seeds =
  {
      0x123456
    , 0x1234567
    , 0x12345678
    , 0x12345679
    , 0xdeadbeef
    , 0xcafebabe
    , 0xbadbeef
    , 0xface
    , 0x804acbd6debcb374
    , 0x1a50744f31d83b5e
    , 0x79bfbd338f06b6e1
    , 0x508d849e73634b5a
    , 0x2b8b21d6137137a6
    , 0xa88e419190620f49
    , 0xd85c7036c0d430b6
    , 0x89b937b8ba3fd3a
    , 0x1b1db1cf580c0e12
    , 0xb98ae7fc4d924976
  };

  /*
  std::vector<uint64_t> seeds;

  for (size_t i = 0; i < 20; i++)
  {
    std::mt19937_64 rng(std::random_device{}());
    seeds.push_back(rng());
  }
  */

  for (uint64_t seed : seeds)
  {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<> distr(1, 10);

    std::vector<Point2D> points;

    std::string ptsCircles;

    for (size_t i = 0; i < 10; i++)
    {
      int x = distr(rng);
      int y = distr(rng);

      points.push_back({ (double)x, (double)y });
    }

    std::string seedStr = std::format("normal-{:#x}", seed);

    ParseAndDrawHull(points, seedStr);

    seedStr = std::format("fixed-{:#x}", seed);

    ParseAndDrawHullCorrect(points, seedStr);
  }
}
