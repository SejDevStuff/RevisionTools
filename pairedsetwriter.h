#ifndef PAIREDSETWRITER_H
#define PAIREDSETWRITER_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <QString>
#include <fstream>

struct PSetPair
{
    QString termOne;
    QString termTwo;
    bool flagged = false;
};

struct PairedSet
{
    uint16_t totalPairs = 0;
    QString title;
    std::vector<PSetPair> pairs;
    bool valid = false;
};

class PairedSetWriter
{
public:
    PairedSetWriter();
    bool WritePairedSet(QString title, std::vector<PSetPair> pairs);
    PairedSet ReadPairedSet(std::string filepath);
};

#endif // PAIREDSETWRITER_H
