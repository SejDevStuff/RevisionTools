#include "textmemorise.h"
#include "ui_textmemorise.h"

void TextMemorise::hideMainLayout() {
    ui->ansStatus->hide();
    ui->nextWordLbl->hide();
    ui->answer->hide();
    ui->submit->hide();
    ui->setContents->hide();
    ui->percentCompleted->hide();
    ui->setTitle->hide();
    ui->continueBtn->hide();
}

void TextMemorise::showMainLayout() {
    ui->ansStatus->show();
    ui->nextWordLbl->show();
    ui->answer->show();
    ui->submit->show();
    ui->setContents->show();
    ui->percentCompleted->show();
    ui->setTitle->show();
    ui->continueBtn->show();
}

TextMemorise::TextMemorise(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextMemorise)
{
    ui->setupUi(this);

    srand(time(0));

    if (!std::filesystem::exists("./RevisionTools_data/textmemorise")) {
        if (!std::filesystem::create_directory("./RevisionTools_data/textmemorise")) {
            QMessageBox::critical(this, tr("RevisionTools"), tr("ERROR: Cannot create storage directory. Please run this program in a place where it has read/write permissions."));
            QCoreApplication::quit();
        }
    }

    ui->setList->addItem("Choose a set...");
    createSet->setWindowTitle("Create set - Memorise Text");

    hideMainLayout();

    loadSets();

    connect(ui->setList, SIGNAL(textActivated(QString)), this, SLOT(setChanged(QString)));
}

SetStructure parseSet(std::string fpath) {
    SetStructure structure;
    std::ifstream inSet(fpath, std::ios::binary);

    std::string magic;
    for (int i = 0; i < 4; i++) {
        char byte;
        inSet.read(&byte, 1);
        magic += byte;
    }

    if (magic != "SET0") {
        return structure;
    }

    inSet.read(reinterpret_cast<char*>(&structure.percentCompleted), sizeof(int8_t));
    inSet.read(reinterpret_cast<char*>(&structure.titleSz), sizeof(uint16_t));

    for (uint16_t i = 0; i < structure.titleSz; i++) {
        char letter;
        inSet.read(&letter, sizeof(letter));
        structure.title += letter;
    }

    inSet.read(reinterpret_cast<char*>(&structure.contentsSize), sizeof(uint64_t));
    for (uint64_t i = 0; i < structure.contentsSize; i++) {
        char letter;
        inSet.read(&letter, sizeof(letter));
        structure.contents += letter;
    }

    return structure;
}

void TextMemorise::writeSet(SetStructure set) {
    QString title = QString::fromStdString(set.title).trimmed();
    QString contents = QString::fromStdString(set.contents).trimmed();

    if (title == "" || contents == "") {
        QMessageBox::critical(this, tr("Memorise Text"), tr("Error saving set"));
        return;
    }

    QString safeText = "";
    for (int i = 0; i < title.size(); i++) {
        if (title[i].isPunct()) {
            safeText += "_";
        } else {
            safeText += title[i];
        }
    }

    int8_t percentComplete = set.percentCompleted;
    uint16_t titleSz = title.size();
    uint64_t textSz = contents.size();

    if (titleSz > UINT16_MAX) {
        QMessageBox::critical(this, tr("Memorise Text"), tr("Error saving set"));
        return;
    }

    if (textSz > UINT64_MAX) {
        QMessageBox::critical(this, tr("Memorise Text"), tr("Error saving set"));
        return;
    }

    std::ofstream outSet("./RevisionTools_data/textmemorise/" + safeText.toStdString() + ".set", std::ios::binary);

    if (!outSet) {
        QMessageBox::critical(this, tr("Memorise Text"), tr("There was an error writing to the set"));
        return;
    }

    char magic[] = {'S', 'E', 'T', '0'};
    for (int i = 0; i < 4; i++) {
        outSet.write(&magic[i], sizeof(magic[i]));
    }

    outSet.write((char*)&percentComplete, sizeof(percentComplete));

    outSet.write((char*)&titleSz, sizeof(titleSz));
    for (uint16_t i = 0; i < titleSz; i++) {
        char letter = title.toStdString()[i];
        outSet.write(&letter, sizeof(letter));
    }

    outSet.write((char*)&textSz, sizeof(textSz));
    for (uint64_t i = 0; i < textSz; i++) {
        char letter = contents.toStdString()[i];
        outSet.write(&letter, sizeof(letter));
    }

    outSet.close();
}

