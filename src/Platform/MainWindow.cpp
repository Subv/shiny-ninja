#include "MainWindow.h"
#include <QLabel>
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    menuBar()->addMenu("Test");
}

MainWindow::~MainWindow()
{
    delete ui;
}
