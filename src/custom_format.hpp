#include "geometry.hpp"

namespace std {

// =============================================================================

template <>
struct formatter<monostate>
{
  constexpr auto parse(format_parse_context& ctx)
  {
    return ctx.begin();
  }

  //
  // Note: this function must be const qualified for custom formatting to work.
  //
  template <typename FormatContext>
  auto format(const monostate& v, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "< std::monostate {:p} >", (void*)&v);
  }
};

// =============================================================================

template <>
struct formatter<geometry::Point2D>
{
  constexpr auto parse(format_parse_context& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const geometry::Point2D& p, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "< Point2D ({:.4f} {:.4f}) >", p.x, p.y);
  }
};

// =============================================================================

template <>
struct formatter<vector<geometry::Point2D>>
{
  bool use_new_line = false;

  constexpr auto parse(format_parse_context& ctx)
  {
    auto it  = ctx.begin();
    auto end = ctx.end();

    if ((it != end) and (*it == ':'))
    {
      it++;

      const string_view nl = "new_line";
      if (string_view(it, end).starts_with(nl))
      {
        use_new_line = true;
        it += nl.size();
      }
      else
      {
        throw format_error("Invalid format specifier for vector<Point2D>. "
                           "Expected ':new_line' or empty.");
      }
    }

    return it;
  }

  template <typename FormatContext>
  auto format(const vector<geometry::Point2D>& v, FormatContext& ctx) const
  {
    stringstream ss;
    for (auto& p : v)
    {
      if (use_new_line)
      {
        ss << std::format("\t{}\n", p);
      }
      else
      {
        ss << std::format("{} ", p);
      }
    }

    return format_to(ctx.out(), "{}", ss.str());
  }
};

// =============================================================================

template <>
struct formatter<geometry::Line>
{
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const geometry::Line &l, FormatContext &ctx) const
  {
    return format_to(ctx.out(), "< Line ({}, {}) >", l.start, l.end);
  }
};

template <>
struct formatter<geometry::Circle>
{
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const geometry::Circle &c, FormatContext &ctx) const
  {
      return format_to(
        ctx.out(),
        "< Circle (c = {}, r = {:.2f}) >",
        c.center_p,
        c.radius
      );
  }
};

// =============================================================================

template <>
struct formatter<geometry::Rectangle>
{
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const geometry::Rectangle &r, FormatContext &ctx) const
  {
    return format_to(
      ctx.out(),
      "< Rectangle (bottom_left={}, w={:.2f}, h={:.2f}) >",
      r.bottom_left,
      r.width,
      r.height
    );
  }
};

template <>
struct formatter<geometry::RegularPolygon>
{
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const geometry::RegularPolygon &p, FormatContext &ctx) const
  {
    return format_to(
      ctx.out(),
      "< RegularPolygon (center={}, r={:.2f}, sides={}) >",
      p.center_p,
      p.radius,
      p.sides
    );
  }
};

// =============================================================================

template <>
struct formatter<geometry::Triangle>
{
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const geometry::Triangle &t, FormatContext &ctx) const
  {
    return format_to(ctx.out(), "< Triangle ({}, {}, {}) >", t.a, t.b, t.c);
  }
};

template <>
struct formatter<geometry::Polygon>
{
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const geometry::Polygon &poly, FormatContext &ctx) const
  {
    auto out = ctx.out();
    out = format_to(out, "< Polygon [{} points]: [", poly.Vertices().size());

    for (const auto &p : poly.Vertices())
    {
      out = format_to(out, "{} ", p);
    }

    return format_to(out, "] >");
  }
};

// =============================================================================

template <>
struct formatter<geometry::Shape>
{
  constexpr auto parse(format_parse_context& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const geometry::Shape& shape, FormatContext& ctx) const
  {
    stringstream ss;
    shape.visit(
      [&ctx, &ss](const auto& s)
      {
        if constexpr (EmptyVariant<decltype(s)>)
        {
          ss << std::format("< std::monostate {:p} >", (void*)&s);
        }
        else
        {
          ss << std::format("{}", s);
        }
      }
    );

    return format_to(ctx.out(), "{}", ss.str());
  }
};

} // namespace std