void TextMemorise::loadSets() {
    ui->setList->clear();
    setMap.clear();
    resetState();
    ui->setList->addItem("Choose a set...");

    std::ifstream inSet;

    for (const auto& dirEntry : std::filesystem::directory_iterator("./RevisionTools_data/textmemorise")) {
        if (dirEntry.is_regular_file()) {
            std::string filename = dirEntry.path();

            uint8_t percentCompleted = 0;
            uint16_t titleSz = 0;
            std::string title = "";

            inSet.open(filename, std::ios::binary);

            if (!inSet) {
                inSet.close();
                continue;
            }

            std::string magic;
            for (int i = 0; i < 4; i++) {
                char byte;
                inSet.read(&byte, 1);
                magic += byte;
            }

            if (magic != "SET0") {
                inSet.close();
                continue;
            }

            inSet.read(reinterpret_cast<char*>(&percentCompleted), sizeof(int8_t));
            inSet.read(reinterpret_cast<char*>(&titleSz), sizeof(uint16_t));

            for (uint16_t i = 0; i < titleSz; i++) {
                char letter;
                inSet.read(&letter, sizeof(letter));
                title += letter;
            }

            if (title == "") {
                continue;
            }

            ui->setList->addItem(QString::fromStdString(title));
            setMap.insert({title, filename});

            inSet.close();
        }
    }
}

void TextMemorise::resetState() {
    ui->NoSetSelect->setText("No set selected");
    ui->NoSetSelect->show();
    ui->setList->setDisabled(false);
    ui->createSet->setDisabled(false);
    hideMainLayout();
    setReady = false;
    correct = 0;
    incorrect = 0;
    ui->ansStatus->setText("Correct: " + QString::number(correct) + ", Incorrect: " + QString::number(incorrect));
    nextWords.clear();
    wordsRead = 0;
    wordsTotal = 0;
    currentSet = *new SetStructure();
    hideList.clear();
    wordsOnView.clear();
}

void TextMemorise::showNextWordSet() {
    nextWords.clear();
    hideList.clear();
    wordsOnView.clear();

    int wordsToRead = 16;
    if ((wordsRead + 16) > wordsTotal) {
        wordsToRead = wordsTotal - wordsRead;
    }

    if (wordsToRead == 0) {
        if (correct >= ((incorrect + correct) * 0.8)) {
            currentSet.percentCompleted += 10;
        } else
        {
            currentSet.percentCompleted -= 10;
        }

        if (currentSet.percentCompleted < 0) {
            currentSet.percentCompleted = 0;
        }

        if (currentSet.percentCompleted > 100) {
            currentSet.percentCompleted = 100;
        }

        writeSet(currentSet);

        QMessageBox::information(this, tr("Set Results - Memorise Text"), tr("You've finished the set. You know " + QString::number(currentSet.percentCompleted).toLocal8Bit() + "% of the set"));
        loadSets();
        return;
    }

    // grab next few words
    std::vector<std::string> words;
    std::string word;
    int wordsReadSoFar = 0;
    bool reachedWord = false;
    for (int i = 0; i < currentSet.contents.size(); i++) {
        if (currentSet.contents[i] == ' ') {
            if (reachedWord) {
                reachedWord = false;
                wordsReadSoFar++;
                if (wordsReadSoFar > wordsRead && wordsReadSoFar <= (wordsRead + wordsToRead)) {
                    words.push_back(word);
                }
                word = "";
            }
        } else {
            if (!reachedWord) {
                reachedWord = true;
            }
            word += currentSet.contents[i];
        }
    }
    if (wordsReadSoFar >= wordsRead && wordsReadSoFar <= (wordsRead + wordsToRead)) {
        if (reachedWord) words.push_back(word);
    }
    wordsRead += wordsToRead;

    // display them
    int percentageHide = currentSet.percentCompleted + 10;
    if (percentageHide > 100) {
        percentageHide = 100;
    }

    QString setContentDisplay = "";
    for (int i = 0; i < words.size(); i++) {
        int hideChance = (rand() % 100) + 1;
        QString convStr = QString::fromStdString(words[i]);
        QString answerWord = "";
        if (hideChance <= percentageHide)
        {
            for (int j = 0; j < convStr.size(); j++) {
                if (convStr[j].isPunct()) {
                    setContentDisplay += convStr[j];
                } else {
                    setContentDisplay += "_";
                    answerWord += convStr[j];
                }
            }
            setContentDisplay += " ";
            hideList.push_back(i);
            nextWords.push_back(answerWord);
        } else {
            setContentDisplay += convStr + " ";
        }
        wordsOnView.push_back(convStr);
    }

    ui->setContents->setText(setContentDisplay);

    if (nextWords.size() == 0) {
        ui->answer->setDisabled(true);
        ui->submit->setDisabled(true);
        ui->continueBtn->setDisabled(false);
    } else {
        ui->answer->setDisabled(false);
        ui->submit->setDisabled(false);
        ui->continueBtn->setDisabled(true);
    }

}

