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
    const int none = 0;
    const int imm = 1;
    const int zp = 2;
    const int zpx = 3;
    const int zpy = 4;
    const int izx = 5;
    const int zxy = 6;
    const int abs = 7;
    const int abx = 8;
    const int aby = 9;
    const int ind = 10;
    const int rel = 11;


    MOS6502Registers r;
    Memory m;
    Mos6502();
    void Initialize();
    QMap<QString, QByteArray> m_opcodes;
    void LoadOpcodes();
    uint getAbs();
    uchar getImm();
    bool Evaluate(uchar instruction, QString op,int type);
    void SetPC(int i);
    int LoadProgram(QString fn);
    bool Eat();
    void Execute();

};

#endif // MOS6502_H
