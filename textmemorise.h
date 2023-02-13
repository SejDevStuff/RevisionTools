#ifndef TEXTMEMORISE_H
#define TEXTMEMORISE_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <iostream>
#include <textmemorise_createset.h>
#include <filesystem>
#include <QMessageBox>
#include <fstream>
#include <cstdint>
#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>

struct SetStructure
{
    int8_t percentCompleted;
    uint16_t titleSz;
    std::string title;
    uint64_t contentsSize;
    std::string contents;
};

namespace Ui {
class TextMemorise;
}

class TextMemorise : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextMemorise(QWidget *parent = nullptr);
    ~TextMemorise();

private:
    Ui::TextMemorise *ui;
    textmemorise_createset* createSet = new textmemorise_createset();
    void hideMainLayout();
    void showMainLayout();
    void loadSets();
    void resetState();
    void writeSet(SetStructure set);
    void showNextWordSet();
    std::map<std::string, std::string> setMap;

    bool setReady = false;
    int correct = 0;
    int incorrect = 0;
    std::vector<QString> nextWords;
    std::vector<int> hideList;
    std::vector<QString> wordsOnView;
    int wordsRead = 0;
    int wordsTotal = 0;
    SetStructure currentSet;

private slots:
    void setChanged(QString s);
    void on_createSet_clicked();
    void on_reloadSets_clicked();
    void on_submit_clicked();
    void on_continueBtn_clicked();
    void on_answer_returnPressed();
};

#endif // TEXTMEMORISE_H