void TextMemorise::setChanged(QString s) {
    resetState();
    if (s == "Choose a set...") {
        return;
    }

    ui->setList->setDisabled(true);
    ui->createSet->setDisabled(true);
    ui->NoSetSelect->setText("Loading ...");
    ui->NoSetSelect->show();

    // Load
    auto it = setMap.find(s.toStdString());

    if (it == setMap.end()) {
        QMessageBox::critical(this, tr("Memorise Text"), tr("Error loading set"));
        resetState();
        return;
    }

    ui->setTitle->setText("Revising set: " + s);
    currentSet = parseSet(it->second);
    ui->percentCompleted->setText(QString::number((int)currentSet.percentCompleted) + "% memorised");

    bool reachedWord = false;
    for (int i = 0; i < currentSet.contents.size(); i++) {
        if (currentSet.contents[i] == ' ') {
            if (reachedWord) {
                reachedWord = false;
                wordsTotal++;
            }
        } else {
            if (!reachedWord) {
                reachedWord = true;
            }
        }
    }
    // one last word in the buffer
    if (reachedWord) wordsTotal++;

    ui->NoSetSelect->hide();
    showMainLayout();
    ui->setList->setDisabled(false);
    ui->createSet->setDisabled(false);

    showNextWordSet();
}

TextMemorise::~TextMemorise()
{
    delete ui;
}

void TextMemorise::on_createSet_clicked()
{
    createSet->reset();
    createSet->show();
}

void TextMemorise::on_reloadSets_clicked()
{
    loadSets();
}


void TextMemorise::on_submit_clicked()
{
    if (ui->answer->text().trimmed() == "") {
        QMessageBox::critical(this, tr("Memorise Text"), tr("Please enter an answer"));
        return;
    }

    QString nextWord;
    QString userAns = ui->answer->text().toUpper();

    if (nextWords.size() > 0) {
        nextWord = nextWords[0].toUpper();
    } else {
        return;
    }

    if (nextWord == userAns) {
        correct++;
    } else {
        incorrect++;
        QMessageBox::information(this, tr("Incorrect - Memorise Text"), tr("The answer you entered was incorrect. Better luck next time!"));
    }

    ui->answer->setText("");
    ui->ansStatus->setText("Correct: " + QString::number(correct) + ", Incorrect: " + QString::number(incorrect));

    nextWords.erase(nextWords.begin());
    hideList.erase(hideList.begin());

    QString updatedContentDisp = "";

    for (int i = 0; i < wordsOnView.size(); i++) {
        bool hide = false;
        for (int j = 0; j < hideList.size(); j++) {
            if (i ==  hideList[j]) {
                hide = true;
            }
        }

        if (hide)
        {
            for (int j = 0; j < wordsOnView[i].size(); j++) {
                if (wordsOnView[i][j].isPunct()) {
                    updatedContentDisp += wordsOnView[i][j];
                } else {
                    updatedContentDisp += "_";
                }
            }

            updatedContentDisp += " ";
        } else {
            updatedContentDisp += wordsOnView[i] + " ";
        }
    }

    ui->setContents->setText(updatedContentDisp);

    if (nextWords.size() == 0) {
        ui->answer->setDisabled(true);
        ui->submit->setDisabled(true);
        ui->continueBtn->setDisabled(false);
    }
}


void TextMemorise::on_continueBtn_clicked()
{
    showNextWordSet();
}


void TextMemorise::on_answer_returnPressed()
{
    on_submit_clicked();
}

