#include "geometry.hpp"
#include "triangulation.hpp"
#include <span>

namespace geometry::visualization {

void Draw(std::span<geometry::Shape> shapes,
          const std::string& plotName = std::string());

void Draw(std::span<const geometry::triangulation::DelaunayTriangle> triangles);

}  // namespace geometry::visualization
