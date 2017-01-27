/*********************************************************
          File Name: pathfinding.h
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:51:22 AM CST
**********************************************************/

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "square.h"

using PointPtrList = std::list<Point*>;
using PointList = std::list<Point>;

class PathFinding
{
  public:
    enum struct Id : int {
      G = 0,
      H,
      F
    };

    PathFinding(int w, int h);

    ~PathFinding();

    void clear();

    PathFinding& reset(std::list<Square *> &snake);

    const PointList &find(Square* start, Square* end);

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

//    bool removeFromList(PointList& l, Point* point);

    void setUpNewPoint(const Point& newp, Point* oldp, Point* target);

    bool addToOpenList(Point* point, Point *target);

    void safe_clear(PointPtrList& l);
};

#endif // PATHFINDING_H
