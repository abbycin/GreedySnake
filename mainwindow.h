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
#include "dialog.h"
#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
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

    void on_game_over();

    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

    void on_actionSpeed_Up_triggered();

    void on_actionSpeed_Down_triggered();

  protected:
   void keyPressEvent(QKeyEvent* e);

private:
  Ui::MainWindow *ui;
  const int kSize;
  Square* food;
  std::list<Square*> world;
  std::list<Square*> snake;
  std::random_device rd;
  //std::mt19937 gen;
  bool is_manual;
  PathFinding aStar;
  QTimer* timer;
  Dialog* dialog;
  int timeout;

  void init();

  bool isBlock(const Point& pos);

  void setUpFoodorHead(Square::Id);

  int genRand(int n);

  bool isReachFood(Square* head);

  Square* findPoint(const Point& p);

  bool moveAround(const Point &target);

  bool moveUp();

  bool moveDown();

  bool moveLeft();

  bool moveRight();

  bool randomMove();

  void autoMove();

  void updateTimeout();
};

#endif // MAINWINDOW_H
