/*********************************************************
          File Name: pathfinding.cpp
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:51:23 AM CST
**********************************************************/

#include "pathfinding.h"

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

PathFinding& PathFinding::reset(std::list<Square *>& snake)
{
  this->clear();
  for(auto iter = snake.begin(); iter != snake.end(); ++iter)
  {
    blockList.push_back((*iter)->get_point());
  }
  return *this;
}

const PointList& PathFinding::find(Square *start, Square *end)
{
  auto target = new Point{end->get_point()};
  auto beg = new Point{start->get_point()};
  beg->h = beg->manHattanDistance(*target);
  closeList.push_back(beg);
  addToOpenList(beg, target);
  Point* next = nullptr;
  bool found = true;
  for(;;)
  {
    next = minF(openList);
    if(next == nullptr)
    {
      found = false;
      break;
    }
    if(next->equal(*target)) // found in closeList
      break;
    addToOpenList(next, target);
  }
  delete target;
  if(!found)
    return path;

  next = closeList.back();
  while(next != nullptr)
  {
    path.push_back(*next);
    next = next->prev;
  }
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
  float minimum = 9999999999.0f;
  Point* res = nullptr;
  PointPtrList::iterator found;
  for(auto iter = l.begin(); iter != l.end(); ++iter)
  {
    if((*iter)->get_f() < minimum)
    {
      minimum = (*iter)->get_f();
      res = *iter;
      found = iter;
    }
  }
  if(res != nullptr)
  {
    openList.erase(found);
    closeList.push_back(res);
  }
  return res;
}

void PathFinding::setUpNewPoint(const Point& newp, Point *oldp, Point *target)
{
  if(isInList(closeList, newp) || isBlock(newp))
  {
    return;
  }
  float h = newp.manHattanDistance(*target);
  float f = oldp->g + 1 + h;
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
        (*iter)->g = oldp->g + 1;
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
    node->g = oldp->g + 1;
    node->h = h;
    node->prev = oldp;
    openList.push_back(node);
  }
}

bool PathFinding::addToOpenList(Point *point, Point* target)
{
  // Up
  Point up{point->x - 1, point->y};
  setUpNewPoint(up, point, target);
  // Down
  Point down{point->x + 1, point->y};
  setUpNewPoint(down, point, target);
  // Left
  Point left{point->x, point->y - 1};
  setUpNewPoint(left, point, target);
  // Right
  Point right{point->x, point->y + 1};
  setUpNewPoint(right, point, target);
  return true;
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
