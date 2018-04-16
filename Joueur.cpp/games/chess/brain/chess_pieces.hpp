#ifndef CHESS_PIECES_HPP
#define CHESS_PIECES_HPP

#include <tuple>
#include <set>
#include <vector>
#include <string>

using namespace std;

namespace Brain {
class Action {
public:
  //Action(const Action& other);
  Action(): m_piece("P"), m_lfile('e'), m_lrank(2), m_file('e'), m_rank(4), m_promote("") {};
  Action(string piece, string file, string rank): m_piece(piece), m_file(file[0]), m_rank(rank[0]-'0') {};
  /// <summary>
  /// Constructs an Action
  /// </summary>
  /// @param string piece the type of peice being passed, as an uppercase single-character string
  /// @param char input_lfile the current file of the piece to be moved
  /// @param int input_lrank the current rank of the piece to be moved
  /// @param char input_file the target file
  /// @param int input_rank the target rank
  Action(string piece, char input_lfile, int input_lrank, char input_file, int input_rank, bool capt=false):
      capture(capt),
      m_piece(piece),
      m_lrank(input_lrank),
      m_lfile(input_lfile),
      m_rank(input_rank),
      m_file(input_file),
      m_promote("") { };
  
  /// <summary>
  /// Constructs an Action
  /// </summary>
  /// @params string piece: the type of peice being passed, as an uppercase single-character string
  /// @params char input_lfile: the current file of the piece to be moved
  /// @params int input_lrank: the current rank of the piece to be moved
  /// @params char input_file: the target file
  /// @params int input_rank: the target rank
  /// @params string in_promote: the piece that a Pawn is promoting to
  Action(string piece, char input_lfile, int input_lrank, char input_file, int input_rank, string in_promote, bool capt=false):
      m_piece(piece),
      m_lrank(input_lrank),
      m_lfile(input_lfile),
      m_rank(input_rank),
      m_file(input_file),
      m_promote(in_promote),
      capture(capt) {};
  bool capture;
  std::string piece() const { return m_piece; };
  void piece_set(string piece) { m_piece = piece; };
  int rank() const { return m_rank; };
  int last_rank() const { return m_lrank; };
  char file() const { return m_file; };
  char last_file() const { return m_lfile; };
  string promote() const { return m_promote; };
  bool operator<(const Brain::Action &rhs);
  bool operator==(const Brain::Action &rhs) const;
  std::string String() const { return std::string(1, m_lfile)
      + to_string(m_lrank)
      + std::string(1, m_file)
      + to_string(m_rank); };
private:
  std::string m_piece;
  int m_lrank;
  char m_lfile;
  int m_rank;
  char m_file;
  std::string m_promote;
};

class Piece {
  /// <summary>
  /// returns the actions a piece can take (returns none for a generic piece)
  /// </summary>
  vector<Action> Actions() { vector<Action> actions; return actions;}
};

class GameInfo {
public:
  /// <summary>
  /// Constructor for GameInfo from FEN String
  /// </summary>
  /// @params char player: the color of the player
  /// @params string castling: the field of the FEN that includes castling information
  /// @params int halfmove: the half-move clock field of a FEN string
  /// @params int fullmove: the number of moves taken since the start of the game
  GameInfo(): m_player('w'), m_castling("KQkq"), m_enpassant("wut"), m_halfmove(0), m_fullmove(0) {};
  GameInfo(char player, string castling, string enpassant, int halfmove, int fullmove):
    m_player(player),
    m_castling(castling),
    m_enpassant(enpassant),
    m_halfmove(halfmove),
    m_fullmove(fullmove) {};
  char player() { return m_player; };
  string castling() { return m_castling; };
  string enpassant() { return m_enpassant; };
  int halfmove() { return m_halfmove; };
  int fullmove() { return m_fullmove; };
private:
  char m_player;
  string m_castling;
  string m_enpassant;
  int m_halfmove;
  int m_fullmove;
};

class Chessboard {
public:
  /// <summary>
  /// default constructor for chessboard
  /// </summary>
  Chessboard();
  Chessboard(char board[8][8]);
  char txt_board[8][8] = {
        {'R','P','.','.','.','.','p','r'},
        {'N','P','.','.','.','.','p','n'},
        {'B','P','.','.','.','.','p','b'},
        {'Q','P','.','.','.','.','p','q'},
        {'K','P','.','.','.','.','p','k'},
        {'B','P','.','.','.','.','p','b'},
        {'N','P','.','.','.','.','p','n'},
        {'R','P','.','.','.','.','p','r'}
    };
  /// <summary>
  /// returns a vector of actions that can be taken by all pieces in the given chessboard
  /// <summary>
  /// @params string player: the color of the player
  /// @params string last_move: the last move taken, represented in SAN
  /// @params string next_last_move: the next to last move taken, represented in SAN
  /// @params GameInfo &castle: info extracted from the FEN string
  vector<Action> Actions(string player, string last_move, string next_last_move, std::vector<Brain::Action> moves, GameInfo &castle) const;
  Brain::GameInfo update(const string &fen);
  void start_pos(const string &fen);
  bool operator==(Brain::Chessboard& other);
};

class Castle : public Piece {
public:
  /// <summary>
  /// returns a vector of actions that can be taken by a King
  /// <summary>
  /// @param Brain::GameInfo castle info from FEN string about what castling is possible
  /// @param const char board[8][8]: the chessboard, as an 8x8 2d array of chars -- first param is file, second is rank
  static vector<Action> Actions(char color, Brain::GameInfo castle, const char board[8][8]);
};

class Pawn : public Piece {
public:
/// <summary>
  /// returns a vector of actions that can be taken by a Pawn
  /// <summary>
  /// @params int rank: the rank of the piece (1-indexed)
  /// @params char file: the file of the piece
  /// @params char color: the color of the player, represented as either 'w' or 'b'
  /// @params const char board[8][8]: the chessboard, as an 8x8 2d array of chars -- first param is file, second is rank
  static vector<Action> Actions(int rank, char file, char color, const char board[8][8],
                             string last_move, string next_last_move);
  bool made_big_move();
private:
  bool big_move;
};

class Knight : public Piece {
public:
  /// <summary>
  /// returns a vector of actions that can be taken by a Knight
  /// <summary>
  /// @params int rank: the rank of the piece (1-indexed)
  /// @params char file: the file of the piece
  /// @params char color: the color of the player, represented as either 'w' or 'b'
  /// @params const char board[8][8]: the chessboard, as an 8x8 2d array of chars -- first param is file, second is rank
  static vector<Action> Actions(int rank, char file, char color, const char board[8][8]);
};

class Bishop : public Piece {
public:
  /// <summary>
  /// returns a vector of actions that can be taken by a Bishop
  /// <summary>
  /// @params int rank: the rank of the piece (1-indexed)
  /// @params char file: the file of the piece
  /// @params char color: the color of the player, represented as either 'w' or 'b'
  /// @params const char board[8][8]: the chessboard, as an 8x8 2d array of chars -- first param is file, second is rank
  static vector<Action> Actions(int rank, char file, char color, const char board[8][8]);
};

class Rook : public Piece {
public:
  /// <summary>
  /// returns a vector of actions that can be taken by a Rook
  /// <summary>
  /// @params int rank: the rank of the piece (1-indexed)
  /// @params char file: the file of the piece
  /// @params char color: the color of the player, represented as either 'w' or 'b'
  /// @params const char board[8][8]: the chessboard, as an 8x8 2d array of chars -- first param is file, second is rank
  static vector<Action> Actions(int rank, char file, char color, const char board[8][8]);
};

class Queen : public Piece {
public:
  /// <summary>
  /// returns a vector of actions that can be taken by a Queen
  /// <summary>
  /// @params int rank: the rank of the piece (1-indexed)
  /// @params char file: the file of the piece
  /// @params char color: the color of the player, represented as either 'w' or 'b'
  /// @params const char board[8][8]: the chessboard, as an 8x8 2d array of chars -- first param is file, second is rank
  static vector<Action> Actions(int rank, char file, char color, const char board[8][8]);
};

class King : public Piece {
public:
  /// <summary>
  /// returns a vector of actions that can be taken by a King
  /// <summary>
  /// @params int rank: the rank of the piece (1-indexed)
  /// @params char file: the file of the piece
  /// @params char color: the color of the player, represented as either 'w' or 'b'
  /// @params const char board[8][8]: the chessboard, as an 8x8 2d array of chars -- first param is file, second is rank
  static vector<Action> Actions(int rank, char file, char color, const char board[8][8]);
};

/// <summary>
/// returns whether a given square is under threat by another piece
/// <summary>
bool threat(char color, char file, int rank, const char board[8][8]);

/// <summary>
/// extracts info from given FEN string
/// </summmary>
Brain::GameInfo info_from_fen(const string &fen);

/// <summary>
/// checks whether taking a particular action will result in the player's king being in check
/// </summary>
/// @params Brain::Action action: the action to be taken
/// @params char color: the color of the player taking an action
/// @params const char board[8][8]: the current state of the chessboard
bool isntCheck(Brain::Action action, char color, const char board[8][8]);

}

#endif