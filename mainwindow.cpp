/*********************************************************
          File Name: mainwindow.cpp
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:30:29 AM CST
**********************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  kSize(20),
  is_manual(true),
  aStar(kSize, kSize),
  timer(new QTimer(this)),
  dialog(new Dialog(this)),
  timeout(200)
{
  ui->setupUi(this);
  this->init();
}

MainWindow::~MainWindow()
{
  delete ui;
  timer->stop();
  delete timer;
  delete dialog;
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
  bool ok = true;
  switch(e->key())
  {
    case Qt::Key_W:
      ok = moveUp();
      break;
    case Qt::Key_S:
      ok = moveDown();
      break;
    case Qt::Key_A:
      ok = moveLeft();
      break;
    case Qt::Key_D:
      ok = moveRight();
      break;
    default:
      break;
  }
  if(!ok)
  {
    emit game_over("<span style=\"text-align: center;"
                   "color: red; font-size: 36px\">Game Over</span>");
  }
}

void MainWindow::init()
{
  // initialize Window
  this->setFixedSize(this->width(), this->height());
  this->setWindowTitle(QString("life: +1s"));
  this->updateTimeout();
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
  timer->setInterval(timeout);
  connect(timer, &QTimer::timeout, this, &MainWindow::autoMove);
  connect(this, &MainWindow::game_over, this, &MainWindow::on_game_over);
}

bool MainWindow::isBlock(const Point &pos)
{
  if(pos.x < 0 || pos.y < 0)
    return true;
  if(pos.x >= kSize || pos.y >= kSize)
    return true;
  for(auto iter = snake.begin(); iter != snake.end(); ++iter)
  {
    if((*iter)->get_point().equal(pos))
      return true;
  }
  return false;
}

void MainWindow::setUpFoodorHead(Square::Id id)
{
  int step = genRand(world.size() - 1) - 1;
  bool tried = false;
  for(auto iter = world.begin(); iter != world.end(); ++iter)
  {
    auto inner = iter;
    while(step-- > 0)
      ++inner;
    for(; !tried && inner != world.end(); ++inner)
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
    }
    tried = true;
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
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<> dis(0, n);
  return dis(gen);
}

void MainWindow::on_btnRestart_clicked()
{
  timer->stop();
  is_manual = true;
  timeout = 200;
  this->updateTimeout();
  this->setWindowTitle(QString("life: +1s"));
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

bool MainWindow::moveAround(const Point& target)
{
  if(isBlock(target))
    return false;

  Square* target_square = this->findPoint(target);
  if(target_square == nullptr)
  {
    emit game_over("no target found in this world");
    return false;
  }
  target_square->set_id(Square::Id::Head).update_style();
  snake.front()->set_id(Square::Id::Snake).update_style();
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
  this->setWindowTitle(QString("life: +%1s").arg(snake.size()));
  return true;
}

bool MainWindow::moveUp()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x - 1, cur_pos.y};
  return moveAround(target);
}

bool MainWindow::moveDown()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x + 1, cur_pos.y};
  return moveAround(target);
}

bool MainWindow::moveLeft()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x, cur_pos.y - 1};
  return moveAround(target);
}

bool MainWindow::moveRight()
{
  Point cur_pos = snake.front()->get_point();
  Point target{cur_pos.x, cur_pos.y + 1};
  return moveAround(target);
}

bool MainWindow::randomMove(Square* vfood)
{
  std::vector<Point> pos;
  Point head = snake.front()->get_point();
  pos.emplace_back(head.x - 1, head.y);
  pos.emplace_back(head.x + 1, head.y);
  pos.emplace_back(head.x, head.y - 1);
  pos.emplace_back(head.x, head.y + 1);
  std::sort(pos.begin(), pos.end(),
            [target = vfood->get_point()] (const Point& lhs, const Point& rhs) {
    return lhs.manHattanDistance(target) < rhs.manHattanDistance(target);
  });
  int step = genRand(pos.size() - 1);
  auto inner = pos.begin() + step;
  bool ok = true;
  for(auto outer = pos.begin(); outer != pos.end(); ++outer)
  {
    for(;ok && inner != pos.end(); ++inner)
    {
      if(moveAround(*inner))
        return true;
      ok = false;
    }
    if(moveAround(*outer))
      return true;
  }
  return false;
}

bool MainWindow::vMove()
{
  std::list<Point> path;
  while(true)
  {
    path = aStar.reset(vsnake).findBest(vsnake.front(), food);
    if(path.size() == 0)
      return false;
    vsnake.push_front(this->findPoint(path.front()));
    if(!isReachFood(vsnake.front()))
      vsnake.pop_back();
    else
      break;
  }
  return true;
}

// also simulate random move ?
void MainWindow::autoMove()
{
  if(real_path.size() != 0)
  {
    // shall never fire.
    if(!moveAround(real_path.front()))
    {
      emit game_over("<span style=\"text-align: center;"
                     "color: red; font-size: 18px\">"
                     "Target in snake, Game Over</span>");
    }
    real_path.pop_front();
    return;
  }

  if(snake.size() < 5)
  {
    real_path = aStar.reset(snake).findBest(snake.front(), food);
    return;
  }

  vsnake = snake;
  bool res = this->vMove();

  if(res)
  {
    // after simulating can we reach the tail?
    auto path = aStar.reset(vsnake, false).findBest(vsnake.front(), vsnake.back());
    if(path.size() == 0)
    {
      // find worst way to the tail for our real snake.
      real_path = aStar.reset(snake, false).findWorst(snake.front(), snake.back());
      if(real_path.size() == 0)
      {
        if(!randomMove(snake.back()))
        {
          emit game_over("<span style=\"text-align: center;"
                         "color: red; font-size: 18px\">"
                         "No available path, Game Over</span>");
        }
      }
      else
      {
        real_path.pop_back(); // pop snake tail
      }
    }
    else
    {
      real_path = aStar.reset(snake).findBest(snake.front(), food);
    }
    return;
  }
  else
  {
    auto path = aStar.reset(vsnake, false).findWorst(vsnake.front(), vsnake.back());
    if(path.size() < 2)
    {
      if(randomMove(food))
        return;
    }
    else
    {
      real_path = aStar.reset(snake, false).findWorst(snake.front(), snake.back());
      if(real_path.size() < 2)
      {
        if(randomMove(snake.back()))
          return;
      }
      else
      {
        real_path.pop_back(); // pop snake tail
        return;
      }
    }
  }
  emit game_over("<span style=\"text-align: center;"
                 "color: red; font-size: 36px\">Game Over</span>");
}

void MainWindow::updateTimeout()
{
  timer->setInterval(timeout);
  ui->timeout->setText(QString("Move Interval: %1").arg(timeout));
}

void MainWindow::on_btnAutoManual_clicked()
{
  real_path.clear();
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

void MainWindow::on_game_over(QString msg)
{
  QMessageBox::information(this, "Game Over", msg);
  qApp->quit();
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionAbout_triggered()
{
    dialog->show();
}

void MainWindow::on_actionSpeed_Up_triggered()
{
  timeout -= 10;
  if(timeout < 11)
    timeout = 10;
  this->updateTimeout();
}

void MainWindow::on_actionSpeed_Down_triggered()
{
    timeout += 10;
    if(timeout > 499)
      timeout = 500;
    this->updateTimeout();
}
