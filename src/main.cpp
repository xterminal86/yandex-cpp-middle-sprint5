#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>
#include <random>

// Столько неймспейсов только мешают.
// Надо было засунуть всё в один, максимум два.
using namespace geometry;
using namespace geometry::queries;
using namespace geometry::convex_hull;
using namespace geometry::triangulation;
using namespace geometry::visualization;
using namespace geometry::intersections;

namespace rng = std::ranges;
namespace views = std::ranges::views;

void PrintAllIntersections(const Shape& lineOrCircle,
                           std::span<const Shape> others)
{
  std::println("\n=== Intersections ===");

  /*
  Используйте ranges чтобы оставить только фигуры,
  поддерживающие возможность находить пересечения между собой

  Затем примените монадический интерфейс для обработки результатов:
      - Пересечение найдено в точке A между фигурами B и C
      - Фигуры B и C не пересекаются
  */

  auto supported =
    others | std::views::filter(
    [](const Shape& s)
    {
      return std::holds_alternative<Line>(s)
          or std::holds_alternative<Circle>(s);
    }
  );

  std::string figType;
  std::string figData;

  if (std::holds_alternative<Line>(lineOrCircle))
  {
    figType = "Line";
    Line l = std::get<Line>(lineOrCircle);
    figData = std::format("({}, {}) - ({}, {})",
                          l.start.x, l.start.y, l.end.x, l.end.y);
  }
  else if (std::holds_alternative<Circle>(lineOrCircle))
  {
    figType = "Circle";
    Circle c = std::get<Circle>(lineOrCircle);
    figData = std::format("({}, {}), r = {}",
                          c.center_p.x, c.center_p.y, c.radius);
  }

  for (const Shape& s : supported)
  {
    try
    {
      std::string against;

      if (std::holds_alternative<Line>(s))   against = "Line";
      if (std::holds_alternative<Circle>(s)) against = "Circle";

      std::println("{} vs {}:", figType, against);

      if (std::holds_alternative<Line>(s))
      {
        Line l = std::get<Line>(s);
        std::println("  {} vs ({}, {}) - ({}, {})",
                     figData,
                     l.start.x, l.start.y, l.end.x, l.end.y);
      }
      else if (std::holds_alternative<Circle>(s))
      {
        Circle c = std::get<Circle>(s);
        std::println("  {} vs ({}, {}), r = {}",
                     figData,
                     c.center_p.x, c.center_p.y, c.radius);
      }

      std::optional<Point2D> ip = GetIntersectPoint(lineOrCircle, s);
      if (ip)
      {
        std::println("  contact point = ({:.4f}, {:.4f})",
                     ip.value().x,
                     ip.value().y);
      }
      else
      {
        std::println("  no intersection");
      }
    }
    catch(std::exception& e)
    {
      std::println("{}", e.what());
    }
  }
}

// =============================================================================

void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes)
{
  std::println("\n=== Distance from Point Test ===");

  /*
    * Используйте ranges чтобы выбрать любые 5 фигур из списка.
    * Затем найдите расстояния от заданной точки до всех выбранных фигур.
    * Выведите результат в формате "Расстояние от точки P до фигуры S равно D"
  */
  std::vector<Shape> random5;

  std::sample(
    shapes.cbegin(),
    shapes.cend(),
    std::back_inserter(random5),
    5,
    std::mt19937_64(std::random_device{}())
  );

  for (const Shape& s: random5)
  {
    s.visit(
      [](auto&& s)
      {
        std::println("  calculating distance to shape '{}'",
                     typeid(s).name());
      }
    );

    std::optional<double> d = std::visit(PointToShapeDistanceVisitor(p), s);
    if (d)
    {
      std::println("Distance from P to S = {:4f}", d.value());
    }
    else
    {
      std::println("Cannot find distance for this point and shape pair!");
    }
  }
}

// =============================================================================

void PerformShapeAnalysis(std::span<const Shape> shapes)
{
  using namespace geometry::utils;
  using namespace geometry::queries;

  std::println("\n=== Shape Analysis ===");

  /*
  Используйте ranges и созданные классы чтобы:
      - Найти все пересечения между фигурами используя метод Bounding Box
      - Найти самую высокую фигуру (чья высота наибольшая) expected
      - Вывести расстояние между любыми двумя фигурами, которые поддерживают
        данную функциональность
  */

  // --------------------------- Bounding Box  ---------------------------------

  for (size_t i = 0; i < shapes.size(); i++)
  {
    for (size_t j = i + 1; j < shapes.size(); j++)
    {
      if (BoundingBoxesOverlap(shapes[i], shapes[j]))
      {
        std::println("Overlap found for shape {} and shape {}", i, j);
      }
    }
  }

  std::println();

  // --------------------------- Height ----------------------------------------

  std::optional<size_t> res = FindHighestShape(shapes);
  if (res)
  {
    size_t index = *res;
    double h = GetHeight(shapes[index]);
    std::println("Max height -> shapes[{}] = {}", index, h);
  }
  else
  {
    std::println("Max height not found!");
  }

  std::println();

  // --------------------------- Distance --------------------------------------

  //
  // ShapeToShapeDistanceVisitor supports only Line and Circle.
  //
  auto lines = shapes | std::views::filter(
    [](const Shape& s)
    {
      return std::holds_alternative<Line>(s);
    }
  );

  auto lines_v = lines | std::ranges::to<std::vector>();

  auto circles = shapes | std::views::filter(
    [](const Shape& s)
    {
      return std::holds_alternative<Circle>(s);
    }
  );

  auto circles_v = circles | std::ranges::to<std::vector>();

  if (not lines_v.empty() and not circles_v.empty())
  {
    std::optional<double> d =
      std::visit(ShapeToShapeDistanceVisitor{}, lines_v[0], circles_v[0]);
    if (d)
    {
      std::println("Distance between two shapes = {:4f}", d.value());
    }
    else
    {
      std::println("Distance between two shapes is std::nullopt!");
    }
  }
  else
  {
    std::println("Couldn't find suitable shapes to get distance!");
  }
}

