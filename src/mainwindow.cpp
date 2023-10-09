#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/color.h>

#include <random>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->openGLWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_actionCreateRect_triggered()
{
    fmt::print("on_actionCreateRect_triggered\n");
    float dx = rand() % 100 - 50.0f;
    float dy = rand() % 100 - 50.0f;
    float dz = rand() % 100 - 50.0f;
    ui->openGLWidget->setNewRect(dx, dy, dz);
}

void MainWindow::on_actionClearAllRects_triggered()
{
    fmt::print("on_actionClearAllRects_triggered\n");
    ui->openGLWidget->cleanAllRects();

}
