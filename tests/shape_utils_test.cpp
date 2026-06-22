#include <gtest/gtest.h>
#include "shape_utils.hpp"
#include "queries.hpp"
#include "geometry.hpp"

using namespace geometry;
using namespace geometry::queries;
using namespace geometry::utils;

TEST(ShapeUtilsTest, SimpleCheck)
{
  std::vector<Shape> shapes = ParseShapes(
    "rectangle 0 0 1 2; "
    "rectangle 0.5 0 1 1; "
    "circle 0 0 0.5; "
    "line -1 5 1 5; "
  );

  std::vector<std::pair<Shape, Shape>> colls = FindAllCollisions(shapes);

  EXPECT_TRUE(colls.size() == 3);

  std::optional<size_t> ind = FindHighestShape(shapes);
  ASSERT_TRUE(ind.has_value());
  EXPECT_DOUBLE_EQ(3, ind.value());
}
