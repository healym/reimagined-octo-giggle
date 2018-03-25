// AI
// This is where you build your AI

#include "ai.hpp"

// <<-- Creer-Merge: includes -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
// You can add #includes here for your AI.
#include "brain/search.hpp"
#include "brain/algorithms.hpp"
// <<-- /Creer-Merge: includes -->>

namespace cpp_client
{

namespace chess
{
Brain::Chessboard c_board;
/// <summary>
/// This returns your AI's name to the game server.
/// Replace the string name.
/// </summary>
/// <returns>The name of your AI.</returns>
std::string AI::get_name() const
{
    // <<-- Creer-Merge: get-name -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    // REPLACE WITH YOUR TEAM NAME!
    return "letting_you_win_on_purpose";
    // <<-- /Creer-Merge: get-name -->>
}

/// <summary>
/// This is automatically called when the game first starts, once the game objects are created
/// </summary>
void AI::start()
{
    // <<-- Creer-Merge: start -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    // This is a good place to initialize any variables

    // <<-- /Creer-Merge: start -->>
}

/// <summary>
/// This is automatically called the game (or anything in it) updates
/// </summary>
void AI::game_updated()
{
    // <<-- Creer-Merge: game-updated -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    // If a function you call triggers an update this will be called before it returns.
    // <<-- /Creer-Merge: game-updated -->>
}

/// <summary>
/// This is automatically called when the game ends.
/// </summary>
/// <param name="won">true if you won, false otherwise</param>
/// <param name="reason">An explanation for why you either won or lost</param>
void AI::ended(bool won, const std::string& reason)
{
    //<<-- Creer-Merge: ended -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
    // You can do any cleanup of your AI here.  The program ends when this function returns.
    //<<-- /Creer-Merge: ended -->>
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{
  // <<-- Creer-Merge: runTurn -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
  // Put your game logic here for run_turn here
  // Here is where you'll want to code your AI.
  vector<Brain::Action> actions;
  vector<Brain::Action> randomPieceMoves;
  Brain::GameInfo info = Brain::info_from_fen(game->fen);
  c_board.start_pos(game->fen);
  int depth = get_setting("depth_limit") != "" ? stoi(get_setting("depth_limit")) : 2;

  cout << "\n         +-----------------+" << endl;
  for (int i = 7; i >= 0; i--)
  {
    cout << "       " << i + 1 << " | "
    << c_board.txt_board[0][i] << " "
    << c_board.txt_board[1][i] << " "
    << c_board.txt_board[2][i] << " "
    << c_board.txt_board[3][i] << " "
    << c_board.txt_board[4][i] << " "
    << c_board.txt_board[5][i] << " "
    << c_board.txt_board[6][i] << " "
    << c_board.txt_board[7][i] << " |"
    << endl;
  }
  cout << "         +-----------------+\n           a b c d e f g h" << endl;

  chess::Move last_move = game->moves.size() >= 2 ? game->moves[game->moves.size() - 2] : nullptr;
  string last_move_string = last_move != nullptr ? last_move->san : ""; 
  chess::Move next_last_move = game->moves.size() >= 4 ? game->moves[game->moves.size() - 4] : nullptr;
  string next_last_move_string = next_last_move != nullptr ? next_last_move->san : "";
  chess::Move third_move = game->moves.size() >= 6 ? game->moves[game->moves.size() - 6] : nullptr;
  string third_move_string = third_move != nullptr ? third_move->san : "";
  std::vector<Brain::Action> moves;
  moves.clear();
  Brain::Action action;
  if (last_move_string != "") { // load moves vector if starting state isn't standard
    moves.push_back(Brain::Action(last_move_string.substr(0,1), last_move_string.substr(1,1), last_move_string.substr(2,1)));
    if (next_last_move_string != "") {
      moves.push_back(Brain::Action(next_last_move_string.substr(0,1), next_last_move_string.substr(1,1), next_last_move_string.substr(2)));
      if (third_move_string != "") {
          moves.push_back(Brain::Action(third_move_string.substr(0,1), third_move_string.substr(1,1), third_move_string.substr(2,1)));
      }
    }
  }
  string color = player->color == "White" ? "Black" : "White"; // first color gets inverted for move generation
  Node root(player->color, moves, c_board);
  action = Search::IteratingDepthDepthLimitedMiniMax(root, depth);
  cout << "= [ LEGAL MOVES FOR PIECE "
       << action.piece() << " at "
       << string(1, action.last_file())
       << action.last_rank()
       << "] ="
       << endl;
  switch(action.piece()[0])
  {
    case 'P':
    case 'p':
        randomPieceMoves = Brain::Pawn::Actions(action.last_rank(),
                                                action.last_file(),
                                                tolower(player->color[0]),
                                                c_board.txt_board,
                                                last_move_string,
                                                next_last_move_string);
        break;

    case 'N':
    case 'n':
        randomPieceMoves = Brain::Knight::Actions(action.last_rank(),
                                                action.last_file(),
                                                tolower(player->color[0]),
                                                c_board.txt_board);
        break;

    case 'B':
    case 'b':
        randomPieceMoves = Brain::Bishop::Actions(action.last_rank(),
                                                action.last_file(),
                                                tolower(player->color[0]),
                                                c_board.txt_board);
        break;

    case 'R':
    case 'r':
        randomPieceMoves = Brain::Rook::Actions(action.last_rank(),
                                                action.last_file(),
                                                tolower(player->color[0]),
                                                c_board.txt_board);
        break;
    
    case 'Q':
    case 'q':
        randomPieceMoves = Brain::Queen::Actions(action.last_rank(),
                                                action.last_file(),
                                                tolower(player->color[0]),
                                                c_board.txt_board);
        break;
    
    case 'K':
    case 'k':
        randomPieceMoves = Brain::King::Actions(action.last_rank(),
                                                action.last_file(),
                                                tolower(player->color[0]),
                                                c_board.txt_board);
        break;
  }

  for (Brain::Action action : randomPieceMoves)
  {
      cout << "::::::            "
      << action.piece() << string(1, action.file()) << action.rank() << "         ::::::"
      << endl;
  }
  Piece piece_to_move = nullptr;

  for( auto piece : game->pieces) {
    if(piece->file == string(1, tolower(action.last_file())) && piece->rank == action.last_rank()) {
      piece_to_move = piece;
      break;
    }
  }

  string file = string(1, tolower(action.file()));
  int rank = action.rank();
  string promote = action.promote();
  piece_to_move->move(file, rank, promote);
  cout << "       =<< MOVING : " << action.piece() << string(1,action.file()) << action.rank() << " >>=" << endl;

  return true; // to signify we are done with our turn.
  // <<-- /Creer-Merge: runTurn -->>
}

//<<-- Creer-Merge: methods -->> - Code you add between this comment and the end comment will be preserved between Creer re-runs.
// You can add additional methods here for your AI to call
//<<-- /Creer-Merge: methods -->>

} // chess

} // cpp_client
