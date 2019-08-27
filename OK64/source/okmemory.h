#ifndef OKMEMORY_H
#define OKMEMORY_H

#include <QString>
#include <QByteArray>

class OKMemory
{
    QByteArray m_memory;
public:
    void Init(int size);
    uchar get(int pos);
    void set(int pos, uchar v);
    QString name;
    OKMemory();
};

#endif // OKMEMORY_H
