#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::dashItemPushed(QString s)
{
    auto tool = tools.find(s.toStdString());
    if (tool != tools.end()) {
        tool->second->setWindowTitle(s);
        tool->second->show();
        hide();
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (!std::filesystem::exists("./RevisionTools_data")) {
        if (!std::filesystem::create_directory("./RevisionTools_data")) {
            QMessageBox::critical(this, tr("RevisionTools"), tr("ERROR: Cannot create storage directory. Please run this program in a place where it has read/write permissions."));
            QCoreApplication::quit();
        }
    }

    tools.insert({"Memorise Text", tm});

    int row = 0;
    int col = 0;

    for (auto const& tool : tools) {
        col++;
        if (col >= 2) {
            col = 0;
            row++;
        }
        QPushButton* b = new QPushButton();
        b->setText(QString::fromStdString(tool.first));
        connect(b, SIGNAL(clicked()), &mapper, SLOT(map()));
        mapper.setMapping(b, QString::fromStdString(tool.first));
        ui->appDash->addWidget(b, row, col);
    }

    connect(&mapper, SIGNAL(mappedString(QString)), this, SLOT(dashItemPushed(QString)));

//    QPushButton* b = new QPushButton();
//    b->setText("TextMemorise");
//    connect(b, SIGNAL(clicked()), this, SLOT(textmemorise_pushed()));
//    ui->appDash->addWidget(b, 0, 0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

