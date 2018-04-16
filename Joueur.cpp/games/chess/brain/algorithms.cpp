#include <algorithm>
#include <iostream>
#include "algorithms.hpp"

namespace Search {


  Brain::Action TLHTQSIDABDLMM(Node root,
                               int turn,
                               double time_left,
                               int alpha,
                               int beta,
                               int qlimit,
                               int limit) {
    Brain::Action current_best;
    // y = ( 1 - e-kt ), k > 0 :: expenential decay(increasing form)
    //                            never exceeds C (half time_left in sec)
    //double time_alloted = 0.5 * time_left * 1000000000 * ( 1 - exp(-(0.05) * turn));
    double time_alloted = (0.5 * time_left) * 1000000000;
    time_t start = time(nullptr);
    int depth = 1; 
    while (depth <= limit) {
      std::cout << "         =<<< DEPTH : " << depth << " >>>=" << std::endl;
      try {
        current_best = Search::TLHTQSABDLMM(root, time_alloted, start, alpha, beta, qlimit, depth);
      } catch (timeException& e) {
        std::cout << e.msg() << std::endl;
        break;
      }
      depth++;
    }
    return current_best;
  }

  Brain::Action TLHTQSABDLMM(Node root,
                             double time_alloted,
                             time_t start,
                             int alpha,
                             int beta,
                             int qlimit,
                             int limit) {
    std::map<string, int> hist_table;
    bool fail = false;
    std::vector<Node> frontier;
    std::vector<Node> explored;
    Brain::Action best_action;
    int best_score = 0;
    Brain::Action current_action;
    int current_score = 0;
    bool found = false;

    if (difftime(start, time(nullptr)) >= time_alloted) {
      throw timeException("Out of time");
    }

    frontier = root.children();
    for (auto child : frontier) {
      fail = false;
      current_score = TLHTQSMinValue(child, time_alloted, start, alpha, beta, qlimit, limit, hist_table);
      if (current_score <= alpha) {
        fail = true; // FAIL LOW
      } else if (current_score >= beta) {
        fail = true;
        found = true;
        best_score = current_score;
        best_action = child.action();
        break; // FAIL HIGH -- PRUNE
      }
      if (current_score > best_score) {
        found = true;
        alpha = !fail ? current_score : alpha; // only set on not fail
        best_score = current_score;
        best_action = child.action();
      }
    }
    if (!found) {
      best_action = frontier.at(rand() % frontier.size()).action();
    }
    return best_action;
  }

  int TLHTQSMinValue(Node root,
                    double time_alloted,
                    time_t start,
                    int alpha,
                    int beta,
                    int qlimit,
                    int limit,
                    std::map<string, int>& hist) {
    bool fail = false;
    bool qsearch = false;
    std::vector<Node> frontier;
    std::vector<Node> explored;
    int best_score = 0;
    int current_score = 0;
    bool found = false;
    Node* best_node;

    if (difftime(start, time(nullptr)) >= time_alloted) {
      throw timeException("Out of time");
    }

    if ( limit == 0 || root.is_terminal()) {
      return root.score();
    } else {
      if (!Quiescent(root) && limit == 0) {
        qsearch = true;
      }
      frontier = root.children();
      for (auto child : frontier) {
        fail = false;
        current_score = TLHTQSMaxValue(child,
                                       time_alloted,
                                       start, alpha,
                                       beta,
                                       qsearch ? qlimit - 1 : qlimit,
                                       qsearch ? limit : limit - 1,
                                       hist);
        if (current_score <= alpha) {
          fail = true;
          found = true;
          best_score = current_score;
          break; // FAIL LOW -- PRUNE
        } else if (current_score >= beta) {
          fail = true; // FAIL HIGH
        }
        if (current_score < best_score) {
          found = true;
          beta = !fail ? current_score : beta; // only set when not fail
          best_score = current_score;
          best_node = &child;
          break;
        }
      }
      if (!found) {
        best_score = frontier.at(rand() % frontier.size()).score();
      }
      if (hist.find(best_node->action().String()) == hist.end()) {
        hist[best_node->action().String()]++;
      } else {
        hist[best_node->action().String()] = 1;
      }
      return best_score;
    }
  }

  int TLHTQSMaxValue(Node root,
                     double time_alloted,
                     time_t start,
                     int alpha,
                     int beta,
                     int qlimit,
                     int limit,
                     std::map<string, int>& hist) {
    std::vector<Node> frontier;
    std::vector<Node> explored;
    Node* best_node;
    int best_score = 0;
    int current_score = 0;
    bool found = false;
    bool fail = false;
    bool qsearch = false;

    if (difftime(start, time(nullptr)) >= time_alloted) {
      throw timeException("Out of time");
    }

    if (Quiescent(root) && (limit == 0 || root.is_terminal())) {
      return root.score();
    } else {
      if (!Quiescent(root) && limit == 0)
      frontier = root.children();
      for (auto child : frontier) {
        fail = false;
        current_score = TLHTQSMinValue(child,
                                       time_alloted,
                                       start, alpha,
                                       beta,
                                       qsearch ? qlimit - 1 : qlimit,
                                       qsearch ? limit : limit - 1,
                                       hist);
        if (current_score <= alpha) {
          fail = true; // FAIL LOW
        } else if (current_score >= beta) {
          fail = true;
          found = true;
          best_score = current_score;
          break; // FAIL HIGH -- PRUNE
        }
        if (current_score > best_score) {
          found = true;
          alpha = !fail ? current_score : alpha; // only set when not fail
          best_score = current_score;
          best_node = &child;
          if (hist.find(best_node->action().String()) == hist.end()) {
            hist[best_node->action().String()]++;
          } else {
           hist[best_node->action().String()] = 1;
         }
          break;
        }
      }
      if (!found) {
        return 0;
      }
      return best_score;
    }
  }

  bool Quiescent(Node root) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        if (Brain::threat(root.player(), i+'A', j+1, root.board().txt_board)) {
          return false;
        }
      }
    }
    return true;
  }
}