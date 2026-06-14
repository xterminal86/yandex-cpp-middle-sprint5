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

// Столько неймспейсов только мешают.
// Надо было засунуть всё в один, максимум два.
using namespace geometry;
using namespace geometry::convex_hull;
using namespace geometry::triangulation;
using namespace geometry::visualization;

namespace rng = std::ranges;
namespace views = std::ranges::views;

void PrintAllIntersections(const Shape& shape, std::span<const Shape> others)
{
  std::println("\n=== Intersections ===");

  /*
    * Используйте ranges чтобы оставить только фигуры,
    * поддерживающие возможность находить пересечения между собой
    *
    * Затем примените монадический интерфейс для обработки результатов:
    *     - Пересечение найдено в точке A между фигурами B и C
    *     - Фигуры B и C не пересекаются
  */
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
}

// =============================================================================

void PerformShapeAnalysis(std::span<const Shape> shapes)
{
  std::println("\n=== Shape Analysis ===");

  /*
    * Используйте ranges и созданные классы чтобы:
    *     - Найти все пересечения между фигурами используя метод Bounding Box
    *     - Найти самую высокую фигуру (чья высота наибольшая)expected
    *     - Вывести расстояние между любыми двумя фигурами, которые поддерживают
    *       данную функциональность
  */
}

// =============================================================================

void PerformExtraShapeAnalysis(std::span<const Shape> shapes)
{
  std::println("\n=== Shape Extra Analysis ===");

  /*
    * Используйте ranges и созданные классы чтобы:
    *     - Вывести 3 любые фигуры, которые находятся выше 50.0
    *     - Вывести фигуры с наименьшей и с наибольшей высотами
  */
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

  uint64_t index = 0;

  // Выведите индекс каждой фигуры и её высоту
  for (const Shape& shape : shapes)
  {
    shape.visit(
      [&index](const auto& s)
      {
        if constexpr (
          std::is_same_v<std::remove_cvref_t<decltype(s)>, std::monostate>
        )
        {
          std::println("{}. shape is std::monostate!", index);
        }
        else
        {
          std::println("{}. height = {}", index, s.Height());

          // Это говно всё равно не работает.
          //std::println("{}", s);

          for (const Point2D& p : s.Vertices())
          {
            std::println("  ({}, {})", p.x, p.y);
          }
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
  geometry::visualization::Draw(shapes);

  //
  // Формируем список из вершин всех фигур
  //
  std::vector<Point2D> points;

  for (const Shape& shape : shapes)
  {
    shape.visit(
      [&points](const auto& s)
      {
        if constexpr (
          std::is_same_v<std::remove_cvref_t<decltype(s)>, std::monostate>
        )
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

    geometry::visualization::Draw(shapes);
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
