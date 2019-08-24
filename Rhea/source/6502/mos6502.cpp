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
        bool ok;
        for (int i=1;i<fields.length();i++)
            b.append((uchar)fields[i].trimmed().toInt(&ok,16));


        m_opcodes[op] = b;
    }

    file.close();
}

uint Mos6502::getAbs()
{
    uint val = ((uchar)m.m_data[r.pc+1])*0x100 +  (uchar)m.m_data[r.pc];
    r.pc=r.pc+2;
    return val;
}

unsigned char Mos6502::getImm()
{
    uchar val = m.m_data[r.pc];
    r.pc=r.pc+1;
    return val;

}

bool Mos6502::Evaluate(unsigned char instruction, QString op, int type)
{
    return ((uchar)m_opcodes[op].at(type)==(uchar)instruction);
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
    qDebug() << "Eat" << Util::numToHex(instruction);
 //   qDebug() << "imm " << Util::numToHex((uchar)m_opcodes["lda"].at(imm));
    if (Evaluate(instruction,"jmp",abs)) {
        r.pc = getAbs();
        qDebug() << "jmp " << Util::numToHex(r.pc);
        return true;
    }
    if (Evaluate(instruction,"lda",abs)) {
        r.a = m.m_data[getAbs()];
        qDebug() << "lda (abs) " << Util::numToHex(r.pc);
        return true;
    }

    if (Evaluate(instruction,"lda",imm)) {
        r.a = getImm();
        qDebug() << "lda (imm) " << Util::numToHex(r.a);
        return true;
    }

    if (Evaluate(instruction,"sta",abs)) {
        uint pos = getAbs();

        m.m_data[pos] = r.a;
        qDebug() << "sta (abs) " << Util::numToHex(pos);

        return true;
    }

}

void Mos6502::Execute()
{
    SetPC(Constants::c.programStartAddress);
    bool ok = true;
    while (ok)
        ok = Eat();
}
