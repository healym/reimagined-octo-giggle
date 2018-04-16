#ifndef ALGO_HPP
#define ALGO_HPP
#include <ctime>
#include <map>
#include "search.hpp"

namespace Search {
  Brain::Action TLHTQSIDABDLMM(Node root, 
                               int turn,
                               double time_left,
                               int alpha,
                               int beta,
                               int qlimit,
                               int limit);
  Brain::Action TLHTQSABDLMM(Node root,
                             double time_alloted,
                             time_t start,
                             int alpha,
                             int beta,
                             int qlimit,
                             int limit);
  int TLHTQSMaxValue(Node root,
                     double time_alloted,
                     time_t start,
                     int alpha,
                     int beta,
                     int qlimit,
                     int limit,
                     std::map<string, int>& hist);
  int TLHTQSMinValue(Node root,
                     double time_alloted,
                     time_t start,
                     int alpha,
                     int beta,
                     int qlimit,
                     int limit,
                     std::map<string, int>& hist);

  bool Quiescent(Node root);

  class timeException : public exception {
  public:
    timeException(string mesg): _msg(mesg) {};
    string msg() { return _msg; };
  private:
    string _msg;
  };
}
#endif