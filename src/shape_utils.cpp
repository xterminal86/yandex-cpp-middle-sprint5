#include "shape_utils.hpp"
#include "queries.hpp"

#include <functional>
#include <print>

namespace geometry::utils {

// Разбивает строку на слова (по пробелам), игнорируя лишние пробелы
std::vector<std::string_view> SplitIntoWords(std::string_view s)
{
  std::vector<std::string_view> words;
  size_t start = 0;
  size_t end = 0;

  while (start < s.size())
  {
    // Пропускаем пробелы
    while (start < s.size()
        && std::isspace(static_cast<unsigned char>(s[start])))
    {
      ++start;
    }

    if (start >= s.size())
    {
      break;
    }

    end = start;
    while (end < s.size()
        && !std::isspace(static_cast<unsigned char>(s[end])))
    {
      ++end;
    }

    words.push_back(s.substr(start, end - start));
    start = end;
  }

  return words;
}

// =============================================================================

// Безопасный парсинг строки в double (без исключений)
std::optional<double> ParseDouble(std::string_view s)
{
  double value = 0.0;
  auto result = std::from_chars(s.data(), s.data() + s.size(), value);
  if (result.ec == std::errc{} && result.ptr == s.data() + s.size())
  {
    return value;
  }

  return std::nullopt;
}

// =============================================================================

// Парсит строку в вектор double
std::optional<std::vector<double>> ParseDoubles(std::string_view s)
{
  if (s.empty())
  {
    return std::nullopt;
  }

  auto tokens = SplitIntoWords(s);
  if (tokens.empty())
  {
    return std::nullopt;
  }

  std::vector<double> result;
  result.reserve(tokens.size());

  for (auto token : tokens)
  {
    auto num = ParseDouble(token);
    if (!num.has_value())
    {
      return std::nullopt;
    }

    result.push_back(*num);
  }

  return result;
}

// =============================================================================

// Проверяет размер вектора и возвращает его, если совпадает
std::optional<std::vector<double>>
RequireSize(const std::vector<double>& v, size_t expected)
{
  return (v.size() == expected) ? std::make_optional(v) : std::nullopt;
}

// =============================================================================

// Проверяет, что значение > 0
std::optional<double> RequirePositive(double x)
{
  return (x > 0) ? std::make_optional(x) : std::nullopt;
}

// =============================================================================

// Проверяет, что double представляет целое число >= min_value
std::optional<int> RequireIntegerAtLeast(double d, int min_value)
{
  int i = static_cast<int>(d);
  if (static_cast<double>(i) == d && i >= min_value) {
      return i;
  }
  return std::nullopt;
}

// =============================================================================

// Конструкторы фигур

/**
    @brief Создаёт круг из параметров
    @note Пример того как могла бы выглядеть эта функция:
        if (v.size() != 3) return std::nullopt;
        if (v[2] <= 0) return std::nullopt; // радиус должен быть > 0
        return Circle{{v[0], v[1]}, v[2]};
*/
std::optional<Shape> MakeCircle(const std::vector<double>& v)
{
  if (v.size() != 3)
  {
    std::println("{}:{} - need 3 values, got {}!",
                 __FILE__, __LINE__, v.size());
    return std::nullopt;
  }

  if (v[2] <= 0)
  {
    std::println("{}:{} - radius must be > 0!", __FILE__, __LINE__);
    return std::nullopt;
  }

  return Circle({ v[0], v[1] }, v[2]);
}

// =============================================================================

/**
    @brief Создаёт линию из параметров
    @note Пример того как могла бы выглядеть эта функция:
        if (v.size() != 4) return std::nullopt;
        return Line{{v[0], v[1]}, {v[2], v[3]}};
*/
std::optional<Shape> MakeLine(const std::vector<double>& v)
{
  if (v.size() != 4)
  {
    std::println("{}:{} - line needs 4 points, got {}!",
                 __FILE__, __LINE__, v.size());
    return std::nullopt;
  }

  return Line({ v[0], v[1] }, { v[2], v[3] });
}

// =============================================================================

/**
    @brief Создаёт треугольник из параметров
    @note Пример того как могла бы выглядеть эта функция:
        if (v.size() != 6) return std::nullopt;
        return Triangle{{v[0], v[1]}, {v[2], v[3]}, {v[4], v[5]}};
*/
std::optional<Shape> MakeTriangle(const std::vector<double>& v)
{
  if (v.size() != 6)
  {
    std::println("{}:{} - triangle needs 3 points, got {}!",
                 __FILE__, __LINE__, v.size());
    return std::nullopt;
  }

  return Triangle({ v[0], v[1] }, { v[2], v[3] }, { v[4], v[5] });
}

// =============================================================================

/**
    @brief Создаёт прямоугольник из параметров
    @note Пример того как могла бы выглядеть эта функция:
        if (v.size() != 4) return std::nullopt;
        if (v[2] <= 0 || v[3] <= 0) return std::nullopt; // ширина/высота > 0
        return Rectangle{{v[0], v[1]}, v[2], v[3]};
*/
std::optional<Shape> MakeRectangle(const std::vector<double>& v)
{
  if (v.size() != 4)
  {
    std::println("{}:{} - rectangle needs 4 points, got {}!",
                 __FILE__, __LINE__, v.size());
    return std::nullopt;
  }

  if (v[2] <= 0)
  {
    std::println("{}:{} - width must be > 0!", __FILE__, __LINE__);
    return std::nullopt;
  }

  if (v[3] <= 0)
  {
    std::println("{}:{} - height must be > 0!", __FILE__, __LINE__);
    return std::nullopt;
  }

  return Rectangle({ v[0], v[1] }, v[2], v[3]);
}

// =============================================================================

/**
    @brief Создаёт правильный многоугольник из параметров
    @note Пример того как могла бы выглядеть эта функция:
        if (v.size() != 4) return std::nullopt;
        if (v[2] <= 0) return std::nullopt; // радиус > 0
        // v[3] — double, но sides — целое
        auto sides_opt = parse_double(std::to_string(v[3]));
        if (!sides_opt.has_value()) return std::nullopt;
        int sides = static_cast<int>(v[3]);
        // должно быть целым и >=3
        if (sides != v[3] || sides < 3) return std::nullopt;
        return RegularPolygon{{v[0], v[1]}, v[2], sides};
*/
std::optional<Shape> MakePolygon(const std::vector<double>& v)
{
  if (v.size() != 4)
  {
    std::println("{}:{} - polygon needs 4 points, got {}!",
                 __FILE__, __LINE__, v.size());
    return std::nullopt;
  }

  if (v[2] <= 0)
  {
    std::println("{}:{} - radius must be > 0!", __FILE__, __LINE__);
    return std::nullopt;
  }

  int sides = (int)v[3];
  if (sides < 3)
  {
    std::println("{}:{} - number of sides must be >= 3!", __FILE__, __LINE__);
    return std::nullopt;
  }

  return RegularPolygon({ v[0], v[1] }, v[2], sides);
}

// =============================================================================

// Парсинг одной фигуры
std::optional<Shape> ParseSingleShape(std::string_view token)
{
  auto parts = SplitIntoWords(token);
  if (parts.empty())
  {
    return std::nullopt;
  }

  std::string_view type = parts[0];
  std::string param_str;
  for (auto i : std::views::iota(1u, parts.size()))
  {
    if (!param_str.empty())
    {
      param_str += ' ';
    }

    param_str += std::string(parts[i]);
  }

  using WTF = std::optional<
    std::function<std::optional<Shape>(const std::vector<double>&)>
  >;

  // Выбираем конструктор по имени
  auto get_maker =
  [](std::string_view t) -> WTF {
      if (t == "circle")    return MakeCircle;
      if (t == "line")      return MakeLine;
      if (t == "triangle")  return MakeTriangle;
      if (t == "rectangle") return MakeRectangle;
      if (t == "polygon")   return MakePolygon;
      else
      {
        std::println("{}:{} - unexpected shape token '{}'",
                     __FILE__, __LINE__, t);
      }
      return std::nullopt;
  };

  //Обратите внимание на код ниже
  return get_maker(type).and_then(
    [&](auto maker)
    {
      return ParseDoubles(param_str).and_then(maker);
    }
  );
}

// =============================================================================

std::vector<Shape> ParseShapes(std::string_view input)
{
  std::vector<Shape> result;

  // Разделяем по ';'
  size_t start = 0;
  size_t end = 0;
  while (start < input.size())
  {
    end = input.find(';', start);
    if (end == std::string_view::npos)
    {
      end = input.size();
    }

    std::string_view token = input.substr(start, end - start);

    // Убираем пробелы по краям
    while (!token.empty()
        && std::isspace(static_cast<unsigned char>(token.front())))
    {
      token.remove_prefix(1);
    }

    while (!token.empty()
        && std::isspace(static_cast<unsigned char>(token.back())))
    {
      token.remove_suffix(1);
    }

    if (!token.empty())
    {
      std::optional<Shape> shape = ParseSingleShape(token);
      if (shape.has_value())
      {
        result.push_back(*shape);
      }
    }

    start = end + 1;
  }

  return result;
}

// =============================================================================

std::vector<std::pair<Shape, Shape>>
FindAllCollisions(std::span<const Shape> shapes)
{
  using namespace geometry::queries;

  std::vector<std::pair<Shape, Shape>> collisions;

  /*
    * Используйте библиотеку ranges, чтобы найти все коллизии между фигурами
    * методом BoundingBoxesOverlap
    *
    * Также используйте наиболее эффективный метод добавления объектов в
    * collisions
  */

  //
  // Мне это всё не нравится, но поскольку по заданию положено, то хрен с ним.
  // Тем не менее, в оправдание своего нытья, оставлю вот эту цитату здесь:
  //
  // https://www.reddit.com/r/cpp/comments/1bx6ube/comment/kybteha/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
  //
  // """
  // Never use std::views in code that does anything beyond very basic plumbing.  //
  // In cases where all you do is very basic plumbing, consider not using them
  // because it doesn't matter if you saved yourself 1 line of code if now you
  // create a possibility for major issues in the future.
  //
  // If you have many-many places where the views would save a meaningful number
  // of lines of code, ask yourself, why is this codebase using C++ to begin
  // with. It's never going to be as good for some random plumbing as, say,
  // Python, and real-world cases where C++ is currently the best choice (or one
  // of the best choices) almost never have these situations.
  //
  // You'll never meet a non-incompetent gamedev, or a writer of a high-perf
  // code in other areas who would say "yes, sure, I would love to zip, drop,
  // take and filter all the time". Even when you have places of the code that
  // fit into the pattern and can technically be rewritten with ranges, these
  // places are also often the ones where someone might want to meaningfully
  // step through with a debugger, so a for loop is better just for that reason
  // alone.
  //
  // Add a risk of shooting yourself in the foot on every step plus
  // non-insignificant performance tax that occasionally happens (it doesn't
  // look like it must happen in principle, but compilers are not there yet to
  // ensure that the abstraction overhead of ranges is always zero), and you are
  // left with almost no situations where piping ranges together is a not a
  // terrible idea. And the remaining 0.001% of the cases? The cost of teaching
  // all the people to read this new syntax dramatically outweighs the benefits.
  // No, it's not obvious wtf the take operation is supposed to do, or how would
  // zip behave if the number of elements in parameters is different, and why
  // bitwise-or operator is used to connect these things together.
  //
  // I'm very curious how this nonsense of a library was lobbied into the
  // standard.
  //
  // But at least now we got a way to call some basic algorithms like sort in a
  // more convenient way, so this is nice. In a better world, we would leave
  // these good parts and remove everything else, but instead, we got a feature
  // with a destructive power close to std::initializer_list, and I didn't think
  // we'll ever top that one.
  // """
  //

  auto indices = std::views::iota(0u, shapes.size());

  for (size_t i = 0; i < shapes.size(); ++i)
  {
    auto remaining = indices | std::views::drop(i + 1) | std::views::filter(
      [&shapes, i](size_t j)
      {
        return BoundingBoxesOverlap(shapes[i], shapes[j]);
      }
    );

    for (size_t j : remaining)
    {
      collisions.emplace_back(shapes[i], shapes[j]);
    }
  }

  return collisions;
}

// =============================================================================

std::optional<size_t> FindHighestShape(std::span<const Shape> shapes)
{
  /*
    * Используйте библиотеку ranges, чтобы найти самую высокую фигуру
    *
    * Важно: использование ручной итерации по фигурам не разрешается
  */

  auto highest_it = std::ranges::max_element(
    shapes,
    std::less{},
    [](const Shape& shape) -> double
    {
      return std::visit(
        // fucking 'auto' is mandatory for std::visit pattern.
        [](const auto& s) -> double
        {
          if constexpr (
            std::is_same_v<std::remove_cvref_t<decltype(s)>, std::monostate>
          )
          {
            return -std::numeric_limits<double>::infinity();
          }
          else
          {
            return s.Height();
          }
        },
        shape
      );
    }
  );

  if (highest_it != shapes.end()
  and !std::holds_alternative<std::monostate>(*highest_it))
  {
    return std::distance(shapes.begin(), highest_it);
  }

  return std::nullopt;
}
}
