#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>

namespace geometry::intersections {

/*
* Класс для поиска пересечений между двумя фигурами
*
* Требуется организовать возможность нахождения пересечений только для следующих
* комбинаций фигур:
*    - Line   & Line
*    - Line   & Circle
*    - Circle & Circle
*
* Для всех остальных требуется выбросить исключение std::logic_error
*/
class IntersectionVisitor
{
  public:
    std::optional<Point2D> operator()(const Line& l1, const Line& l2)
    {
      //
      // 1. Using parametric line equations:
      //
      // d1 = l1.end - l1.start
      // d2 = l2.end - l2.start
      //
      // l1.start + t*d1
      // l2.start + u*d2
      //
      // define line intersection:
      //
      // l1.start + t*d1 = l2.start + u*d2
      //
      // 2. Set up the system:
      //
      // l1.sx + t*d1.x = l2.sx + u*d2.x
      // l1.sy + t*d1.y = l2.sy + u*d2.y
      //
      // 3. Solve the system against t and u using Cramer's Rule.
      //
      // t*d1.x - u*d2.x = l2.sx - l1.sx
      // t*d1.y - u*d2.y = l2.sy - l1.sy
      //
      // Main determinant (D):
      //
      // D = (d1.x * d2.y) - (d1.y * d2.x)
      //
      // Auxillary determinants:
      //
      // Nt = d2.x * (l2.sy - l1.sy) - ( d2.y * (l2.sx - l1.sx) )
      // Nu = d1.x * (l2.sy - l1.sy) - ( d1.y * (l2.sx - l1.sx) )
      //
      // t = Nt / D
      // u = Nu / D
      //
      // If D == 0 then lines don't intersect (they're parallel or collinear).
      // For intersection to occur t and u must fall between 0 and 1.
      //
      // Contact point can then be found like so:
      //
      // intersection_x = l1.start.x + t * (l1.end.x - l1.start.x)
      // intersection_y = l1.start.y + t * (l1.end.y - l1.start.y)
      //

      Point2D d1 = l1.end - l1.start;
      Point2D d2 = l2.end - l2.start;

      double D = d1.x * d2.y - d1.y * d2.x;

      double epsilon = std::numeric_limits<double>::epsilon();
      if (D < epsilon)
      {
        return std::nullopt;
      }

      double Nt = d2.x * (l2.start.y - l1.start.y) -
                  d2.y * (l2.start.x - l1.start.x);
      double Nu = d1.x * (l2.start.y - l1.start.y) -
                  d1.y * (l2.start.x - l1.start.x);

      double t = Nt / D;
      double u = Nu / D;

      if ( (t > 0.0 and t < 1.0) and (u > 0.0 and u < 1.0) )
      {
        return Point2D
        {
          { l1.start.x + t * (l1.end.x - l1.start.x) },
          { l1.start.y + t * (l1.end.y - l1.start.y) }
        };
      }

      return std::nullopt;
    }

    std::optional<Point2D> operator()(const Line& l, const Circle& c)
    {
      Point2D lineVector   = l.end - l.start;
      Point2D circleToLine = c.center_p - l.start;

      //
      // Divide projection on lineVector by its length (since lineVector can be
      // interpreted as a vector originating from (0,0) ) to get a "percentage"
      // of projection's length compared to whole line (lineVector) length.
      // This variant uses squared magnitude to avoid square root computation.
      //
      // Step by step explanation (d is lineVector, A -> l.start,
      // C - c.center_p):
      //
      // 1. Our condition that we're getting at (find a point on the line so
      //    that dot product of vector from that point to circle's center and
      //    line vector itself gives 0, since that's the marker of
      //    perpendicularity):
      //
      // (C - (A + t*d)) * d = 0
      //
      // 2. Solve for t:
      //
      // (C - A - t*d) * d = 0
      // C*d - A*d - t*(d * d) = 0
      // (C - A) * d - t*(d * d) = 0
      // t*(d * d) = (C - A) * d
      // t = (C - A) * d / (d * d)
      //
      // More intuitive approach would be to normalize d first, which will leave
      // just a dot product for calculating a projection, but this would involve
      // calculating square root first to normalize d.
      //
      double percentage =
        circleToLine.Dot(lineVector) / lineVector.Dot(lineVector);

      //
      // Advance line's starting point by percentage along itself to get to the
      // perpendicular point.
      //
      Point2D closestToCircle =
        {
          { l.start.x + percentage * lineVector.x },
          { l.start.y + percentage * lineVector.y }
        };

      //
      // Find distance from perpendicular point to circle's center.
      //
      double circleToLineDist = (closestToCircle - c.center_p).Length();

      // If it's greater than radius - no intersection.
      if (circleToLineDist > c.radius)
      {
        return std::nullopt;
      }

      // Tangential intersection - one point (i.e. closestToCircle).
      double epsilon = std::numeric_limits<double>::epsilon();
      if (std::abs(circleToLineDist - c.radius) < epsilon)
      {
        return closestToCircle;
      }

      //
      // "Normal" intersection - two points. In this case closestToCircle
      // point will divide line segment inside the circle in two equal parts,
      // which we can use to find out intersection points by leveraging
      // Pythagoras' theorem: distance from circle's center to an intersection
      // point in this case will be equal to circle's radius (and that's a
      // hypotenuse by the way), and perpendicular we already have
      // (circleToLineDist). So, all that's left is to subtract hypotenuse from
      // the leg:
      //
      double offset =
        std::sqrt(c.radius * c.radius - circleToLineDist * circleToLineDist);

      //
      // And now we can add / subtract this offset from perpendicular point to
      // get intersection points.
      //
      double t1 = (percentage - offset) / lineVector.Length();
      double t2 = (percentage + offset) / lineVector.Length();

      Point2D p1 =
        {
          { l.start.x + t1 * lineVector.x },
          { l.start.y + t1 * lineVector.y },
        };

      Point2D p2 =
        {
          { l.start.x + t2 * lineVector.x },
          { l.start.y + t2 * lineVector.y },
        };

      //
      // We have a bit of a problem though: this method's signature requires to
      // return only 1 point, so let's pick any of the two.
      //
      return p1;
    }

