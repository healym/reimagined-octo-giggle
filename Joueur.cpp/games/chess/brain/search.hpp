#ifndef SEARCH_HPP
#define SEARCH_HPP
/*
 * file including all search utilities
 */
#include <vector>
#include <queue>
#include "chess_pieces.hpp"

const int STEP_COST = 1;
const int KING_VAL = 39;

class Node {
public:
  /// <summary>
  /// Node constructor that takes a parent node as an argument
  /// </summary>
  Node(Node* parent, Brain::Action action);

  /// <summary>
  /// default Node constructor
  /// </summary>
  Node(string player, std::vector<Brain::Action> moves, const Brain::Chessboard);
  Node(int pcost,
        int depth,
        Node *parent,
        Brain::Action action,
        string player,
        string last_move,
        string next_last_move,
        Brain::GameInfo &info);
  bool is_terminal();
  int path_cost() const { return m_path_cost; };
  void third();
  bool is_goal()const { return m_is_goal; };
  int depth() const { return m_depth; };
  int score() const { return m_score; };
  Brain::GameInfo info() const { return m_info; };
  Node* parent() const { return m_parent; };
  Brain::Action action() const { return m_action; };
  Brain::Chessboard board();
  /// <summary>
  /// returns a vector of actions that can be taken, given the node's state
  /// </summary>
  /// @params string player: the color of the player
  /// @params string last_move: the last move made in SAN
  /// @params string next_last_move: the next to last move made in SAN
  /// @params Brain::GameInfo &castle: the information that can be gleaned from a FEN string
  std::vector<Brain::Action> all_actions();
  std::vector<Node> children();
private:
  std::vector<Brain::Action> m_moves;
  string m_player;
  string m_max_player;
  string m_last_move;
  string m_next_last_move;
  string m_third_move;
  Brain::Chessboard m_third_state;
  int m_score;
  int m_path_cost;
  bool m_is_goal;
  Brain::Action m_action;
  int m_depth;
  Node* m_parent;
  Brain::Chessboard m_board;
  Brain::GameInfo m_info;
};

inline bool operator<(const Node& lhs, const Node& rhs) {
  return lhs.score() < rhs.score();
}

int Score(string player, char board[8][8]);
Brain::Chessboard Result(Brain::Action a, const char board[8][8]);

#endif