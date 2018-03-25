#include <algorithm>
#include "search.hpp"
#include <iostream>

using namespace std;


Node::Node(string player, std::vector<Brain::Action> moves, const Brain::Chessboard board) {
  m_parent = nullptr;
  m_path_cost = 0;
  m_player = string(1,tolower(player[0]));
  m_max_player = string(1,tolower(player[0]));
  m_moves = moves;
  m_last_move = moves.size() >= 1 ? string(moves.at(0).piece())
                      + string(1,moves.at(0).file())
                      + to_string(moves.at(0).rank())
                : "";
  m_next_last_move = moves.size() >= 2 ? string(moves.at(1).piece())
                      + string(1,moves.at(1).file())
                      + to_string(moves.at(1).rank())
                : "";
  m_score = Score(m_max_player, m_board.txt_board);
  m_board = board;
  m_third_state = board;
}

Node::Node(Node* parent, Brain::Action action) {
  m_parent = parent;
  m_player = parent->m_player == "w" ? "b" : "w";
  m_depth = parent->m_depth + 1;
  m_max_player = parent->m_max_player;
  m_path_cost = parent->path_cost() + STEP_COST;
  m_action = action;
  m_score = Score(m_max_player, m_board.txt_board);
  m_last_move = string(parent->action().piece())
      + string(1,parent->action().file())
      + to_string(parent->action().rank());
  m_next_last_move = parent != nullptr ? parent->m_last_move : "";
  m_moves = parent->m_moves;
  m_board = Result(action, parent->m_board.txt_board);
  if (m_moves.size() >= 8) {
    m_moves.erase(m_moves.begin());
  }
  m_moves.push_back(action);
}

std::vector<Node> Node::children() {
  std::vector<Node> children;
  std::vector<Brain::Action> actions;
  actions = all_actions();
  for (Brain::Action action : actions) {
    Node child(this, action);
    children.push_back(child);
  }
  return children;
}

bool Node::is_terminal() {
  std::vector<Brain::Action> actions = all_actions();
  if (actions.size() == 0) {
    if (isntCheck(this->m_action, this->m_player[0], this->m_parent->m_board.txt_board)) {
      m_score = -100; // draw
    } else {
      m_score = -500; // win
    }
  }
  return actions.size() == 0;
}

int Score(string player, char board[8][8]) {
  int score = 0;
  int ally_offset = player == "w" ? 0 : 32;
  int enemy_offset = player == "w" ? 32 : 0;
  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      if (board[file][rank] == 'P' + ally_offset) {
        score += 1;
      } else if (board[file][rank] == 'P' + enemy_offset) {
        score -= 1;
      } else if (board[file][rank] == 'N' + ally_offset) {
        score += 3;
      } else if (board[file][rank] == 'N' + enemy_offset) {
        score -= 3;
      } else if (board[file][rank] == 'B' + ally_offset) {
        score += 3;
      } else if (board[file][rank] == 'B' + enemy_offset) {
        score -= 3;
      } else if (board[file][rank] == 'R' + ally_offset) {
        score += 6;
      } else if (board[file][rank] == 'R' + enemy_offset) {
        score -= 6;
      } else if (board[file][rank] == 'Q' + ally_offset) {
        score += 9;
      } else if (board[file][rank] == 'Q' + enemy_offset) {
        score -= 9;
      } else if (board[file][rank] == 'K' + ally_offset) {
        score += KING_VAL;
      } else if (board[file][rank] == 'K' + enemy_offset) {
        score -= KING_VAL;
      }
    }
  }
  return score;
}

Brain::Chessboard Result(Brain::Action action, const char board[8][8]) {
  char board_copy[8][8];
  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      board_copy[file][rank] = board[file][rank];
    }
  }
  char old_piece = board_copy[action.last_file() - 65][action.last_rank() - 1];
  board_copy[action.file() - 65][action.rank() - 1] = old_piece;
  board_copy[action.last_file() - 65][action.last_rank() - 1] = '.';

  return Brain::Chessboard(board_copy);
}

vector<Brain::Action> Node::all_actions() {
  return m_board.Actions(m_player, m_last_move, m_next_last_move, m_moves, m_info);
}