    std::optional<Point2D> operator()(const Circle& c, const Line& l)
    {
      return operator()(l, c);
    }

    std::optional<Point2D> operator()(const Circle& c1, const Circle& c2)
    {
      double distance = (c1.center_p - c2.center_p).Length();
      double radSum   = c1.radius + c2.radius;

      double dx = (c2.center_p.x - c1.center_p.x);
      double dy = (c2.center_p.y - c1.center_p.y);

      double epsilon = std::numeric_limits<double>::epsilon();

      bool tooFarApart = (distance > radSum);
      bool oneInsideTheOther = (distance < std::abs(c1.radius - c2.radius));
      bool identical = (std::abs(c1.radius - c2.radius) < epsilon);

      // No collision.
      if (tooFarApart or oneInsideTheOther or identical)
      {
        return std::nullopt;
      }

      //
      // First, place 1st circle at (0, 0), and 2nd at the (distance, 0):
      //
      // (obviously not to scale)
      //
      //             x - collision point 1
      //            /|
      //       r1 /  |
      //        /    | h
      //      /      |
      //    /    a   |
      // C1----------O----C2
      //  |               |
      //   ---------------
      //          d
      //
      //
      //             x - collision point 2
      //
      // We need to find point O (a, h). From there we just add / subtract h to
      // get intersection points.
      //
      // For C1:
      //
      // a^2 + h^2 = r1^2        (1)
      //
      // For C2:
      //
      // (d - a)^2 + h^2 = r2^2  (2)
      //
      // Set up the system:
      //
      // r1^2 - a^2       = h^2  (1)
      // r2^2 - (d - a)^2 = h^2  (2)
      //
      // Solve shit:
      //
      // r2^2 - (d - a)^2         = r1^2 - a^2
      // r2^2 - (d^2 - 2da + a^2) = r1^2 - a^2
      // r2^2 -  d^2 + 2da - a^2  = r1^2 - a^2
      // r2^2 -  d^2 + 2da        = r1^2
      //
      //     (r1^2 - r2^2 + d^2)
      // a = -------------------
      //             2d
      //
      // After which we can plug a into here to find h:
      //
      // h = sqrt(r1^2 - a^2)
      //
      // And intersection points will be:
      //
      // p1 = (a,  h)
      // p2 = (a, -h)
      //
      double r1sq = c1.radius * c1.radius;
      double r2sq = c2.radius * c2.radius;

      double dsq = distance * distance;

      double a = (r1sq - r2sq + dsq) / (2 * distance);
      double h = std::sqrt(r1sq - a*a);

      // Tangential - one point.
      bool tangentialExternal =
        (std::abs(distance - (c1.radius + c2.radius)) < epsilon);
      bool tangentialInternal =
        (std::abs(distance - std::abs(c1.radius - c2.radius)) < epsilon);

      if (tangentialExternal or tangentialInternal)
      {
        // Advance along the line connecting two circles in X axis direction.
        double x = c1.center_p.x + (a * dx) / distance;

        // Advance along the line connecting two circles in Y axis direction.
        double y = c1.center_p.y + (a * dy) / distance;

        // Arrived at collision point.
        return Point2D(x, y);
      }

      // Two points otherwise.
      double x1 = c1.center_p.x + (a * dx - h * dy) / distance;
      double y1 = c1.center_p.y + (a * dy + h * dx) / distance;

      double x2 = c1.center_p.x + (a * dx + h * dy) / distance;
      double y2 = c1.center_p.y + (a * dy - h * dx) / distance;

      // But our method's signature demands one, so...
      return Point2D(x1, y1);
    }

    template <typename T, typename U>
    std::optional<Point2D> operator()(const T& a, const U& b)
    {
      throw std::logic_error(
        std::format("IntersectionVisitor() incompatible types: '{}' and '{}'",
                    typeid(T).name(),
                    typeid(U).name())
      );
    }
};

inline std::optional<Point2D> GetIntersectPoint(const Shape& shape1,
                                                const Shape& shape2)
{
  return std::visit(IntersectionVisitor{}, shape1, shape2);
}

}  // namespace geometry::intersections
