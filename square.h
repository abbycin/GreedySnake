/*********************************************************
          File Name: square.h
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:33:16 AM CST
**********************************************************/

#ifndef SQUARE_H
#define SQUARE_H

#include <QLabel>

class Point
{
  public:
  float g;
  float h;
  int x;
  int y;
  Point* prev;

  Point();

  ~Point() {}

  Point(int x_, int y_);

  Point(const Point& rhs);

  const Point& operator= (const Point& rhs);

  bool equal(const Point& rhs) const;

  float get_f() const;

  float manHattanDistance(const Point& target) const;
};

bool operator< (const Point& lhs, const Point& rhs);

class Square
{
  public:
    enum struct Id : int
    {
      Snake = 0,
      Food,
      None,
      Head,
    };

    Square(int, int);
    ~Square();

    void update_style();

    bool equal(const Square& rhs) const;

    float get_f() const;

    const Point& get_point() const;

    float get_g() const;

    float get_h() const;

    void set_pos(int x, int y);

    void set_pos(const Point& pos);

    void set_g(float n);

    void set_h(const Square& rhs);

    void set_h(const Point& point);

    Square& set_id(Id id);

    Id get_id() const;

    QLabel* label();

  private:
    QLabel* label_;
    Point pos_;
    Id id_;
};

#endif // PATH_FINDING_H
