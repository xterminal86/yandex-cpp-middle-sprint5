#include "convex_hull.hpp"
#include <algorithm>
#include <stdexcept>
#include <print>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2)
{
  Point2D new_p1 = p1 - middle;
  Point2D new_p2 = p2 - middle;
  return new_p1.Cross(new_p2);
}

// =============================================================================

GrahamScanResult GrahamScan(std::span<Point2D> points) noexcept
{
  if (points.size() < 3)
  {
    return std::unexpected("At least three points are required for convex hull.");
  }

  auto smallest = *std::min_element(points.begin(), points.end());

  std::sort(
    points.begin() + 1,
    points.end(),
    [&smallest](const Point2D& p1, const Point2D& p2)
    {
      static const auto precision = 1e-10;

      double cross = CrossProduct(p1, smallest, p2);
      if (std::abs(cross) < precision)
      {
        return smallest.DistanceTo(p1) < smallest.DistanceTo(p2);
      }

      return cross > 0;
    }
  );

  StackForGrahamScan hull;
  for (const auto& new_p : points)
  {
    while (hull.Size() > 1
       && CrossProduct(hull.NextToTop(), hull.Top(), new_p) > 0.0)
    {
      hull.Pop();
    }

    hull.Push(new_p);
  }

  return std::vector{hull.Extract()};
}

// =============================================================================

double Cross2D(const Point2D& o, const Point2D& a, const Point2D& b)
{
  //
  // Let's recall "normal" cross product for a moment:
  //
  // x1 y1 z1 x1 y1 z1
  // x2 y2 z2 x2 y2 z2
  // rx ry rz rx ry rz
  //
  // To find the resulting vector (rx, ry, rz) perform multiplication and
  // subtraction using a "criss-cross" rule (hence the name) that is similar to
  // the one you use during finding of determinant of a 2x2 matrix - multiply
  // along the left to right downwards diagonal and subtract along the left to
  // right upwards diagonal for the components in question:
  //
  // rx = y1 * z2 - y2 * z1
  // ry = z1 * x2 - z2 * x1
  // rz = x1 * y2 - x2 * y1
  //
  // For 2D z is obviously 0, so you're left with just:
  //
  // (x1 * y2 - x2 * y1)
  //
  // which can be interpreted as a vector pointing inside / outside the screen,
  // if you like.
  //
  // We're dealing with situation like this:
  //
  //     b
  //        _.a
  //        /|
  //      /
  //    /
  //  o
  //
  // Direction of difference vector is always TOWARDS the minuend, so in this
  // case it should be (a - o). Then by finding Cross2D of (a - o) and (b - o)
  // we can determine whether we're doing left turn or not.
  //
  return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

//
// To avoid square root calculation.
//
double DistanceSquared(const Point2D& p1, const Point2D& p2)
{
  return std::pow((p1.x - p2.x), 2.0) +
         std::pow((p1.y - p2.y), 2.0);
}

//
// 1. Find lowest point.
// 2. Sort all points by polar angle around pivot.
// 3. Iterate through sorted points keeping only left turns (see above).
//
GrahamScanResult GrahamScanCorrect(const std::vector<Point2D>& points)
{
  GrahamScanResult res;

  if (points.size() <= 3)
  {
    return std::unexpected(
      std::format("Need at least 3 points to form convex hull! "
                  "Got {}.",points.size()
      )
    );
    return res;
  }

  double epsilon = std::numeric_limits<double>::epsilon();

  // 1. Find the lowest point.
  Point2D pivot = Point2D::Invalid();
  for (auto& p : points)
  {
    bool equalY = std::abs(p.y - pivot.y) < epsilon;
    if ( (p.y < pivot.y) or (equalY and (p.x < pivot.x)) )
    {
      pivot = p;
    }
  }

  //std::println("pivot = ({:.2f}, {:.2f})", pivot.x, pivot.y);

  // 2. Sort points by polar angle around pivot.
  std::vector<Point2D> sorted = points;
  std::sort(
    sorted.begin(),
    sorted.end(),
    [&pivot, &epsilon](const Point2D& p1, const Point2D& p2)
    {
      double cross = Cross2D(pivot, p1, p2);

      //std::println("  p1 = ({:.2f} {:.2f}), p2 = ({:.2f}, {:.2f})",
      //             p1.x, p1.y, p2.x, p2.y);
      //std::println("  cross = {:.2f}", cross);

      // Collinear - closes point comes first.
      if (std::abs(cross) < epsilon)
      {
        double d1 = DistanceSquared(pivot, p1);
        double d2 = DistanceSquared(pivot, p2);

        //std::println("    d1 = {:.2f}", d1);
        //std::println("    d2 = {:.2f}", d2);

        return (d1 < d2);
      }
      // Left turn.
      else if (cross > 0.0)
      {
        return true;
      }

      // RIght turn.
      return false;
    }
  );

  // Remove duplicates and keep only the farthest collinear point.
  std::vector<Point2D> uniques;
  for (const Point2D& p : sorted)
  {
    while (uniques.size() >= 2
       and std::abs(Cross2D(pivot, uniques[ uniques.size() - 1 ], p)) < epsilon)
    {
      uniques.pop_back();
    }

    uniques.push_back(p);
  }

  if (uniques.size() < 3)
  {
    return std::unexpected("Failed to form convex hull given these points! "
                           "Not enough points for a polygon.");
  }

  StackForGrahamScan hull;
  for (const Point2D& p : uniques)
  {
    // While last 3 points make a non-left turn or collinear.
    while (
      hull.Size() >= 2 and (
        (Cross2D(hull.NextToTop(), hull.Top(), p) < 0.0)
     or (std::abs(Cross2D(hull.NextToTop(), hull.Top(), p)) < epsilon)
        )
      )
    {
      hull.Pop();
    }

    hull.Push(p);
  }

  res = hull.Points();

  return res;
}

}  // namespace geometry::convex_hull
