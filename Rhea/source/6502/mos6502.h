#ifndef MOS6502_H
#define MOS6502_H
#include <QByteArray>
#include <QMap>
#include <QDebug>
#include "source/misc/util.h"
#define uchar unsigned char

class MOS6502Registers {
public:
    int a=0,x=0,y=0;
    int sp=0, pc=0x400;
    int N,V,B,D,I,Z,C;
};



class Memory {
public:
    QByteArray m_data;
};

class Constants
{
public:
    int programStartAddress = 0x400;

    static Constants c;
};


class Mos6502
{
public:
    MOS6502Registers r;
    Memory m;
    Mos6502();
    void Initialize();
    QMap<QString, QByteArray> m_opcodes;
    void LoadOpcodes();
    void SetPC(int i);
    int LoadProgram(QString fn);
    bool Eat();
    void Execute();

};

#endif // MOS6502_H
