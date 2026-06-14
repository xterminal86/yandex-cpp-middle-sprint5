#include "visualization.hpp"
#include "geometry.hpp"

#include <matplot/matplot.h>
#include <print>
#include <variant>

template <class... Ts>
struct Multilambda : Ts... {
  using Ts::operator()...;
};

namespace geometry::visualization {

matplot::figure_handle DrawConfig()
{
  using namespace geometry;
  using namespace matplot;

  // Disable gnuplot warnings
  figure_handle f = figure(false);
  f->backend()->run_command("unset warnings");
  f->ioff();
  f->size(900, 900);

  hold(on);     // Multiple plots mode
  axis(equal);  // Squre view
  grid(on);     // Enable grid by default

  return f;
}

void Draw(std::span<geometry::Shape> shapes)
{
  static uint64_t imageInd = 1;

  using namespace geometry;
  using namespace matplot;

  auto _plot_fig =
  []<size_t N>(const Lines2D<N>& lines, const std::string& color)
  {
    line_handle h = plot(lines.x, lines.y);
    h->line_width(2);
    h->color(color);
  };

  auto _plot_poly = [](const Lines2DDyn& lines, const std::string& color)
  {
    line_handle h = plot(lines.x, lines.y);
    h->line_width(2);
    h->color(color);
  };

  const figure_handle& fh = DrawConfig();
  for (const auto &[index, shape] : std::ranges::views::enumerate(shapes))
  {
    /**
    * @brief Для каждой фигуры примените `std::visit` с помощью мульти-лямбды
    * (Multilambda), которая обрабатывает каждый возможный тип фигуры отдельно.
    *    Внутри каждой лямбды:
    *    - Вызовите метод `.Lines()` у фигуры — он возвращает структуру с двумя
    *      векторами:
    *      `.x` и `.y`, содержащими координаты точек для отрисовки.
    *    - Передайте эти координаты в функцию `plot(lines.x, lines.y)`.
    *    - Настройте внешний вид линии: установите толщину `.line_width(2)` и
    *      задайте цвет `.color()`:
    *        • Line      → "yellow"
    *        • Triangle  → "blue"
    *        • Rectangle → "green"
    *        • RegularPolygon → "magenta"
    *        • Circle    → "red"
    *        • Polygon   → "cyan"
    *
    */

    std::visit(
      Multilambda
      {
        [](const std::monostate& s)
        {
          std::println("{}:{} - shape is std::monostate!", __FILE__, __LINE__);
        }
        , [&_plot_fig](const Line& fig)            { _plot_fig(fig.Lines(),  "yellow");  }
        , [&_plot_fig](const Triangle& fig)        { _plot_fig(fig.Lines(),  "blue");    }
        , [&_plot_fig](const Rectangle& fig)       { _plot_fig(fig.Lines(),  "green");   }
        , [&_plot_poly](const RegularPolygon& fig) { _plot_poly(fig.Lines(), "magenta"); }
        , [&_plot_poly](const Circle& fig)         { _plot_poly(fig.Lines(), "red");     }
        , [&_plot_poly](const Polygon& fig)        { _plot_poly(fig.Lines(), "cyan");    }
      },
      shape
    );

    // Add shape number
    const Point2D center = shape.visit(
      [](auto &&s)
      {
        if constexpr (not std::is_same_v<
          std::remove_cvref_t<decltype(s)>, std::monostate
        >)
        {
          return s.Center();
        }

        std::println("Variant holds no valid type - "
                     "returning Point2D::Invalid()");
        return Point2D::Invalid();
      }
    );

    if (center != Point2D::Invalid())
    {
      auto t = text(center.x, center.y, std::to_string(index));
      t->font_size(14);
      t->color("black");
    }
  }

  // Display plot
  fh->save(std::format("plot_{}.png", imageInd++));
}

void Draw(std::span<const geometry::triangulation::DelaunayTriangle> triangles)
{
  static uint64_t imageInd = 1;

  using namespace geometry;
  using namespace matplot;

  const figure_handle& fh = DrawConfig();

  for (const auto &[index, d_triangle] : std::ranges::views::enumerate(triangles))
  {
    const geometry::Triangle tri{d_triangle.a, d_triangle.b, d_triangle.c};
    const Lines2D<4> lines = tri.Lines();
    plot(lines.x, lines.y)->line_width(2).color("cyan");

    // Add triangle number
    const Point2D center = tri.Center();
    auto t = text(center.x, center.y, std::to_string(index));
    t->font_size(14);
    t->color("black");
  }

  // Display plot
  fh->save(std::format("delaunay_{}.png", imageInd++));
}

}  // namespace geometry::visualization
