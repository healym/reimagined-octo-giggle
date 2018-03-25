#include <iostream>
#include <sstream>
#include <algorithm>
#include "chess_pieces.hpp"
#include "search.hpp"

namespace Brain {

  bool Action::operator==(const Brain::Action &rhs) const {
    return (this->last_file() == rhs.last_file()
          && this->last_rank() == rhs.last_rank()
          && this->file() == rhs.file()
          && this->rank() == rhs.rank());
  }

  Chessboard::Chessboard() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        txt_board[i][j] = '.';
      }
    }
  }

  bool Chessboard::Chessboard::operator==(Chessboard& other) {
    for (int file = 0; file < 8; file++) {
      for (int rank = 0; rank < 0; rank++) {
        if (txt_board[file][rank] != other.txt_board[file][rank]) {
          return false;
        }
      }
    }
    return true;
  }

  Chessboard::Chessboard(char board[8][8]) {
    for (int file = 0; file < 8; file++) {
      for (int rank = 0; rank < 8; rank++) {
        txt_board[file][rank] = board[file][rank];
      }
    }
  }

  std::vector<Action> Chessboard::Actions(std::string player_string,
      std::string last_move,
      std::string next_last_move,
      std::vector<Brain::Action> moves,
      GameInfo &castle) const {
    char player = tolower(char(player_string[0]));
    std::vector<Action> actions;
    std::vector<Action> new_actions;
    actions.clear();
    int color_offset = player == 'w' ? 0 : 32;
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        if (txt_board[i][j] == 'K' + color_offset) {
            new_actions = King::Actions(j+1, char(i+65), player, txt_board);
        } else if (txt_board[i][j] == 'Q' + color_offset) {
            new_actions = Queen::Actions(j+1, char(i+65), player, txt_board);
        } else if (txt_board[i][j] == 'R' + color_offset) {
            new_actions = Rook::Actions(j+1, char(i+65), player, txt_board);
        } else if (txt_board[i][j] == 'B' + color_offset) {
            new_actions = Bishop::Actions(j+1, char(i+65), player, txt_board);
        } else if (txt_board[i][j] == 'P' + color_offset) {
            new_actions = Pawn::Actions(j+1, char(i+65), player, txt_board, last_move, next_last_move);
        } else if (txt_board[i][j] == 'N' + color_offset) {
            new_actions = Knight::Actions(j+1, char(i+65), player, txt_board);
        }
        // remove three-fold repetition
      /*new_actions.erase(std::remove_if(new_actions.begin(), new_actions.end(), [this, moves](Brain::Action current){
        for (auto move : moves) {
          if (move.capture) {
            return true;
          }
        }
        if (moves.size() >= 8
            && current == moves.at(4)
            && moves.at(1) == moves.at(5)
            && moves.at(2) == moves.at(6)
            && moves.at(3) == moves.at(7)) {
          return true;
        }
      }), new_actions.end());
      */
        actions.insert(actions.end(), new_actions.begin(), new_actions.end());
        new_actions.clear();
      }
    }

    new_actions = Castle::Actions(player, castle, txt_board);
    // remove three-fold repetition
    /*new_actions.erase(std::remove_if(new_actions.begin(), new_actions.end(), [this, moves](Brain::Action current){
        for (auto move : moves) {
          if (move.capture) {
            return true;
          }
        }
        if (moves.size() >= 8
            && current == moves.at(4)
            && moves.at(1) == moves.at(5)
            && moves.at(2) == moves.at(6)
            && moves.at(3) == moves.at(7)) {
          return true;
        }
      }), new_actions.end());*/
    actions.insert(actions.end(), new_actions.begin(), new_actions.end());
    return actions;
  }

  std::vector<Action> Castle::Actions(char color, Brain::GameInfo castle, const char board[8][8]) {
    std::vector<Action> actions;
    actions.clear();
    int rank = color == 'w' ? 0 : 7;
    string piece = color == 'w' ? "K" : "k";
    string queen_check = color == 'w' ? "Q" : "q";
    // QUEEN-SIDE CASTLING
    if (castle.castling().find(queen_check) != std::string::npos
        && board[1][rank-1] == '.'
        && board[2][rank-1] == '.'
        && board[3][rank-1] == '.'
        && !threat(color, 'b', rank, board)
        && !threat(color, 'c', rank, board)
        && !threat(color, 'd', rank, board)
        && !threat(color, 'e', rank, board)) {
    actions.push_back(Action("0-0", 'e', rank, '-', 0));
    }
    // KING-SIDE CASTLING
    if (castle.castling().find(piece) != std::string::npos
        && board[5][rank-1] == '.'
        && board[6][rank-1] == '.'
        && !threat(color, 'e', rank, board)
        && !threat(color, 'f', rank, board)
        && !threat(color, 'g', rank, board)) {
      actions.push_back(Action("0", 'e', rank, '-', 0));
    }

    return actions;
  }

  std::vector<Action> Pawn::Actions(int rank, char file, char color, const char board[8][8],
                            std::string last_move, std::string next_last_move) {

    std::vector<Action> actions;
    actions.clear();
    int big_rank = color == 'w' ? 2 : 7;
    int enpass_rank = color == 'w' ? 5 : 4;
    int offset = color == 'w' ? 1 : -1;
    int lower_bound = color == 'w' ? 97 : 65;
    int upper_bound = color == 'w' ? 123 : 91;
    int color_offset = color == 'w' ? 32 : 0;
    std::string piece = color == 'w' ? "P" : "p";

    // find san notation for moves required for a pawn "big move"
    std::string left_space = color == 'w' ? std::string(1, file) + "5" : std::string(1, file) + "4";
    std::string right_space = color == 'w' ? std::string(1, file) + "5" : std::string(1, file) + "4";
    std::string fl_space = color == 'w' ? std::string(1, file) + "6" : std::string(1, file) + "3";
    std::string fr_space = color == 'w' ? std::string(1, file) + "6" : std::string(1, file) + "3";

    if ((color == 'w' && rank == 7 && board[file-65][rank-1+offset] == '.')
        || (color == 'b' && rank == 2 && board[file-65][rank-1+offset] == '.')) {
      if (isntCheck(Action(piece, file, rank, file, rank+offset), color, board)) {
        actions.push_back(Action(piece, file, rank, file, rank+offset, "Queen", true));
        actions.push_back(Action(piece, file, rank, file, rank+offset, "Knight", true));
        actions.push_back(Action(piece, file, rank, file, rank+offset, "Bishop", true));
        actions.push_back(Action(piece, file, rank, file, rank+offset, "Rook", true));
      }
    }

    if (rank == big_rank
        && board[file-65][rank-1+offset] == '.'
        && board[file-65][rank-1+(2*offset)] == '.') {
      if (isntCheck(Action(piece, file, rank, file, rank+(2*offset)), color, board)) {
        actions.push_back(Action(piece, file, rank, file, rank+(2*offset), true));
      }
    }

    if (file-65 > 0 
        && board[file-66][rank-1+offset] >= lower_bound
        && board[file-66][rank-1+offset] < upper_bound) {
      if (isntCheck(Action(piece, file, rank, char(file-1), rank+offset), color, board)) {
        actions.push_back(Action(piece, file, rank, char(file-1), rank+offset, true));
      }
    }

    if (file-65 < 7
        && board[file-64][rank-1+offset] >= lower_bound
        && board[file-64][rank-1+offset] < upper_bound) {
      if (isntCheck(Action(piece, file, rank, char(file+1), rank+offset), color, board)) {
        actions.push_back(Action(piece, file, rank, char(file+1), rank+offset, true));
      }
    }

    if (rank+offset < 8 // dont go into promotion zone unless promoting
        && rank+offset >= 2
        && board[file-65][rank-1+offset] == '.') {
      if (isntCheck(Action(piece, file, rank, file, rank+offset), color, board)) {
        actions.push_back(Action(piece, file, rank, file, rank+offset, true));
      }
    }


    if (board[file-66][enpass_rank] == 'P'+color_offset && rank == enpass_rank) {
      if (last_move == left_space && next_last_move != fl_space)
      {
        if (isntCheck(Action(piece, file, rank, char(file-1), rank+offset), color, board))
          actions.push_back(Action(piece, file, rank, char(file-1), rank+offset, true));
      }
    }

    if (board[file-64][enpass_rank] == 'P'+color_offset && rank == enpass_rank) {
      if (last_move == right_space && next_last_move != fr_space)
      {
        if (isntCheck(Action(piece, file, rank, char(file+1), rank+offset), color, board))
          actions.push_back(Action(piece, file, rank, char(file+1), rank+offset, true));
      }
    }
    return actions;
  }

  std::vector<Action> Knight::Actions(int rank, char file, char color, const char board[8][8]) {
    std::vector<Action> actions;
    std::string piece = color == 'w' ? "N" : "n";
    int lower_bound = color == 'w' ? 97 : 65;
    int upper_bound = color == 'w' ? 123 : 91;
    actions.clear();
    char ttr = rank < 7 && file-65 < 7 ? board[file-64][rank+1] : ':';
    char trr = rank < 8 && file-65 < 6 ? board[file-63][rank] : ':';
    char ttl = rank < 7 && file-65 > 0 ? board[file-66][rank+1] : ':';
    char tll = rank < 8 && file-65 > 1 ? board[file-67][rank] : ':';
    char bbl = rank > 2 && file-65 > 0 ? board[file-66][rank-3] : ':';
    char bll = rank > 1 && file-65 > 1 ? board[file-67][rank-2] : ':';
    char bbr = rank > 2 && file-65 < 7 ? board[file-64][rank-3] : ':';
    char brr = rank > 1 && file-65 < 6 ? board[file-63][rank-2] : ':';

    if (ttr == '.' || (ttr >= lower_bound && ttr < upper_bound)) {
      if (ttr >= lower_bound && ttr < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file+1), rank+2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+1), rank+2));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file+1), rank+2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+1), rank+2));
        }
      }
    }

    if (trr == '.' || (trr >= lower_bound && trr < upper_bound)) {
      if (trr >= lower_bound && trr < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file+2), rank+1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+2), rank+1));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file+2), rank+1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+2), rank+1));
        }
      }
    }

    if (ttl == '.' || (ttl >= lower_bound && ttl < upper_bound)) {
      if (ttl >= lower_bound && ttl < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file-1), rank+2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-1), rank+2));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file-1), rank+2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-1), rank+2));
        }
      }
    }

    if (tll == '.' || (tll >= lower_bound && tll < upper_bound)) {
      if (tll >= lower_bound && tll < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file-2), rank+1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-2), rank+1));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file-2), rank+1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-2), rank+1));
        }
      }
    }

    if (bbl == '.' || (bbl >= lower_bound && bbl < upper_bound)) {
      if (bbl >= lower_bound && bbl < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file-1), rank-2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-1), rank-2));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file-1), rank-2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-1), rank-2));
        }
      }
    }

    if (bll == '.' || (bll >= lower_bound && bll < upper_bound)) {
      if (bll >= lower_bound && bll < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file-2), rank-1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-2), rank-1));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file-2), rank-1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file-2), rank-1));
        }
      }
    }

    if (bbr == '.' || (bbr >= lower_bound && bbr < upper_bound)) {
      if (bbr >= lower_bound && bbr < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file+1), rank-2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+1), rank-2));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file+1), rank-2), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+1), rank-2));
        }
      }
    }

    if (brr == '.' || (brr >= lower_bound && brr < upper_bound)) {
      if (brr >= lower_bound && brr < upper_bound) {
        if (isntCheck(Action(piece, file, rank, char(file+2), rank-1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+2), rank-1));
        }
      } else {
        if (isntCheck(Action(piece, file, rank, char(file+2), rank-1), color, board)) {
          actions.push_back(Action(piece, file, rank, char(file+2), rank-1));
        }
      }
    }

    return actions;
  }

  std::vector<Action> Bishop::Actions(int rank, char file, char color, const char board[8][8]) {
    std::vector<Action> actions;
    actions.clear();
    int f, r;
    int upper_bound = color == 'w' ? 123 : 91;
    int lower_bound = color == 'w' ? 97 : 65;
    std::string piece = color == 'w' ? "B" : "b";

    f = file - 64;
    r = rank;
    while(f < 8 && r < 8) { // checking north-east
      if (board[f][r] == '.' && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1));
      } else if (board[f][r] >= lower_bound && board[f][r] < upper_bound
                && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1, true));
        break; // enemy found, cannot continue
      } else {
        break; // ally found
      }
      f++;
      r++;
    }

    f = file - 66;
    r = rank;
    while(f >= 0 && r < 8) { // checking north-west
      if (board[f][r] == '.' && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1));
      } else if (board[f][r] >= lower_bound && board[f][r] < upper_bound
                && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1, true));
        break; // enemy found, cannot continue
      } else {
        break; // ally found
      }
      f--;
      r++;
    }

    f = file - 66;
    r = rank - 2;
    while(f >= 0 && r >= 0) { // checking south-west
      if (board[f][r] == '.' && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1));
      } else if (board[f][r] >= lower_bound && board[f][r] < upper_bound
                && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1, true));
        break; // enemy found, cannot continue
      } else {
        break; // ally found
      }
      f--;
      r--;
    }

    f = file - 64;
    r = rank - 2;
    while(f < 8 && r >= 0) { // checking south-east
      if (board[f][r] == '.' && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1));
      } else if (board[f][r] >= lower_bound && board[f][r] < upper_bound
                && isntCheck(Action(piece, file, rank, char(f+65), r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), r+1, true));
        break; // enemy found, cannot continue
      } else {
        break; // ally found
      }
      f++;
      r--;
    }

    return actions;
  }

  std::vector<Action> Rook::Actions(int rank, char file, char color, const char board[8][8]) {
    std::vector<Action> actions;
    actions.clear();
    int upper_bound = color == 'w' ? 123 : 91;
    int lower_bound = color == 'w' ? 97 : 65;
    std::string piece = color == 'w' ? "R" : "r";

    for (int r = rank; r < 8; r++) { // checking up
      if (board[file-65][r] == '.' && isntCheck(Action(piece, file, rank, file, r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, file, r+1));
      } else if (board[file-65][r] >= lower_bound && board[file-65][r] < upper_bound
                && isntCheck(Action(piece, file, rank, file, r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, file, r+1, true));
        break; // enemy found, cannot continue
      } else {
        break; // non-enemy found
      }
    }

    for (int r = rank-2; r >= 0; r--) { // checking down
      if (board[file-65][r] == '.' && isntCheck(Action(piece, file, rank, file, r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, file, r+1));
      } else if (board[file-65][r] >= lower_bound && board[file-65][r] < upper_bound
                && isntCheck(Action(piece, file, rank, file, r+1), color, board)) {
        actions.push_back(Action(piece, file, rank, file, r+1, true));
        break; // enemy found, cannot continue
      } else {
        break; // non-enemy found
      }
    }

    for (int f = file-66; f >= 0; f--) { // checking left
      if (board[f][rank-1] == '.' && isntCheck(Action(piece, file, rank, char(f+65), rank), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), rank));
      } else if (board[f][rank-1] >= lower_bound && board[f][rank-1] < upper_bound
                && isntCheck(Action(piece, file, rank, char(f+65), rank), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), rank, true));
        break; // enemy found, cannot continue
      } else {
        break; // non-enemy found
      }
    }

    for (int f = file-64; f < 8; f++) { // checking right
      if (board[f][rank-1] == '.' && isntCheck(Action(piece, file, rank, char(f+65), rank), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), rank));
      } else if (board[f][rank-1] >= lower_bound && board[f][rank-1] < upper_bound
                && isntCheck(Action(piece, file, rank, char(f+65), rank), color, board)) {
        actions.push_back(Action(piece, file, rank, char(f+65), rank, true));
        break; // enemy found, cannot continue
      } else {
        break; // non-enemy found
      }
    }
    return actions;
  }

  std::vector<Action> Queen::Actions(int rank, char file, char color, const char board[8][8]) {
    std::vector<Action> actions;
    std::vector<Action> new_actions;
    actions.clear();
    std::string piece = color == 'w' ? "Q" : "q";
    new_actions = Bishop::Actions(rank, file, color, board); // find bishop moves
    actions.insert(actions.end(), new_actions.begin(), new_actions.end()); // add bishop moves
    new_actions = Rook::Actions(rank, file, color, board); // find rook moves
    actions.insert(actions.end(), new_actions.begin(), new_actions.end()); // add rook moves

    for (auto action : actions) { // replace all piece-strings with queen
      action.piece_set(piece);
    }

    return actions;
  }

  std::vector<Action> King::Actions(int rank, char file, char color, const char board[8][8]) {
    std::vector<Action> actions;
    actions.clear();
    char current;
    int upper_bound = color == 'w' ? 123 : 91;
    int lower_bound = color == 'w' ? 97 : 65;
    string piece = color == 'w' ? "K" : "k";

    for (int f = -1; f < 2; f++) {
      for (int r = -1; r < 2; r++) {
        if ((f != 0 || f != 0)
            && rank + r >= 1 && rank + r <= 8
            && f + file - 65 >= 0 && f + file - 65 < 8) {
          current = board[f + file - 65][r + rank - 1];
          if (current == '.') {
            if (!threat(color, char(file+f), rank+r, board)) {
              actions.push_back(Action(piece, file, rank, char(file+f), rank+r));
            }
          } else if (current >= lower_bound && current < upper_bound) {
            if (!threat(color, char(file+f), rank+r, board)) {
              actions.push_back(Action(piece, file, rank, char(file+f), rank+r, true));
            }
          }
        }
      }
    }
    return actions;
  }

  bool threat(char color, char file, int rank, const char board[8][8]) {
    file = toupper(file); // ensure file is expected values
    int f,r;
    int color_offset = color == 'w' ? 32 : 0;
    int pawn_offset = color == 'w' ? -1 : 1;

    // ROOKS AND QUEENS
    for (int r = rank; r < 8; r++) { // checking up
      if (board[file-65][r] == 'R' + color_offset || board[file-65][r] == 'Q' + color_offset) {
        return true;
      } else if (board[file-65][r] != '.') {
        break;
      }
    }

    for (int r = rank-2; r >= 0; r--) { // checking down
      if (board[file-65][r] == 'R' + color_offset || board[file-65][r] == 'Q' + color_offset) {
        return true;
      } else if (board[file-65][r] != '.') {
        break;
      }
    }

    for (int f = file-66; f >= 0; f--) { // checking left
      if (board[f][rank-1] == 'R' + color_offset || board[f][rank-1] == 'Q' + color_offset) {
        return true;
      } else if (board[f][rank-1] != '.') {
        break;
      }
    }

    for (int f = file-64; f < 8; f++) { // checking right
      if (board[f][rank-1] == 'R' + color_offset || board[f][rank-1] == 'Q' + color_offset) {
        return true;
      } else if (board[f][rank-1] != '.') {
        break;
      }
    }

    // BISHOPS AND QUEENS
    f = file - 64;
    r = rank;
    while(f < 8 && r < 8) { //checking north-east
      if (board[f][r] == 'Q' + color_offset || board[f][r] == 'B' + color_offset) {
        return true;
      } else if (board[f][r] != '.') {
        break;
      }
      f++;
      r++;
    }

    f = file - 66;
    r = rank;
    while(f >= 0 && r < 8) { // checking north-west
      if (board[f][r] == 'Q' + color_offset || board[f][r] == 'B' + color_offset) {
        return true;
      } else if (board[f][r] != '.') {
        break;
      }
      f--;
      r++;
    }

    f = file - 64;
    r = rank - 2;
    while(f < 8 && r >= 0) { // checking south-east
      if (board[f][r] == 'Q' + color_offset || board[f][r] == 'B' + color_offset) {
        return true;
      } else if (board[f][r] != '.') {
        break;
      }
      f++;
      r--;
    }

    f = file - 66;
    r = rank - 2;
    while(f >= 0 && r >= 0) { // checking south-west
      if (board[f][r] == 'Q' + color_offset || board[f][r] == 'B' + color_offset) {
        return true;
      } else if (board[f][r] != '.') {
        break;
      }
      f--;
      r--;
    }

    // PAWNS
    if(!(color == 'w' && rank == 1) // don't check off the board
        && !(color=='b' && rank == 8)
        && file-66 >= 0 
        && board[file-66][rank-1+pawn_offset] == 'P' + color_offset) {
      return true;
    }

    if(!(color == 'w' && rank == 1) // don't check off the board
        && !(color=='b' && rank == 8)
        && file-64 >= 0
        && board[file-64][rank-1+pawn_offset] == 'P' + color_offset) {
      return true;
    }

  // KNIGHTS
    char ttr = rank < 7 && file-65 < 7 ? board[file-64][rank+1] : ':';
    char trr = rank < 8 && file-65 < 6 ? board[file-63][rank] : ':';
    char ttl = rank < 7 && file-65 > 0 ? board[file-66][rank+1] : ':';
    char tll = rank < 8 && file-65 > 1 ? board[file-67][rank] : ':';
    char bbl = rank > 2 && file-65 > 0 ? board[file-66][rank-3] : ':';
    char bll = rank > 1 && file-65 > 1 ? board[file-67][rank-2] : ':';
    char bbr = rank > 2 && file-65 < 7 ? board[file-64][rank-3] : ':';
    char brr = rank > 1 && file-65 < 6 ? board[file-63][rank-2] : ':';

  if (ttr == 'N' + color_offset
      || trr == 'N' + color_offset
      || ttl == 'N' + color_offset
      || tll == 'N' + color_offset
      || bbl == 'N' + color_offset
      || bll == 'N' + color_offset
      || bbr == 'N' + color_offset
      || brr == 'N' + color_offset) {
    return true;
  }
    return false; // looks like nothing is threatening us
  }

  Brain::GameInfo info_from_fen(const std::string &fen) {
    std::vector<std::string> fields;
    size_t pos = 0;
    std::string token;
    std::string fen_copy = fen;

    while ((pos = fen_copy.find(' ')) != std::string::npos) {
      token = fen_copy.substr(0, pos);
      fields.push_back(token);
      fen_copy.erase(0, pos + 1);
    }
    fields.push_back(fen_copy);

    return Brain::GameInfo(fields.at(1)[0], fields.at(2), fields.at(3), stoi(fields.at(4)), stoi(fields.at(5)));
  }

  void Chessboard::start_pos(const std::string &fen) {
    int i = 0;
    for (int rank = 7; rank >= 0; rank--) {
      for (int file = 0; file < 8; file++) {
        switch (fen[i++]) {
          case 'p':
            txt_board[file][rank] = 'p';
            break;
          case 'n':
            txt_board[file][rank] = 'n';
            break;
          case 'b':
            txt_board[file][rank] = 'b';
            break;
          case 'r':
            txt_board[file][rank] = 'r';
            break;
          case 'q':
            txt_board[file][rank] = 'q';
            break;
          case 'k':
            txt_board[file][rank] = 'k';
            break;
          case 'P':
            txt_board[file][rank] = 'P';
            break;
          case 'N':
            txt_board[file][rank] = 'N';
            break;
          case 'B':
            txt_board[file][rank] = 'B';
            break;
          case 'R':
            txt_board[file][rank] = 'R';
            break;
          case 'Q':
            txt_board[file][rank] = 'Q';
            break;
          case 'K':
            txt_board[file][rank] = 'K';
            break;

          case '/':
            file--; // slash is beginning of next rank
            break;
          case'1':
            txt_board[file][rank] = '.';
            break;
          case '2':
            txt_board[file][rank] = '.';
            txt_board[file+1][rank] = '.';
            file++;
            break;
          case '3':
            txt_board[file][rank] = '.';
            txt_board[file+1][rank] = '.';
            txt_board[file+2][rank] = '.';
            file +=2;
            break;
          case'4':
            txt_board[file][rank] = '.';
            txt_board[file+1][rank] = '.';
            txt_board[file+2][rank] = '.';
            txt_board[file+3][rank] = '.';
            file+=3;
            break;
          case '5':
            txt_board[file][rank] = '.';
            txt_board[file+1][rank] = '.';
            txt_board[file+2][rank] = '.';
            txt_board[file+3][rank] = '.';
            txt_board[file+4][rank] = '.';
            file+=4;
            break;
          case '6':
            txt_board[file][rank] = '.';
            txt_board[file+1][rank] = '.';
            txt_board[file+2][rank] = '.';
            txt_board[file+3][rank] = '.';
            txt_board[file+4][rank] = '.';
            txt_board[file+5][rank] = '.';
            file+=5;
            break;
          case '7':
            txt_board[file][rank] = '.';
            txt_board[file+1][rank] = '.';
            txt_board[file+2][rank] = '.';
            txt_board[file+3][rank] = '.';
            txt_board[file+4][rank] = '.';
            txt_board[file+5][rank] = '.';
            txt_board[file+6][rank] = '.';
            file+=6;
            break;
          case '8':
            txt_board[file][rank] = '.';
            txt_board[file+1][rank] = '.';
            txt_board[file+2][rank] = '.';
            txt_board[file+3][rank] = '.';
            txt_board[file+4][rank] = '.';
            txt_board[file+5][rank] = '.';
            txt_board[file+6][rank] = '.';
            txt_board[file+7][rank] = '.';
            file +=7;
            break;
        }
      }
    }
    return;
  }

  bool isntCheck(Brain::Action action, char color, const char board[8][8]) {
    char board_copy[8][8];
    char king_char = color == 'w' ? 'K' : 'k';
    char king_file = 'I'; // invalid values
    int king_rank = 9;    // so i know if i never find the king
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        board_copy[i][j] = board[i][j];
      }
    }
    /* ---- APPLY ACTION ---- */
    board_copy[action.file()-65][action.rank()-1] = board_copy[action.last_file()-65][action.last_rank()-1];
    board_copy[action.last_file()-65][action.last_rank()-1] = '.';
    /* ------- END APPLY ---- */

    for (int f = 0; f < 8; f++)
    {
      for (int r = 0; r < 8; r++)
      {
        if (board_copy[f][r] == king_char)
        {
          king_file = char(f+65);
          king_rank = r+1;
        }
      }
    }
    return !threat(color, king_file, king_rank, board_copy);
  }

}