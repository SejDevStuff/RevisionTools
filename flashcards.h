#ifndef FLASHCARDS_H
#define FLASHCARDS_H

#include "pairedsetcreator.h"
#include "pairedsetwriter.h"
#include <QMainWindow>
#include <filesystem>
#include <map>
#include <QMessageBox>

namespace Ui {
class Flashcards;
}

class Flashcards : public QMainWindow
{
    Q_OBJECT

public:
    explicit Flashcards(QWidget *parent = nullptr);
    ~Flashcards();

private slots:
    void on_createPSet_clicked();

    void on_reloadSetList_clicked();

    void on_fcSetBox_textActivated(const QString &arg1);

    void on_nextCard_clicked();

    void on_prevCard_clicked();

    void on_flipCard_clicked();

    void on_toggleVSide_clicked();

    void on_flagCard_clicked();

private:
    void loadSets();
    void reset();
    void showTerm();

    std::map<QString, std::string> setMap;

    Ui::Flashcards *ui;
    PairedSetCreator* psc = new PairedSetCreator();
    PairedSetWriter* psw = new PairedSetWriter();

    PairedSet currentSet;
    int cardIndex = 0;
    bool SeeTermOneFirst = true;
    bool flipped = false;
};

#endif // FLASHCARDS_H