// =============================================================================

void PerformExtraShapeAnalysis(std::span<const Shape> shapes)
{
  std::println("\n=== Shape Extra Analysis ===");

  /*
  Используйте ranges и созданные классы чтобы:
      - Вывести 3 любые фигуры, которые находятся выше 50.0
      - Вывести фигуры с наименьшей и с наибольшей высотами
  */

  std::vector<Shape> higherThan50;
  for (const Shape& s : shapes)
  {
    double h = GetHeight(s);
    if (h > 50.0)
    {
      higherThan50.push_back(s);
    }
  }

  if (not higherThan50.empty())
  {
    std::vector<Shape> random3;
    std::sample(
      higherThan50.cbegin(),
      higherThan50.cend(),
      std::back_inserter(random3),
      3,
      std::mt19937_64(std::random_device{}())
    );

    for (const Shape& s : random3)
    {
      /*
      Point2D p = s.visit(
        [](const auto&& sh)
        {
          if constexpr (SupportedShape<decltype(sh)>)
          {
            return Point2D::Invalid();
          }
          else
          {
            return Point2D::Invalid();
          }
        }
      );

      std::println("Shape higher than 50 - ({}, {})", p.x, p.y);
      */
    }

  }



}

// =============================================================================

int main()
{
  std::vector<Shape> shapes = utils::ParseShapes(
    "circle 0 0 1.5; "
    "line 1 2 3 4; "
    "polygon 0 0 2 5; "
    "triangle 0 0 1 0 0.5 1; "
    "polygon 0 0 1 2; "
    "badshape; "
    "circle 0 0 -1"
  );

  std::println("Parsed {} shapes", shapes.size());

  //
  // Кастомный формат почему-то не работает, поэтому приходится всё делать через
  // жопу.
  //
  for (size_t i = 0; i < shapes.size(); i++)
  {
    const Shape& s = shapes[i];

    s.visit(
      [i](auto&& sh)
      {
        if constexpr (not EmptyVariant<decltype(sh)>)
        {
          std::println("{}. '{}'", i, typeid(sh).name());
        }
      }
    );
  }

  std::println();

  uint64_t index = 0;

  // Выведите индекс каждой фигуры и её высоту
  for (const Shape& shape : shapes)
  {
    shape.visit(
      [&index](const auto& s)
      {
        if constexpr (EmptyVariant<decltype(s)>)
        {
          std::println("{}. shape is std::monostate!", index);
        }
        else
        {
          std::println("{}. height = {}", index, s.Height());

          // Это говно всё равно не работает.
          //std::println("{}", s);
        }

        index++;
      }
    );
  }

  //
  // Вызываем разработанные функции
  //
  PrintAllIntersections(shapes[0], shapes);

  PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

  PerformShapeAnalysis(shapes);

  PerformExtraShapeAnalysis(shapes);

  //
  // Рисуем все фигуры
  //
  // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и
  // построить 2ой график
  //
  geometry::visualization::Draw(shapes, "main");

  //
  // Формируем список из вершин всех фигур
  //
  std::vector<Point2D> points;

  for (const Shape& shape : shapes)
  {
    shape.visit(
      [&points](const auto& s)
      {
        if constexpr (EmptyVariant<decltype(s)>)
        {
          std::println("{}:{} - shape is std::monostate!", __FILE__, __LINE__);
        }
        else
        {
          for (const Point2D& p : s.Vertices())
          {
            points.push_back(p);
          }
        }
      }
    );
  }

  //
  // Находим список точек, для построения выпуклой оболочки - convex hull -
  // алгоритмом Грэхема
  // Создаём из них объект класса `Polygon` и добавляем его в список shapes
  // Рисуем все фигуры
  //

  GrahamScanResult convexHull = GrahamScan(points);
  if (not convexHull)
  {
    std::println("{}:{} - convex hull construction failed for points:",
                 __FILE__, __LINE__);

    for (const Point2D& p : points)
    {
      std::println("  ({}, {})", p.x, p.y);
    }

    // Почему-то это говно не работает.
    //std::println("{}:{} - convex hull construction failed for points '{}'",
    //             __FILE__, __LINE__, points);

    // И так тоже.
    //for (const Point2D& p : points)
    //{
    //  std::println("  {}", p);
    //}
  }
  else
  {
    Polygon p(convexHull.value());
    shapes.push_back(p);

    geometry::visualization::Draw(shapes, "main");
  }

  //
  // после изучения графика - нажмите Enter чтобы продолжить выполнение и
  // построить 3ий график
  //
  {
    std::vector<Point2D> points =
    {
        {  0,  0 }
      , { 10,  0 }
      , {  5,  8 }
      , { 15,  5 }
      , {  2, 12 }
    };

    //
    // Используйте список точек points или свой, чтобы
    // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
    //
    // После успешного завершения алгоритма - выведите результат для проверки
    // используя geometry::visualization::Draw
    //
    DelaunayResult res = DelaunayTriangulation(points);
    if (res)
    {
      geometry::visualization::Draw(res.value());
    }
    else
    {
      std::println("{}:{} - triangulation failed for points:",
                   __FILE__, __LINE__);

      for (const Point2D& p : points)
      {
        std::println("  ({}, {})", p.x, p.y);
      }
    }
  }

  std::println("All done!\n");

  return 0;
}
