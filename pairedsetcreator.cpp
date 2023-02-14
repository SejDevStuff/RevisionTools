#include "pairedsetcreator.h"
#include "ui_pairedsetcreator.h"

PairedSetCreator::PairedSetCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PairedSetCreator)
{
    ui->setupUi(this);
}

PairedSetCreator::~PairedSetCreator()
{
    delete ui;
}


void PairedSetCreator::reset() {
    ui->termOneEntry->setText("");
    ui->termOneEntry->setDisabled(true);

    ui->termTwoEntry->setText("");
    ui->termTwoEntry->setDisabled(true);

    ui->flagged->setCheckState(Qt::CheckState::Unchecked);
    ui->flagged->setDisabled(true);

    ui->SavePairChanges->setDisabled(true);
    ui->DeleteTerm->setDisabled(true);

    ui->setName->setText("");

    pairMap.clear();
    ui->Terms->clear();

}
void PairedSetCreator::on_CreateNewTerm_clicked()
{
    PSetPair p;
    QListWidgetItem* termItem = new QListWidgetItem("TermOne");
    p.termOne = "TermOne";
    p.termTwo = "TermTwo";
    pairMap.insert({termItem, p});

    ui->Terms->addItem(termItem);
    ui->Terms->setCurrentItem(termItem);
}

void PairedSetCreator::on_Terms_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (ui->Terms->selectedItems().size() == 0) {
        clearTermEntryData();
        return;
    }

    if (pairMap.empty()) {
        return;
    }

    auto it = pairMap.find(current);
    if (it == pairMap.end()) {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("Could not find item"));
        return;
    }

    PSetPair p = it->second;

    ui->termOneEntry->setText(p.termOne);
    ui->termTwoEntry->setText(p.termTwo);

    if (p.flagged) {
        ui->flagged->setCheckState(Qt::CheckState::Checked);
    } else {
        ui->flagged->setCheckState(Qt::CheckState::Unchecked);
    }

    currentPair = current;

    ui->termOneEntry->setDisabled(false);
    ui->termTwoEntry->setDisabled(false);
    ui->flagged->setDisabled(false);
    ui->SavePairChanges->setDisabled(false);
    ui->DeleteTerm->setDisabled(false);
}

void PairedSetCreator::clearTermEntryData() {
    ui->termOneEntry->setText("");
    ui->termOneEntry->setDisabled(true);

    ui->termTwoEntry->setText("");
    ui->termTwoEntry->setDisabled(true);

    ui->flagged->setCheckState(Qt::CheckState::Unchecked);
    ui->flagged->setDisabled(true);

    ui->SavePairChanges->setDisabled(true);
    ui->DeleteTerm->setDisabled(true);
}

void PairedSetCreator::on_DeleteTerm_clicked()
{
    if (currentPair == NULL) {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("No term selected"));
        return;
    }

    auto it = pairMap.find(currentPair);
    if (it == pairMap.end()) {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("Could not find pair"));
        return;
    }


    //pairs.erase(pairs.begin() + it->second);
    pairMap.erase(currentPair);
    ui->Terms->removeItemWidget(currentPair);
    delete currentPair;

    if (pairMap.size() == 0) {
        clearTermEntryData();
    }
}

void PairedSetCreator::on_SavePairChanges_clicked()
{
    if (currentPair == NULL) {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("No term selected"));
        return;
    }

    auto it = pairMap.find(currentPair);
    if (it == pairMap.end()) {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("Could not find pair"));
        return;
    }

    QString termOne = ui->termOneEntry->text().trimmed();
    QString termTwo = ui->termTwoEntry->text().trimmed();
    Qt::CheckState checkState = ui->flagged->checkState();

    if (termOne == "" || termTwo == "") {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("Invalid data"));
        return;
    }

    currentPair->setText(termOne);

    bool flagged = false;
    if (checkState == Qt::CheckState::Checked) {
        flagged = true;
    }

    it->second.termOne = termOne;
    it->second.termTwo = termTwo;
    it->second.flagged = flagged;
}

void PairedSetCreator::on_SaveSet_clicked()
{
    QString title = ui->setName->text().trimmed();

    if (title == "" || pairMap.empty()) {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("Please fill out the data properly"));
        return;
    }

    std::vector<PSetPair> pairList;

    for (auto const& x : pairMap)
    {
        pairList.push_back(x.second);
    }

    PairedSetWriter psw;

    if (!psw.WritePairedSet(title, pairList)) {
        QMessageBox::critical(this, tr("Create Paired Set - Error"), tr("There was an error writing to the set"));
        return;
    } else {
        reset();
        this->hide();
    }
}

void PairedSetCreator::closeEvent(QCloseEvent *event)
{
    reset();
    event->accept();
}
