#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "textmemorise.h"
#include <QMainWindow>
#include <QPushButton>
#include <map>
#include <iostream>
#include <QSignalMapper>
#include <filesystem>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::map<std::string, QWidget*> tools;
    TextMemorise* tm = new TextMemorise();
    QSignalMapper mapper;

private slots:
    void dashItemPushed(QString s);
};
#endif // MAINWINDOW_H
