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

GrahamScanResult ParseAndDrawHull(const std::vector<Point2D>& points,
                                  const std::string& plotTitle = std::string())
{
  std::vector<Shape> shapes;

  std::span<Point2D> ref = const_cast<std::vector<Point2D>&>(points);

  GrahamScanResult convexHull = GrahamScan(ref);
  if (not convexHull)
  {
    std::println("{}:{} - convexHull is null!", __FILE__, __LINE__);
    return convexHull;
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

  return convexHull;
}

// =============================================================================

GrahamScanResult ParseAndDrawHullCorrect(
  const std::vector<Point2D>& points,
  const std::string& plotTitle = std::string()
)
{
  std::vector<Shape> shapes;

  GrahamScanResult convexHull = GrahamScanCorrect(points);
  if (not convexHull)
  {
    std::println("{}:{} - convexHull is null!", __FILE__, __LINE__);
    return convexHull;
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

  return convexHull;
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

//
// If convex hull is correct, every points from the input data must lie inside
// the hull or on its edge(s). To check that we check that cross product between
// vector going from every input point to hull points always yields the same
// sign, meaning for every vector pair vector multiplication is performed in the
// same direction (CW or CCW).
//
bool TestConvexHullInvariant1(const std::vector<Point2D>& input,
                              const std::vector<Point2D>& hull)
{
  double epsilon = std::numeric_limits<double>::epsilon();

  std::vector<double> signs;
  for (const Point2D& inPoint : input)
  {
    size_t ln = hull.size();
    for (size_t i = 0; i < ln; i++)
    {
      Point2D hp1 = hull[i];
      Point2D hp2 = hull[(i + 1) % ln];
      Point2D hullsEdge = hp2 - hp1;
      Point2D pointToHullEdge = inPoint - hull[i];
      double cp = hullsEdge.Cross(pointToHullEdge);
      //bool isZero = (std::abs(cp) < epsilon);
      //std::println("{:.4f}, is zero? {}", cp, isZero ? "Y" : "N" );
      signs.push_back(cp);
    }
  }

  bool geqZero = true;
  bool leqZero = true;

  for (double d : signs)
  {
    bool isZero = (std::abs(d) < epsilon);
    if (not isZero and d > 0.0)
    {
      leqZero = false;
    }

    if (not isZero and d < 0.0)
    {
      geqZero = false;
    }
  }

  //std::println("all > 0 ? {}, all < 0 ? {}", geqZero, leqZero);

  return (geqZero or leqZero);
}

// =============================================================================

//
// If convex hull is correct, all its two consecutive edges must yield the same
// sign of cross product since convex hull formation enforces vertex ordering.
//
bool TestConvexHullInvariant2(const std::vector<Point2D>& hull)
{
  double epsilon = std::numeric_limits<double>::epsilon();

  std::vector<double> signs;

  size_t ln = hull.size();
  for (size_t i = 0; i < ln; i++)
  {
    Point2D p1 = hull[i];
    Point2D p2 = hull[(i + 1) % ln];
    Point2D p3 = hull[(i + 2) % ln];

    Point2D edge1 = p2 - p1;
    Point2D edge2 = p3 - p2;

    double cp = edge1.Cross(edge2);
    signs.push_back(cp);
  }

  bool geqZero = true;
  bool leqZero = true;

  for (double d : signs)
  {
    bool isZero = (std::abs(d) < epsilon);
    if (not isZero and d > 0.0)
    {
      leqZero = false;
    }

    if (not isZero and d < 0.0)
    {
      geqZero = false;
    }
  }

  return (geqZero or leqZero);
}

// =============================================================================

TEST(ConvexHull, SimpleCheck)
{
  {
    const std::vector<Point2D> input =
    {
      { 0, 0 }, { 1, 1 }, { 2, 2 }, { 2, 0 },
      { 3, 1 }, { 4, 0 }, { 4, 2 }, { 3, 3 }
    };

    GrahamScanResult hull = ParseAndDrawHullCorrect(input, "Convex Hull simple");
    ASSERT_TRUE(hull.has_value());
    bool inv1 = TestConvexHullInvariant1(input, hull.value());
    bool inv2 = TestConvexHullInvariant2(hull.value());
    EXPECT_TRUE( inv1 and inv2 );
  }
  // ---------------------------------------------------------------------------
  // Less than 3 points
  {
    const std::vector<Point2D> input =
    {
      { 0, 0 }
    };
    GrahamScanResult hull = ParseAndDrawHullCorrect(input, "insufficient points");
    ASSERT_FALSE(hull.has_value());
    std::println("{}", hull.error());
  }
  // ---------------------------------------------------------------------------
  // Impossible to form convex hull.
  {
    const std::vector<Point2D> input =
    {
      { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }
    };
    GrahamScanResult hull = ParseAndDrawHullCorrect(input, "cannot form");
    ASSERT_FALSE(hull.has_value());
    std::println("{}", hull.error());
  }
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

    GrahamScanResult hull = ParseAndDrawHull(randomPoints, seedStr);
    ASSERT_TRUE(hull.has_value());
    bool inv1 = TestConvexHullInvariant1(randomPoints, hull.value());
    bool inv2 = TestConvexHullInvariant2(hull.value());
    EXPECT_FALSE( inv1 and inv2 );
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

    GrahamScanResult hull = ParseAndDrawHullCorrect(randomPoints, seedStr);
    ASSERT_TRUE(hull.has_value());
    bool inv1 = TestConvexHullInvariant1(randomPoints, hull.value());
    bool inv2 = TestConvexHullInvariant2(hull.value());
    EXPECT_TRUE( inv1 and inv2 );
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

    std::string seedStr = std::format("original-{:#x}", seed);

    ParseAndDrawHull(points, seedStr);

    seedStr = std::format("fixed-{:#x}", seed);

    GrahamScanResult hull = ParseAndDrawHullCorrect(points, seedStr);

    ASSERT_TRUE(hull.has_value());
    bool inv1 = TestConvexHullInvariant1(points, hull.value());
    bool inv2 = TestConvexHullInvariant2(hull.value());
    EXPECT_TRUE( inv1 and inv2 );
  }
}
