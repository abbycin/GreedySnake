/*********************************************************
          File Name: mainwindow.cpp
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:30:29 AM CST
**********************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <thread>
#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  kSize(20), // 20 row 20 column
  gen{rd()},
  is_manual(true),
  aStar(kSize, kSize),
  timer(new QTimer(this))
{
  ui->setupUi(this);
  this->init();
}

MainWindow::~MainWindow()
{
  delete ui;
  timer->stop();
  delete timer;
  for(auto& x: world)
  {
    if(x)
    {
      delete x;
      x = nullptr;
    }
  }
  world.clear();
  snake.clear();
  food = nullptr;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
  if(!is_manual)
  {
    e->accept();
    return;
  }
  switch(e->key())
  {
    case Qt::Key_W:
      moveUp();
      break;
    case Qt::Key_S:
      moveDown();
      break;
    case Qt::Key_A:
      moveLeft();
      break;
    case Qt::Key_D:
      moveRight();
      break;
    default:
      break;
  }
}

void MainWindow::init()
{
  // initialize Window
  this->setFixedSize(this->width(), this->height());
  // initialize GridView
  ui->playground->setSpacing(0);
  ui->playground->setMargin(1);
  Square* b = nullptr;
  for(int i = 0; i < kSize; ++i)
  {
    for(int j = 0; j < kSize; ++j)
    {
      b = new Square{i, j};
      world.push_back(b);
      ui->playground->addWidget(b->label(), i, j);
    }
  }
  // initialize snake head and food
  this->setUpFoodorHead(Square::Id::Food);
  this->setUpFoodorHead(Square::Id::Head);
  // setup signals and slots
  timer->setInterval(500);
  connect(timer, &QTimer::timeout, this, &MainWindow::autoMove);
}

void MainWindow::setUpFoodorHead(Square::Id id)
{
  int step = genRand(world.size() - 1);
  bool tried = false;
  for(auto iter = world.begin(); iter != world.end(); ++iter)
  {
    auto inner = iter;
    while(step--)
      ++inner;
    for(; !tried && inner != world.end(); ++iter)
    {
      if((*inner)->get_id() == Square::Id::None)
      {
        if(id == Square::Id::Food)
        {
          food = *inner;
        }
        else if(id == Square::Id::Head)
        {
          snake.push_back(*inner);
        }
        (*inner)->set_id(id).update_style();
        return;
      }
      tried = true;
    }
    if((*iter)->get_id() == Square::Id::None)
    {
      if(id == Square::Id::Food)
      {
        food = *iter;
      }
      else if(id == Square::Id::Head)
      {
        snake.push_back(*iter);
      }
      (*iter)->set_id(id).update_style();
      return;
    }
  }
}

int MainWindow::genRand(int n)
{
  std::uniform_int_distribution<> dis(0, n - 1);
  return dis(gen);
}

void MainWindow::on_btnRestart_clicked()
{
    for(auto iter = world.begin(); iter != world.end(); ++iter)
    {
      (*iter)->set_id(Square::Id::None).update_style();
      (*iter)->label()->clear();
    }
    snake.clear();
    this->setUpFoodorHead(Square::Id::Food);
    this->setUpFoodorHead(Square::Id::Head);
}

bool MainWindow::isReachFood(Square *head)
{
  if(head->equal(*food))
    return true;
  return false;
}

Square* MainWindow::findPoint(const Point &p)
{
  for(auto iter = world.begin(); iter != world.end(); ++iter)
  {
    if((*iter)->get_point().equal(p))
      return *iter;
  }
  return nullptr;
}

/*
 *  ---------> y
 *  |
 *  |
 *  |
 *  x
 */

void MainWindow::moveAround(const Point& target)
{
  Square* target_square = this->findPoint(target);
  if(target_square == nullptr)
  {
    emit game_over();
    return;
  }
  target_square->set_id(Square::Id::Head).update_style();
  for(auto& x: snake)
  {
    x->set_id(Square::Id::Snake).update_style();
  }
  //qDebug() << "(" << target.x << ", " << target.y << ")";
  snake.push_front(target_square);
  if(isReachFood(target_square))
  {
    setUpFoodorHead(Square::Id::Food);
  }
  else
  {
    snake.back()->set_id(Square::Id::None).update_style();
    snake.pop_back();
  }
}

void MainWindow::moveUp()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x - 1, cur_pos.y};
  moveAround(target);
}

void MainWindow::moveDown()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x + 1, cur_pos.y};
  moveAround(target);
}

void MainWindow::moveLeft()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x, cur_pos.y - 1};
  moveAround(target);
}

void MainWindow::moveRight()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x, cur_pos.y + 1};
  moveAround(target);
}

void MainWindow::autoMove()
{
  timer->stop();
  auto& path = aStar.reset(snake).find(snake.front(), food);
  /*
  for(auto iter = path.rbegin(); iter != path.rend(); ++iter)
  {
    moveAround(*iter);
  }
  */
  // only go first step
  auto iter = path.rbegin();
  ++iter;
  moveAround(*iter);
  timer->start();
}

void MainWindow::on_btnAutoManual_clicked()
{
  if(is_manual)
  {
    ui->btnAutoManual->setText("Manual");
    timer->start();
  }
  else
  {
    ui->btnAutoManual->setText("Auto");
    timer->stop();
  }
  is_manual = !is_manual;
}
