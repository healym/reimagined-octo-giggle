#ifndef ALGO_HPP
#define ALGO_HPP
#include "search.hpp"

namespace Search {
  Brain::Action IteratingDepthDepthLimitedMiniMax(Node root, int limit);
  Brain::Action DepthLimitedMiniMax(Node root, int limit);
  int MaxValue(Node root, int limit);
  int MinValue(Node root, int limit);
}
#endif