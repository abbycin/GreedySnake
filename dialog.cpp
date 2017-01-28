#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);
  ui->author->setText("<style> a {text-decoration: none; font-size: 18px;"
                      " text-align: center} </style>"
                      "<a href=\"https://note.isliberty.me\">Abby Cin"
                      " (abbytsing@gmail.com)</a>");
  ui->author->setOpenExternalLinks(true);
  this->setFixedSize(this->width(), this->height());
  this->setWindowTitle("Author");
}

Dialog::~Dialog()
{
  delete ui;
}
