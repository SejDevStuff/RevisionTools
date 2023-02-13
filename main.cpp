#include "mainwindow.h"

#include <QApplication>
#include <filesystem>

int main(int argc, char *argv[])
{
    if (!std::filesystem::exists("./RevisionTools_data")) {
        if (!std::filesystem::create_directory("./RevisionTools_data")) {
            return 1;
        }
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("RevisionTools");
    w.show();
    return a.exec();
}
