/*********************************************************
          File Name: mainwindow.h
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Thu 26 Jan 2017 09:30:29 AM CST
**********************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "square.h"
#include "pathfinding.h"
#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <map>
#include <random>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

signals:
      void game_over();

private slots:
      void on_btnRestart_clicked();

      void on_btnAutoManual_clicked();

  protected:
   void keyPressEvent(QKeyEvent* e);

private:
  Ui::MainWindow *ui;
  const int kSize;
  Square* food;
  std::list<Square*> world;
  std::list<Square*> snake;
  std::random_device rd;
  std::mt19937 gen;
  bool is_manual;
  PathFinding aStar;
  QTimer* timer;

  void init();

  void setUpFoodorHead(Square::Id);

  int genRand(int n);

  bool isReachFood(Square* head);

  Square* findPoint(const Point& p);

  void moveAround(const Point &target);

  void moveUp();

  void moveDown();

  void moveLeft();

  void moveRight();

  void autoMove();
};

#endif // MAINWINDOW_H
