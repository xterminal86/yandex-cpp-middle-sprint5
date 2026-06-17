#include <gtest/gtest.h>
#include "intersections.hpp"
#include "visualization.hpp"
#include "shape_utils.hpp"

using namespace geometry;
using namespace geometry::visualization;
using namespace geometry::intersections;

TEST(IntersectionsTest, LineVsLine)
{
  // Intersection at (0, 0)
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -1 -1 1 1; "
      "line 1 -1 -1 1"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_TRUE(ip.has_value());
    EXPECT_DOUBLE_EQ(ip->x, 0.0);
    EXPECT_DOUBLE_EQ(ip->y, 0.0);
  }
  // Intersection at (0.4, 0.8)
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -1 -2 1 2; "
      "line 2 0 -2 2"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_TRUE(ip.has_value());
    EXPECT_DOUBLE_EQ(ip->x, 0.4);
    EXPECT_DOUBLE_EQ(ip->y, 0.8);
  }
  // No collision - too far apart.
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -1 -2 0 0; "
      "line 2 0 -2 2"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_FALSE(ip.has_value());
  }
  // No collision - touching.
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -1 -2 0.4 0.8; "
      "line 2 0 -2 2"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_FALSE(ip.has_value());
  }
  // No collision - parallel.
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -1 -1 -1 2; "
      "line 1 -1 1 2"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_FALSE(ip.has_value());
  }
  // No collision - collinear.
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -1 -1 -1 2; "
      "line -1 0 -1 2"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_FALSE(ip.has_value());
  }
}

// =============================================================================

TEST(IntersectionsTest, LineVsCircle)
{
  // Intersection at (1, 0) and (0, -1)
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -0.5 -1.5 1.5 0.5; "
      "circle 0 0 1"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_TRUE(ip.has_value());
    EXPECT_DOUBLE_EQ(ip->x, 0.0);
    EXPECT_DOUBLE_EQ(ip->y, -1.0);
  }
  // Intersection at (1, 0) and (0.6, -0.8)
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line 0.5 -1 1.5 1; "
      "circle 0 0 1"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_TRUE(ip.has_value());
    EXPECT_DOUBLE_EQ(ip->x, 0.6);
    EXPECT_DOUBLE_EQ(ip->y, -0.8);
  }
  // Intersection at (0.8, 0.6) - segment starts inside the circle.
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line 0.2 0 1 0.8; "
      "circle 0 0 1"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_TRUE(ip.has_value());
    EXPECT_DOUBLE_EQ(ip->x, 0.8);
    EXPECT_DOUBLE_EQ(ip->y, 0.6);
  }
  // Tangential intersection at (1, 0).
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line 1 -1 1 1; "
      "circle 0 0 1"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_TRUE(ip.has_value());
    EXPECT_DOUBLE_EQ(ip->x, 1.0);
    EXPECT_DOUBLE_EQ(ip->y, 0.0);
  }
  // No intersection - apart.
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line 2 0 2 2; "
      "circle 0 0 1"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_FALSE(ip.has_value());
  }
  // No intersection - segment fully inside.
  {
    std::vector<Shape> shapes = utils::ParseShapes(
      "line -0.5 -0.5 0.5 0.5; "
      "circle 0 0 1"
    );

    Draw(shapes);

    std::optional<Point2D> ip;
    ASSERT_NO_THROW(ip = GetIntersectPoint(shapes[0], shapes[1]));
    ASSERT_FALSE(ip.has_value());
  }
}

// =============================================================================

TEST(IntersectionsTest, CircleVsCircle)
{
}
