#ifndef OKMEMORY_H
#define OKMEMORY_H

#include <QString>
#include <QByteArray>

class OKMemory
{
public:
    QByteArray m_memory;
    void Init(int size);
    uchar get(int pos);
    ushort getWord(int pos);
    void set(int pos, uchar v);
    QString name;
    OKMemory();
};

#endif // OKMEMORY_H
