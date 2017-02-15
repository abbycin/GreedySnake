/*********************************************************
          File Name: square.cpp
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:33:16 AM CST
**********************************************************/

#include "square.h"
#include <cmath>

Point::Point()
  : g(0.0), h(0.0),
    x(0), y(0),
    prev(nullptr)
{}

Point::Point(int x_, int y_)
  : Point()
{
  x = x_;
  y = y_;
}

Point::Point(const Point &rhs)
  : Point()
{
  *this = rhs;
}

const Point& Point::operator= (const Point& rhs)
{
  if(this == &rhs)
    return *this;
  x = rhs.x;
  y = rhs.y;
  g = 0.0;
  h = 0.0;
  prev = nullptr;
  return *this;
}

bool Point::equal(const Point& rhs) const
{
  return (x == rhs.x && y == rhs.y);
}

float Point::get_f() const
{
  return g + h;
}

float Point::manHattanDistance(const Point &target) const
{
  return std::fabs(x - target.x) + std::fabs(y - target.y);
}

bool operator< (const Point& lhs, const Point& rhs)
{
  return (lhs.x < rhs.x && lhs.y < rhs.y);
}

Square::Square(int x_, int y_)
  : label_(new QLabel),
    pos_(x_, y_),
    id_(Id::None)
{
  label_->setFixedSize(27, 22);
  label_->setAlignment(Qt::AlignCenter);
  this->update_style();
}

Square::~Square()
{
  if(label_ != nullptr)
    delete label_;
}

void Square::update_style()
{
  switch (id_) {
    case Id::None:
      label_->setStyleSheet("background-color: white");
      break;
    case Id::Snake:
      label_->setStyleSheet("background-color: red");
      break;
    case Id::Food:
      label_->setStyleSheet("background-color: black");
      break;
    case Id::Head:
      label_->setStyleSheet("background-color: green");
      break;
  }
}

bool Square::equal(const Square &rhs) const
{
  return pos_.equal(rhs.pos_);
}

float Square::get_f() const
{
  return pos_.get_f();
}

const Point& Square::get_point() const
{
  return pos_;
}

float Square::get_g() const
{
  return pos_.g;
}

void Square::set_pos(int x, int y)
{
  pos_.x = x;
  pos_.y = y;
}

void Square::set_pos(const Point &pos)
{
  pos_ = pos;
}

void Square::set_g(float g)
{
  pos_.g = g;
}

void Square::set_h(const Square &rhs)
{
  pos_.h = pos_.manHattanDistance(rhs.get_point());
}

void Square::set_h(const Point &point)
{
  pos_.h = pos_.manHattanDistance(point);
}

Square& Square::set_id(Id id)
{
  id_ = id;
  return *this;
}

Square::Id Square::get_id() const
{
  return id_;
}

QLabel* Square::label()
{
  return label_;
}
