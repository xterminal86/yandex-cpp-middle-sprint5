#include <gtest/gtest.h>
#include "queries.hpp"
#include "geometry.hpp"
#include "shape_utils.hpp"

using namespace geometry;
using namespace geometry::queries;
using namespace geometry::utils;

TEST(QueriesTest, DistanceToPoint)
{
  std::vector<Shape> shapes = ParseShapes(
    "circle 0 0 1.5; "
    "line 0 3 3 3; "
  );

  Point2D p(2, 2);

  double d1 = DistanceToPoint(shapes[0], p);
  double sqrt8 = std::sqrt(8.0);

  EXPECT_DOUBLE_EQ((sqrt8 - 1.5), d1);

  double d2 = DistanceToPoint(shapes[1], p);
  EXPECT_DOUBLE_EQ(1.0, d2);
}

// =============================================================================

TEST(QueriesTest, GetHeight)
{
  std::vector<Shape> shapes = ParseShapes(
    "circle 0 0 1.5; "
    "line -1 2 3 2; "
  );

  EXPECT_DOUBLE_EQ(1.5, GetHeight(shapes[0]));
  EXPECT_DOUBLE_EQ(2.0, GetHeight(shapes[1]));
}

// =============================================================================

TEST(QueriesTest, BoundingBox)
{
  std::vector<Shape> shapes = ParseShapes(
    "rectangle 0 0 2 2; "
    "rectangle 1 1 2 2; "
    "rectangle 2 0 2 2; "
    "rectangle 5 5 1 1; "
    "triangle -1 0 1 0 0 1; "
  );

  EXPECT_TRUE(  BoundingBoxesOverlap(shapes[0], shapes[1]) );
  EXPECT_TRUE(  BoundingBoxesOverlap(shapes[0], shapes[2]) );
  EXPECT_FALSE( BoundingBoxesOverlap(shapes[0], shapes[3]) );

  BoundingBox bb = GetBoundBox(shapes[4]);
  EXPECT_DOUBLE_EQ(-1.0, bb.min_x);
  EXPECT_DOUBLE_EQ( 0.0, bb.min_y);
  EXPECT_DOUBLE_EQ( 1.0, bb.max_x);
  EXPECT_DOUBLE_EQ( 1.0, bb.max_y);
}

// =============================================================================

TEST(QueriesTest, ShapesDistance)
{
  std::vector<Shape> shapes = ParseShapes(
    "circle 0 0 1; "
    "circle 3 0 1; "
    "line -1 3 3 3; "
    "line -1 4 4 4; "
    "triangle -1 0 1 0 0 1; "
  );
  std::optional<double> d;

  d = DistanceBetweenShapes(shapes[0], shapes[1]);
  ASSERT_TRUE(d.has_value());
  EXPECT_DOUBLE_EQ(1.0, d.value());

  d = DistanceBetweenShapes(shapes[2], shapes[3]);
  ASSERT_TRUE(d.has_value());
  EXPECT_DOUBLE_EQ(1.0, d.value());

  d = DistanceBetweenShapes(shapes[0], shapes[3]);
  ASSERT_FALSE(d.has_value());
}
