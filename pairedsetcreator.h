#ifndef PAIREDSETCREATOR_H
#define PAIREDSETCREATOR_H

#include <QWidget>
#include <map>
#include <vector>
#include <QListWidgetItem>
#include <iostream>
#include <QMessageBox>

#include "pairedsetwriter.h"

namespace Ui {
class PairedSetCreator;
}

class PairedSetCreator : public QWidget
{
    Q_OBJECT

public:
    explicit PairedSetCreator(QWidget *parent = nullptr);
    void reset();
    ~PairedSetCreator();

private slots:
    void on_CreateNewTerm_clicked();

    void on_Terms_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_DeleteTerm_clicked();

    void on_SavePairChanges_clicked();

    void on_SaveSet_clicked();

private:
    Ui::PairedSetCreator *ui;
    void clearTermEntryData();

    std::map<QListWidgetItem*, PSetPair> pairMap;
    QListWidgetItem* currentPair;
};

#endif // PAIREDSETCREATOR_H
