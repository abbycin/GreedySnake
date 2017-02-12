/*********************************************************
          File Name: pathfinding.cpp
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:51:23 AM CST
**********************************************************/

#include "pathfinding.h"
#include <QDebug>

PathFinding::PathFinding(int w, int h)
  : openList(),
    closeList(),
    blockList(),
    path(),
    width(w), height(h)
{
}

PathFinding::~PathFinding()
{
  this->clear();
}

void PathFinding::clear()
{
  this->safe_clear(openList);
  this->safe_clear(closeList);
  blockList.clear();
  path.clear();
}

PathFinding& PathFinding::reset(std::list<Square *>& snake, bool including_tail)
{
  this->clear();
  for(auto iter = snake.begin(); iter != snake.end(); ++iter)
  {
    blockList.push_back((*iter)->get_point());
  }
  if(!including_tail)
    blockList.pop_back();
  return *this;
}

PointList PathFinding::findBest(Square *start, Square *end)
{
  auto target = new Point{end->get_point()};
  auto beg = new Point{start->get_point()};
  this->find(beg, target, [this] {
    return minF(openList);
  });
  delete target;
  return path;
}

PointList PathFinding::findWorst(Square *start, Square *end)
{
  auto beg = new Point{start->get_point()};
  auto target = new Point{end->get_point()};
  this->find(beg, target, [this] {
    return maxF(openList);
  });
  delete target;
  return path;
}

bool PathFinding::isInList(PointPtrList &l, const Point& point)
{
  for(auto iter = l.begin(); iter != l.end(); ++iter)
  {
    if((*iter)->equal(point))
      return true;
  }
  return false;
}

bool PathFinding::isBlock(const Point& point)
{
  if(point.x < 0 || point.y < 0)
    return true;
  if(point.x >= width || point.y >= height)
    return true;
  for(auto iter = blockList.begin(); iter != blockList.end(); ++iter)
  {
    if(iter->equal(point))
      return true;
  }
  return false;
}

Point* PathFinding::minF(PointPtrList &l)
{
  if(l.empty())
    return nullptr;
  l.sort([](const Point* lhs, const Point* rhs) {
    return lhs->get_f() < rhs->get_f();
  });
  auto iter = l.front();
  closeList.push_back(iter);
  l.pop_front();
  return iter;
}

Point* PathFinding::maxF(PointPtrList &l)
{
  if(l.empty())
    return nullptr;
  l.sort([](const Point* lhs, const Point* rhs) {
    return lhs->get_f() > rhs->get_f(); // descending order
  });
  auto iter = l.front();
  closeList.push_back(iter);
  l.pop_front();
  return iter;
}

const PointList& PathFinding::find(Point *start, Point *target, Callback cb)
{
  start->h = start->manHattanDistance(*target);
  closeList.push_back(start);
  addToOpenList(start, target);
  Point* next = nullptr;
  for(;;)
  {
    next = cb();
    if(next == nullptr)
    {
      break;
    }
    if(next->equal(*target)) // found in closeList
      break;
    addToOpenList(next, target);
  }
  if(next == nullptr)
    return path;

  next = closeList.back();
  while(next != nullptr)
  {
    path.push_back(*next);
    next = next->prev;
  }
  path.pop_back(); // pop head
  std::reverse(path.begin(), path.end());
  return path;
}

void PathFinding::setUpNewPoint(const Point& newp, Point *oldp, Point *target, float cost)
{
  if(isInList(closeList, newp) || isBlock(newp))
  {
    return;
  }
  float h = newp.manHattanDistance(*target);
  float f = oldp->g + cost + h;
  bool is_in_list = false;
  // if it's already in openList and it has shorter path (small f)
  // then update its f
  for(auto iter = openList.begin(); iter != openList.end(); ++iter)
  {
    if((*iter)->equal(newp))
    {
      is_in_list = true;
      if(f < oldp->get_f())
      {
        (*iter)->g = oldp->g + cost;
        (*iter)->h = h;
        (*iter)->prev = oldp;
        return;
      }
    }
  }
  // if it's not in openList(set up f and push to openList)
  // or not better(let it unchanged)
  if(!is_in_list)
  {
    Point* node = new Point{newp.x, newp.y};
    node->g = oldp->g + cost;
    node->h = h;
    node->prev = oldp;
    openList.push_back(node);
  }
}

void PathFinding::addToOpenList(Point *point, Point* target)
{
  // Up
  Point up{point->x - 1, point->y};
  setUpNewPoint(up, point, target, 1);
  // Down
  Point down{point->x + 1, point->y};
  setUpNewPoint(down, point, target, 1);
  // Left
  Point left{point->x, point->y - 1};
  setUpNewPoint(left, point, target, 1);
  // Right
  Point right{point->x, point->y + 1};
  setUpNewPoint(right, point, target, 1);
}

void PathFinding::safe_clear(PointPtrList &l)
{
  for(auto& x: l)
  {
    if(x != nullptr)
    {
      delete x;
      x = nullptr;
    }
  }
  l.clear();
}
