#include "textmemorise_createset.h"
#include "ui_textmemorise_createset.h"

textmemorise_createset::textmemorise_createset(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::textmemorise_createset)
{
    ui->setupUi(this);
}

textmemorise_createset::~textmemorise_createset()
{
    delete ui;
}

void textmemorise_createset::reset() {
    ui->setTitle->setText("");
    ui->setContents->setPlainText("");
    ui->setKnowPercent->setValue(0);
}

void textmemorise_createset::on_pushButton_clicked()
{
    QString title = ui->setTitle->text().trimmed();
    QString contents = ui->setContents->toPlainText().trimmed();
    int knowPercent = ui->setKnowPercent->value();

    if ((knowPercent % 10) != 0) {
        QMessageBox::critical(this, tr("Create Set"), tr("The percentage should be a multiple of 10"));
        return;
    }

    if (title.trimmed() == "" || contents.trimmed() == "") {
        QMessageBox::critical(this, tr("Create Set"), tr("Please fill out the data correctly"));
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

    std::ifstream f("./RevisionTools_data/textmemorise/" + safeText.toStdString() + ".set");
    bool exists = f.good();
    f.close();

    if (exists) {
        QMessageBox::critical(this, tr("Create Set"), tr("A set with the same name already exists"));
        return;
    }

    int8_t percentComplete = knowPercent;
    uint16_t titleSz = title.size();
    uint64_t textSz = contents.size();

    if (titleSz > UINT16_MAX) {
        QMessageBox::critical(this, tr("Create Set"), tr("Your title is too long"));
        return;
    }

    if (textSz > UINT64_MAX) {
        QMessageBox::critical(this, tr("Create Set"), tr("The size of the contents is too long"));
        return;
    }

    std::ofstream outSet("./RevisionTools_data/textmemorise/" + safeText.toStdString() + ".set", std::ios::binary);

    if (!outSet) {
        QMessageBox::critical(this, tr("Create Set"), tr("There was an error writing to the set"));
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
    this->hide();
}

