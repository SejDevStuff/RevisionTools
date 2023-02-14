#include "pairedsetwriter.h"

PairedSetWriter::PairedSetWriter()
{

}

PairedSet PairedSetWriter::ReadPairedSet(std::string filepath) {
    PairedSet ps;

    std::ifstream inSet(filepath);
    if (!inSet)
    {
        return ps;
    }

    inSet.seekg(0, std::ios::end);
    int sz = inSet.tellg();
    inSet.seekg(0);

    std::string magic;
    for (int i = 0; i < 4; i++)
    {
        char letter;
        inSet.read(&letter, sizeof(char));
        magic += letter;
    }

    if (magic != "PSET") {
        return ps;
    }

    uint16_t titleSz;
    std::string title;

    inSet.read((char*)&titleSz, sizeof(uint16_t));
    for (uint16_t i = 0; i < titleSz; i++) {
        char letter;
        inSet.read(&letter, sizeof(char));
        title += letter;
    }

    ps.title = QString::fromStdString(title);

    inSet.read((char*)&ps.totalPairs, sizeof(ps.totalPairs));

    while ((sz - inSet.tellg()) > 0) {
        PSetPair p;

        uint64_t t1Sz, t2Sz;
        uint8_t flagStatus;

        inSet.read((char*)&t1Sz, sizeof(uint64_t));
        std::string t1;
        for (uint64_t j = 0; j < t1Sz; j++) {
            char letter;
            inSet.read(&letter, sizeof(char));
            t1 += letter;
        }
        p.termOne = QString::fromStdString(t1);

        inSet.read((char*)&t2Sz, sizeof(uint64_t));
        std::string t2;
        for (uint64_t j = 0; j < t2Sz; j++) {
            char letter;
            inSet.read(&letter, sizeof(char));
            t2 += letter;
        }
        p.termTwo = QString::fromStdString(t2);

        inSet.read((char*)&flagStatus, sizeof(uint8_t));

        if (flagStatus == 1) {
            p.flagged = true;
        } else {
            p.flagged = false;
        }

        ps.pairs.push_back(p);
    }

    if (ps.pairs.size() == 0) {
        return ps;
    }

    if (ps.pairs.size() != ps.totalPairs) {
        ps.totalPairs = ps.pairs.size();
    }

    ps.valid = true;
    return ps;
}

bool PairedSetWriter::WritePairedSet(QString title, std::vector<PSetPair> pairs)
{
    title = title.trimmed();

    if (title == "") {
        return false;
    }

    QString safeText = "";
    for (int i = 0; i < title.size(); i++) {
        if (title[i].isPunct()) {
            safeText += "_";
        } else {
            safeText += title[i];
        }
    }

    std::ifstream f("./RevisionTools_data/" + safeText.toStdString() + ".set");
    bool exists = f.good();
    f.close();

    if (exists) {
        return false;
    }

    std::ofstream outSet("./RevisionTools_data/" + safeText.toStdString() + ".pset", std::ios::binary);

    if (!outSet) {
        return false;
    }

    char magic[] = {'P', 'S', 'E', 'T'};
    for (int i = 0; i < 4; i++) {
        outSet.write(&magic[i], sizeof(magic[i]));
    }

    if (title.size() > UINT16_MAX) {
        outSet.close();
        return false;
    }

    uint16_t titleSz = title.size();

    outSet.write((char*)&titleSz, sizeof(titleSz));

    for (int i = 0; i < title.size(); i++) {
        char letter = title.toStdString()[i];
        outSet.write(&letter, sizeof(letter));
    }

    uint8_t FLAGGED = 1;
    uint8_t UNFLAGGED = 2;

    if (pairs.size() > UINT16_MAX)
    {
        outSet.close();
        return false;
    }

    uint16_t totalPairs = pairs.size();
    outSet.write((char*)&totalPairs, sizeof(totalPairs));

    for (int i = 0; i < pairs.size(); i++) {
        PSetPair p = pairs[i];

        if (p.termOne.size() > UINT64_MAX || p.termTwo.size() > UINT64_MAX)
        {
            outSet.close();
            return false;
        }

        uint64_t t1Sz = p.termOne.size();
        outSet.write((char*)&t1Sz, sizeof(t1Sz));

        for (int j = 0; j < p.termOne.size(); j++) {
            char letter = p.termOne.toStdString()[j];
            outSet.write(&letter, sizeof(letter));
        }

        uint64_t t2Sz = p.termTwo.size();
        outSet.write((char*)&t2Sz, sizeof(t2Sz));

        for (int j = 0; j < p.termTwo.size(); j++) {
            char letter = p.termTwo.toStdString()[j];
            outSet.write(&letter, sizeof(letter));
        }

        if (p.flagged) {
            outSet.write((char*)&FLAGGED, sizeof(FLAGGED));
        } else {
            outSet.write((char*)&UNFLAGGED, sizeof(UNFLAGGED));
        }

    }

    outSet.close();

    return true;
}
