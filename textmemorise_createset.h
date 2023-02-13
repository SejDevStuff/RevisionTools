#ifndef TEXTMEMORISE_CREATESET_H
#define TEXTMEMORISE_CREATESET_H

#include <QWidget>
#include <fstream>
#include <QMessageBox>
#include <cstdint>


namespace Ui {
class textmemorise_createset;
}

class textmemorise_createset : public QWidget
{
    Q_OBJECT

public:
    explicit textmemorise_createset(QWidget *parent = nullptr);
    ~textmemorise_createset();
    void reset();

private slots:
    void on_pushButton_clicked();

private:
    Ui::textmemorise_createset *ui;
};

#endif // TEXTMEMORISE_CREATESET_H
