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
#include <memory>

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
  int step = genRand(world.size() - 1);
  bool tried = false;
  for(auto iter = world.begin(); iter != world.end(); ++iter)
  {
    auto inner = iter;
    while(step-- > 0 && inner != world.end())
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
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<> dis(0, n - 1);
  return dis(gen);
}

void MainWindow::on_btnRestart_clicked()
{
  timer->stop();
  is_manual = true;
  timeout = 200;
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
    emit game_over();
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

bool MainWindow::randomMove()
{
  if(moveUp())
    return true;
  if(moveDown())
    return true;
  if(moveLeft())
    return true;
  return moveRight();
}

void MainWindow::autoMove()
{
  auto& path = aStar.reset(snake).find(snake.front(), food);
  // only go first step
  if(path.size() < 1)
  {
    if(!this->randomMove())
      emit game_over();
    return;
  }
  /*
  for(auto iter = path.rbegin(); iter != path.rend(); ++iter)
  {
    moveAround(*iter);
  }
  */
  auto iter = ++path.rbegin();
  moveAround(*iter);
}

void MainWindow::updateTimeout()
{
  ui->timeout->setText(QString("Move Interval: %1").arg(timeout));
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

void MainWindow::on_game_over()
{
  QMessageBox::information(this, "Game Over", "<span style=\"text-align: center;"
                           "color: red; font-size: 36px\">Game Over</span>");
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
  timer->setInterval(timeout);
}

void MainWindow::on_actionSpeed_Down_triggered()
{
    timeout += 10;
    if(timeout > 499)
      timeout = 500;
    this->updateTimeout();
    timer->setInterval(timeout);
}
