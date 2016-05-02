#ifndef FLOOD_FILL_H_
#define FLOOD_FILL_H_
#include <unordered_set>
#include "boost/functional/hash.hpp"
namespace algorithm
{
typedef std::pair<int, int> NodeCoordinates;

template<class Node> class FloodFill
{
private:
  typedef std::unordered_set<NodeCoordinates, boost::hash<NodeCoordinates>> NodeSet;

  Node node_;
  NodeSet set_;
  NodeCoordinates coords_;
  bool horizontal_;
  bool vertical_;

  auto Fill() -> void
  {
    if(set_.insert(coords_).second)
    {
      if(node_(coords_))
      {
        if(horizontal_)
        {
          coords_.first -= 1; Fill();
          coords_.first += 2; Fill();
          coords_.first -= 1;
        }
        if(vertical_)
        {
          coords_.second += 1; Fill();
          coords_.second -= 2; Fill();
          coords_.second += 1;
        }
      }
    }
  }

public:
  FloodFill::FloodFill(Node&& node, bool horizontal, bool vertical) : node_(std::move(node)), coords_(0, 0), horizontal_(horizontal), vertical_(vertical)
  {
    Fill();
  }
};
}
#endif