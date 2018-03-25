#include <algorithm>
#include <iostream>
#include "algorithms.hpp"

namespace Search {

  Brain::Action IteratingDepthDepthLimitedMiniMax(Node root, int limit) {
    Brain::Action current_best;
    for (int depth = 1; depth <= limit; depth++) {
      std::cout << "        =<<< DEPTH : " << depth << " >>>=" << std::endl;
      current_best = Search::DepthLimitedMiniMax(root, depth);
    }
    return current_best;
  }

  Brain::Action DepthLimitedMiniMax(Node root, int limit) {
    std::vector<Node> frontier;
    std::vector<Node> explored;
    Brain::Action best_action;
    int best_score = 0;
    Brain::Action current_action;
    int current_score = 0;
    bool found = false;

    frontier = root.children();
    for (auto child : frontier) {
      current_score = MinValue(child, limit);
      if (current_score > best_score) {
        found = true;
        best_score = current_score;
        best_action = child.action();
      }
    }
    if (!found) {
      best_action = frontier.at(rand() % frontier.size()).action();
    }
    return best_action;
  }

  int MinValue(Node root, int limit) {
        std::vector<Node> frontier;
    std::vector<Node> explored;
    int best_score = 0;
    int current_score = 0;
    bool found = false;

    if ( limit == 0 || root.is_terminal()) {
      return root.score();
    } else {
      frontier = root.children();
      for (auto child : frontier) {
        current_score = MaxValue(child, limit - 1);
        if (current_score < best_score) {
          found = true;
          best_score = current_score;
        }
      }
      if (!found) {
        best_score = frontier.at(rand() % frontier.size()).score();
      }
      return best_score;
    }
  }

    int MaxValue(Node root, int limit) {
        std::vector<Node> frontier;
    std::vector<Node> explored;
    int best_score = 0;
    int current_score = 0;
    bool found = false;

    if (limit == 0 || root.is_terminal()) {
      return root.score();
    } else {
      frontier = root.children();
      for (auto child : frontier) {
        current_score = MinValue(child, limit - 1);
        if (current_score > best_score) {
          found = true;
          best_score = current_score;
        }
      }
      if (!found) {
        best_score = frontier.at(rand() % frontier.size()).score();
      }
      return best_score;
    }
  }

}