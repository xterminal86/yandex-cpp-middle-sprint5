#include <gtest/gtest.h>
#include "visualization.hpp"
#include "shape_utils.hpp"

using namespace geometry;
using namespace geometry::visualization;

TEST(VisualizationTest, SimpleCheck)
{
  std::vector<Shape> shapes = utils::ParseShapes(
    "line 1 2 2 4; "
    "line 2 2 1 3"
  );

  Draw(shapes);
}
