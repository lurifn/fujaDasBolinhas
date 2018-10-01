#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //faz a janela não ser redimensionavel
    setMaximumSize(size());
    setMinimumSize(size());
}

MainWindow::~MainWindow()
{
    delete ui;
}
