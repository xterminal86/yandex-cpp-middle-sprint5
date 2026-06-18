#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>
#include <print>

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
      // 3. Solve the system against 't' and 'u' using Cramer's Rule
      //
      // t*d1.x - u*d2.x = l2.sx - l1.sx
      // t*d1.y - u*d2.y = l2.sy - l1.sy
      //
      // Main determinant (D):
      //
      // D = (d1.x * -d2.y) - (d1.y * -d2.x)
      //
      // Auxillary determinants (swap unknown columns with free coefficients
      // column):
      //
      // Nt = (l2.sx - l1.sx) * -d2.y - (l2.sy - l1.sy) * -d2.x
      // Nu = d1.x * (l2.sy - l1.sy) - ( d1.y * (l2.sx - l1.sx) )
      //
      // t = Nt / D
      // u = Nu / D
      //
      // If D == 0 then line segments don't intersect (they're parallel or
      // collinear). For line segments intersection to occur 't' and 'u' must
      // fall between 0 and 1.
      //
      // Intersection point can then be found like so:
      //
      // intersection_x = l1.start.x + t * (l1.end.x - l1.start.x)
      // intersection_y = l1.start.y + t * (l1.end.y - l1.start.y)
      //

      Point2D d1 = l1.end - l1.start;
      Point2D d2 = l2.end - l2.start;

      //std::println("  d1 = ({}, {})", d1.x, d1.y);
      //std::println("  d2 = ({}, {})", d2.x, d2.y);

      double D = d1.x * -d2.y - d1.y * -d2.x;

      //std::println("  D = {}", D);

      double epsilon = std::numeric_limits<double>::epsilon();
      if (std::abs(D) < epsilon)
      {
        return std::nullopt;
      }

      double Nt = (l2.start.x - l1.start.x) * -d2.y -
                  (l2.start.y - l1.start.y) * -d2.x;
      double Nu = d1.x * (l2.start.y - l1.start.y) -
                  d1.y * (l2.start.x - l1.start.x);

      //std::println("  Nt = {}", Nt);
      //std::println("  Nu = {}", Nu);

      double t = Nt / D;
      double u = Nu / D;

      //std::println("  t = {}", t);
      //std::println("  u = {}", u);

      if ( (t > 0.0 and t < 1.0) and (u > 0.0 and u < 1.0) )
      {
        Point2D intersectionPoint =
        {
          { l1.start.x + t * (l1.end.x - l1.start.x) },
          { l1.start.y + t * (l1.end.y - l1.start.y) }
        };

        //std::println("  lines intersect at ({:4f}, {:4f})",
        //             intersectionPoint.x, intersectionPoint.y);

        return intersectionPoint;
      }

      return std::nullopt;
    }

    // =========================================================================

    std::optional<Point2D> operator()(const Line& l, const Circle& c)
    {
      Point2D lineVector = l.end - l.start;

      //
      // We must subtract line start from circle's center. Look below for
      // details on why. Basically it's because math says so (look for (A - C)).
      //
      Point2D circleToLine = l.start - c.center_p;

      //
      // Recall circle equation:
      //
      // x^2 + y^2 = r^2
      //
      // For circle at point (px, py) this will become:
      //
      // (x - px)^2 + (y - py)^2 = r^2
      //
      // Parametric equation of an infinite line:
      //
      // P(t) = A + t * d
      //
      // Where:
      //
      // A = l.start
      // d = (l.end - l.start)
      //
      // To get an intersection point we need to find value of 't' for the line
      // that gives a point for which length of a difference vector between
      // circle's center point and this point on a line equals to circle's
      // radius (since for any point where line crosses the circle we can draw
      // a line from circle's center to this point and it will be the radius of
      // the circle).
      //
      // So we need to find point P on the line:
      //
      // P = A + t * d
      //
      // for which:
      //
      // |(P - C)| = r
      //
      // Plug in line parametric equation for P:
      //
      // |(A + t * d) - C| = r
      //
      // Our goal is to find t.
      //
      // We can square all that to leverage the fact that for any vector:
      //
      // |v|^2 = v * v (dot product)
      //
      // (A + t * d - C)^2 = r^2
      //
      // We can rearrange the stuff inside the braces:
      //
      // (A - C + t * d)^2 = r^2
      //
      // Note that (A - C) -> circleToLine (let's call it c2l for short) and
      // d -> lineVector:
      //
      // (c2l + t * d)^2 = r^2
      //
      // Open up the braces:
      //
      // c2l^2 + 2 * c2l * t * d + (t * d) ^ 2       = r^2
      // c2l * c2l + 2 * c2l * t * d + (t*d) * (t*d) = r^2
      // c2l * c2l + 2 * c2l * t * d + t*t * d*d     = r^2
      //
      // Rearrange terms:
      //
      // (d*d)*t^2 + 2 * c2l * t * d + (c2l^2 - r^2) = 0
      //
      // If we alias some variables this will start to look familiar:
      //
      // (d*d)         = A
      // (2 * c2l * d) = B
      // (c2l^2 - r^2) = C
      //
      // At^2 + Bt + C = 0
      //
      // We have a quadratic equation!
      //
      // So from now on it's high school math.
      //
      // D < 0 means no intersection.
      //
      // Also we must remember that we're dealing with line segment here,
      // so after / if we found the solution, we must check that 't' falls into
      // [0, 1] range. If it doesn't then there's no intersection as well
      // (meaning line lies somewhere "outside" the circle).
      //
      // =======================================================================
      // PRO TIP:
      //
      // Given circle and line equations:
      //
      // x^2 + y^2 = r^2
      // y = kx + b
      //
      // to check if they intersect you can solve the system by just plugging
      // a line equation in directly into circle equation - that way you'll be
      // trying to find a point on a circle that is the same as the point on the
      // line:
      //
      // x^2 + (kx + b)^2 = r^2
      // x^2 + (kx)^2 + 2*kx*b + b^2 - r^2 = 0
      // x^2 + (k^2)*(x^2) + 2*kx*b + b^2 - r^2 = 0
      // (k^2)*(2x^2) + 2kxb + (b^2 - r^2) = 0
      //
      // and try to solve quadratic equation. Each solution gives an
      // intersection point.
      // =======================================================================
      //

      // A = (d*d)
      double A = lineVector.Dot(lineVector);
      //std::println("A = {:4f}", A);

      // B = (2 * c2l * d)
      double B = 2 * circleToLine.Dot(lineVector);
      //std::println("B = {:4f}", B);

      // C = (c2l^2 - r^2)
      double C = circleToLine.Dot(circleToLine) - (c.radius * c.radius);
      //std::println("C = {:4f}", C);

      double D = B*B - 4*A*C;
      //std::println("D = {:4f}", D);

      if (D < 0.0)
      {
        return std::nullopt;
      }

      double sqrtD = std::sqrt(D);

      double t1 = (-B - sqrtD) / (2 * A);
      double t2 = (-B + sqrtD) / (2 * A);

      //std::println("t1 = {:4f}", t1);
      //std::println("t2 = {:4f}", t2);

      std::vector<Point2D> intersectionPoints;

      auto _checker = [&intersectionPoints, &l, &lineVector](double t)
      {
        if (t > 0.0 and t < 1.0)
        {
          intersectionPoints.push_back(
            {
              l.start.x + t * lineVector.x,
              l.start.y + t * lineVector.y
            }
          );

          //std::println("Found intersection point: ({:4f}, {:4f})",
          //             intersectionPoints.back().x,
          //             intersectionPoints.back().y);
        }
      };

      _checker(t1);
      _checker(t2);

      if (intersectionPoints.empty())
      {
        return std::nullopt;
      }

      //
      // We have a bit of a problem though: this method's signature requires to
      // return only 1 point, so let's pick first.
      //
      return intersectionPoints[0];
    }

    // =========================================================================

    std::optional<Point2D> operator()(const Circle& c, const Line& l)
    {
      return operator()(l, c);
    }

    // =========================================================================

    std::optional<Point2D> operator()(const Circle& c1, const Circle& c2)
    {
      //
      // Circle to circle collison is trivial - collision occurs if distance
      // between circle centers is less than sum of their radiuses.
      //
      // Special cases are:
      //
      // 1. One circle is completely inside another - sum of radiuses will be
      //    less than distance.
      //
      // 2. Circles are identical - distance is 0 and difference between
      //    radiuses is also 0.
      //
      double distance = (c1.center_p - c2.center_p).Length();
      double radSum   = c1.radius + c2.radius;

      //std::println("d = {:4f}", distance);
      //std::println("radSum = {:4f}", radSum);

      double epsilon = std::numeric_limits<double>::epsilon();

      bool tooFarApart       = (distance > radSum);
      bool oneInsideTheOther = (distance < std::abs(c1.radius - c2.radius));
      bool identical = (
            (std::abs(c1.radius - c2.radius) < epsilon)
        and (std::abs(distance) < epsilon)
      );

      //std::println("tooFarApart? {}", tooFarApart ? "Y" : "N");
      //std::println("oneInsideTheOther? {}", oneInsideTheOther ? "Y" : "N");
      //std::println("identical? {}", identical ? "Y" : "N");

      // No collision.
      if (tooFarApart or oneInsideTheOther or identical)
      {
        return std::nullopt;
      }

      //
      // Now to find actual collision point(s).
      //
      // First, place 1-st circle at (0, 0), and 2-nd at the (distance, 0):
      //
      // (obviously not to scale)
      //
      //             q - collision point 1
      //            /|
      //       r1 /  |
      //        /    | h
      //      /      |
      //    /    a   |
      // C1----------z----C2
      //  ^          |    ^
      //  |----------|----|
      //          d  |
      //             |
      //             |
      //             w - collision point 2
      //
      // We need to find point z (a, 0). From there we just add / subtract h to
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
      // After which we can plug 'a' into here to find 'h':
      //
      // h = sqrt(r1^2 - a^2)
      //
      // And intersection points will be:
      //
      // p1 = (a,  h)
      // p2 = (a, -h)
      //
      double dx = (c2.center_p.x - c1.center_p.x);
      double dy = (c2.center_p.y - c1.center_p.y);

      double r1sq = c1.radius * c1.radius;
      double r2sq = c2.radius * c2.radius;

      double dsq = distance * distance;

      double a = (r1sq - r2sq + dsq) / (2 * distance);
      double h = std::sqrt(r1sq - a*a);

      //std::println("a = {:4f}", a);
      //std::println("h = {:4f}", h);

      // Tangential - one point.
      bool tangentialExternal =
        (std::abs(distance - (c1.radius + c2.radius)) < epsilon);
      bool tangentialInternal =
        (std::abs(distance - std::abs(c1.radius - c2.radius)) < epsilon);

      Point2D p;

      if (tangentialExternal or tangentialInternal)
      {
        // Advance along the line connecting two circles in X axis direction.
        double x = c1.center_p.x + (a * dx) / distance;

        // Advance along the line connecting two circles in Y axis direction.
        double y = c1.center_p.y + (a * dy) / distance;

        p.x = x;
        p.y = y;

        //std::println("collision point = ({}, {})", p.x, p.y);

        // Arrived at the collision point.
        return p;
      }

      // Two points otherwise.
      double x1 = c1.center_p.x + (a * dx - h * dy) / distance;
      double y1 = c1.center_p.y + (a * dy + h * dx) / distance;

      double x2 = c1.center_p.x + (a * dx + h * dy) / distance;
      double y2 = c1.center_p.y + (a * dy - h * dx) / distance;

      // But our method's signature demands one, so...
      p.x = x1;
      p.y = y1;

      //std::println("collision point = ({}, {})", p.x, p.y);

      return p;
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
