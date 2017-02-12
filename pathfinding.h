/*********************************************************
          File Name: pathfinding.h
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:51:22 AM CST
**********************************************************/

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "square.h"
#include <functional>

using PointPtrList = std::list<Point*>;
using PointList = std::list<Point>;
using Callback = std::function<Point*()>;

class PathFinding
{
  public:
    PathFinding(int w, int h);

    ~PathFinding();

    void clear();

    PathFinding& reset(std::list<Square *> &snake, bool including_tail = true);

    PointList findBest(Square* start, Square* end);

    PointList findWorst(Square* start, Square* end);

  private:
    PointPtrList openList;
    PointPtrList closeList;
    PointList blockList;
    PointList path;
    const int width;
    const int height;

    bool isInList(PointPtrList& l, const Point& point);

    bool isBlock(const Point &point);

    Point* minF(PointPtrList& l);

    Point* maxF(PointPtrList& l);

    const PointList& find(Point* start, Point* target, Callback cb);

    void setUpNewPoint(const Point& newp, Point* oldp, Point* target, float cost);

    void addToOpenList(Point* point, Point *target);

    void safe_clear(PointPtrList& l);
};

#endif // PATHFINDING_H
