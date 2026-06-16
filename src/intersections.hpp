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
    bool operator()(const Line& l1, const Line& l2)
    {

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
      // Set up the system:
      //
      // l1.sx + t*d1.x = l2.sx + u*d2.x
      // l1.sy + t*d1.y = l2.sy + u*d2.y
      //
      // Solve the system against t and u using Cramer's method.
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
      //
      return true;
    }

    bool operator()(const Line& l, const Circle& c)
    {
      Point2D v1 = l.start - c.center_p;
      Point2D v2 = l.end - l.start;

      double area = v1.Cross(v2);

      double perpendicular = area / v2.Length();

      return perpendicular < c.radius;
    }

    bool operator()(const Circle& c, const Line& l)
    {
      return operator()(l, c);
    }

    bool operator()(const Circle& c1, const Circle& c2)
    {
      double distanceVector = (c1.center_p - c2.center_p).Length();
      double radSum         = c1.radius + c2.radius;

      return (distanceVector < radSum);
    }

    template <typename T, typename U>
    bool operator()(const T& a, const U& b)
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
  return std::nullopt;
}

}  // namespace geometry::intersections
