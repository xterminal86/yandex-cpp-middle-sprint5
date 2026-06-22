#include <gtest/gtest.h>
#include <print>
#include "geometry.hpp"

using namespace geometry;

TEST(GeometryTest, Point2D)
{
  Point2D p1(1, 1);
  Point2D p2(2, 2);
  Point2D p3(1, 1);
  Point2D p4(3, 2);

  const double sqrt2 = std::sqrt(2.0);
  const double sqrt5 = std::sqrt(5.0);

  EXPECT_TRUE(p1 < p2);

  // Triggers warning for some reason.
  // EXPECT_TRUE((p1 == p3));

  EXPECT_DOUBLE_EQ(3.0, (p1 + p2).x);
  EXPECT_DOUBLE_EQ(3.0, (p1 + p2).y);

  EXPECT_DOUBLE_EQ(-1.0, (p1 - p2).x);
  EXPECT_DOUBLE_EQ(-1.0, (p1 - p2).y);

  EXPECT_DOUBLE_EQ(3.0, (p1 * 3.0).x);
  EXPECT_DOUBLE_EQ(4.0, (p1 * 4.0).y);

  EXPECT_DOUBLE_EQ(0.5, (p1 / 2.0).x);
  EXPECT_DOUBLE_EQ(0.5, (p1 / 2.0).y);

  EXPECT_DOUBLE_EQ((p1.x * p2.x + p1.y * p2.y), p1.Dot(p2));
  EXPECT_DOUBLE_EQ((p1.x * p2.y - p1.y * p2.x), p1.Cross(p2));
  EXPECT_DOUBLE_EQ(sqrt2, p1.Length());
  EXPECT_DOUBLE_EQ(sqrt2, p1.DistanceTo(p2));
  EXPECT_DOUBLE_EQ(sqrt5, p1.DistanceTo(p4));
  EXPECT_DOUBLE_EQ(1.0 / sqrt2, p1.Normalize().x);
  EXPECT_DOUBLE_EQ(1.0 / sqrt2, p1.Normalize().y);
}

// =============================================================================

TEST(GeometryTest, Line)
{
  Point2D p(1, 2);

  Line l({ 0, 0 }, { 4, 4 });

  EXPECT_DOUBLE_EQ(2.0, l.Center().x);
  EXPECT_DOUBLE_EQ(2.0, l.Center().y);
  EXPECT_DOUBLE_EQ(1.0, p.DistanceTo(l.Center()));
  EXPECT_DOUBLE_EQ(4.0, l.Height());
}

// =============================================================================

TEST(GeometryTest, Triangle)
{
  Triangle t({ 0, 0 }, { 2, 0 }, { 1, 3 });

  EXPECT_DOUBLE_EQ(3.0, t.Area());
  EXPECT_DOUBLE_EQ(3.0, t.Height());
  EXPECT_DOUBLE_EQ(1.0, t.Center().x);
  EXPECT_DOUBLE_EQ(1.0, t.Center().y);
}

// =============================================================================

TEST(GeometryTest, Rectangle)
{
  Rectangle r({ 0, 0 }, 3, 2);

  EXPECT_DOUBLE_EQ(3.0, r.TopRight().x);
  EXPECT_DOUBLE_EQ(2.0, r.TopRight().y);
  EXPECT_DOUBLE_EQ(2.0, r.Height());
  EXPECT_DOUBLE_EQ(1.5, r.Center().x);
  EXPECT_DOUBLE_EQ(1.0, r.Center().y);
}

// =============================================================================

TEST(Distance, Test)
{
  Point2D p(10, 10);
  Circle c({ 0, 0 }, 1.5);

  double sqrt200 = std::sqrt(200.0);

  double d = p.DistanceTo(c.center_p);

  EXPECT_DOUBLE_EQ(sqrt200, d);
}
