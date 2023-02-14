#include "flashcards.h"
#include "ui_flashcards.h"

Flashcards::Flashcards(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Flashcards)
{
    ui->setupUi(this);
    psc->setWindowTitle("Paired Set Creator");
    loadSets();
}

Flashcards::~Flashcards()
{
    delete ui;
}

void Flashcards::on_createPSet_clicked()
{
    psc->reset();
    psc->show();
}

void Flashcards::reset() {
    ui->nextCard->setDisabled(true);
    ui->prevCard->setDisabled(true);
    ui->flipCard->setDisabled(true);
    ui->flagCard->setDisabled(true);
    ui->toggleVSide->setDisabled(true);
    ui->cardNumber->setText("Card --- of ---");

    ui->cardContent->setText("No Set Selected");
    ui->cardContent->setDisabled(true);
    QFont font = ui->cardContent->font();
    font.setItalic(true);
    ui->cardContent->setFont(font);

    cardIndex = 0;

    PairedSet blank;
    currentSet = blank;

    flipped = false;
}

void Flashcards::loadSets() {
    ui->fcSetBox->clear();
    ui->fcSetBox->addItem("Choose a set...");
    setMap.clear();

    for (const auto& dirEntry : std::filesystem::directory_iterator("./RevisionTools_data")) {
        if (!dirEntry.is_regular_file()) continue;
        PairedSet set = psw->ReadPairedSet(dirEntry.path());
        if (set.valid) {
            ui->fcSetBox->addItem(set.title);
            setMap.insert({set.title, dirEntry.path()});
        }
    }
}

void Flashcards::on_reloadSetList_clicked()
{
    loadSets();
    reset();
}

void Flashcards::showTerm() {
    if (!currentSet.valid) return;
    if (cardIndex >= currentSet.pairs.size() || cardIndex < 0) return;
    PSetPair currentPair = currentSet.pairs[cardIndex];

    if (currentPair.flagged) {
        ui->flagCard->setText("Unflag card");
    } else {
        ui->flagCard->setText("Flag card");
    }

    bool seeT1 = SeeTermOneFirst;

    if (flipped) {
        seeT1 = !seeT1;
    }

    if (seeT1) {
        ui->cardContent->setText(currentPair.termOne);
    } else {
        ui->cardContent->setText(currentPair.termTwo);
    }

    ui->cardNumber->setText("Card " + QString::number(cardIndex + 1) + " of " + QString::number(currentSet.totalPairs));

    ui->nextCard->setDisabled(false);
    ui->prevCard->setDisabled(false);

    if (currentSet.totalPairs == 1) {
        ui->nextCard->setDisabled(true);
    }

    if (cardIndex >= currentSet.pairs.size() - 1) {
        ui->nextCard->setDisabled(true);
    }

    if (cardIndex <= 0) {
        ui->prevCard->setDisabled(true);
    }
}

void Flashcards::on_fcSetBox_textActivated(const QString &arg1)
{
    reset();

    if (arg1 == "Choose a set...") {
        return;
    }

    auto it = setMap.find(arg1);

    if (it == setMap.end()) {
        QMessageBox::critical(this, tr("Flashcards - Error"), tr("Cannot find set"));
        return;
    }

    currentSet = psw->ReadPairedSet(it->second);

    if (!currentSet.valid) {
        QMessageBox::critical(this, tr("Flashcards - Error"), tr("Invalid set"));
        return;
    }

    if (currentSet.totalPairs == 0) {
        QMessageBox::critical(this, tr("Flashcards - Error"), tr("Empty set"));
        return;
    }

    if (currentSet.totalPairs > 1) {
        ui->nextCard->setDisabled(false);
    }

    ui->flipCard->setDisabled(false);
    ui->flagCard->setDisabled(false);
    ui->toggleVSide->setDisabled(false);
    ui->cardContent->setDisabled(false);

    QFont font = ui->cardContent->font();
    font.setItalic(false);
    ui->cardContent->setFont(font);

    showTerm();
}


void Flashcards::on_nextCard_clicked()
{
    flipped = false;
    cardIndex++;
    if (cardIndex >= currentSet.pairs.size()) {
        cardIndex = currentSet.pairs.size() - 1;
    }
    showTerm();
}


void Flashcards::on_prevCard_clicked()
{
    flipped = false;
    cardIndex--;
    if (cardIndex < 0) {
        cardIndex = 0;
    }
    showTerm();
}


void Flashcards::on_flipCard_clicked()
{
    flipped = !flipped;
    showTerm();
}


void Flashcards::on_toggleVSide_clicked()
{
    SeeTermOneFirst = !SeeTermOneFirst;
    showTerm();
}


void Flashcards::on_flagCard_clicked()
{
    if (cardIndex >= currentSet.pairs.size() || cardIndex < 0) return;
    currentSet.pairs[cardIndex].flagged = !currentSet.pairs[cardIndex].flagged;
    bool write = psw->WritePairedSet(currentSet.title, currentSet.pairs);

    if (!write) {
        QMessageBox::critical(this, tr("Flashcards - Error"), tr("Error saving PSET with modified card. The card was not (un)flagged."));
        currentSet.pairs[cardIndex].flagged = !currentSet.pairs[cardIndex].flagged;
    } else {
        if (currentSet.pairs[cardIndex].flagged) {
            QMessageBox::information(this, tr("Flashcards - Info"), tr("The card had been flagged for further revision"));
        } else {
            QMessageBox::information(this, tr("Flashcards - Info"), tr("The card had been unflagged"));
        }
    }

    showTerm();
}

