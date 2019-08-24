#include "mos6502.h"
#include <QFile>
#include <QTextStream>

Constants Constants::c;

Mos6502::Mos6502()
{

}

void Mos6502::Initialize()
{
    m.m_data.resize(65536);
}

void Mos6502::LoadOpcodes()
{
    QFile file(":/resources/text/opcodes.txt");

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Mos6502::LoadOpcodes can't load opcodes";
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        QByteArray b;
        QString op = fields[0].trimmed().toLower();
        if (op=="" || op.startsWith("#"))
            continue;
        for (int i=1;i<fields.length();i++)
            b.append(i);

        m_opcodes[op] = b;
    }

    file.close();
}

void Mos6502::SetPC(int i)
{
    r.pc = i;
}

int Mos6502::LoadProgram(QString fn)
{
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly)) return 0;
    QByteArray blob = file.readAll();
    int pos = blob[1]*0x100 + blob[0];
    for (int i=2;i<blob.size();i++)
        m.m_data[i-2+pos] = blob[i];
    return pos;
}

bool Mos6502::Eat()
{
    uchar instruction = m.m_data[r.pc];
    r.pc++;
}

void Mos6502::Execute()
{
    SetPC(Constants::c.programStartAddress);
    bool ok = true;
    while (ok)
        ok = Eat();
}
