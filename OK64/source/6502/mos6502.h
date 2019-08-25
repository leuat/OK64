#ifndef MOS6502_H
#define MOS6502_H
#include <QByteArray>
#include <QMap>
#include <QDebug>
#include "source/misc/util.h"
#define uchar unsigned char

class MOS6502Registers {
public:
    uchar a=0,x=0,y=0;
    ushort sp=0xFF, pc=0x400;
    int N=0,V=0,B=0,D=0,I=0,Z=0,C=0;

    void setZN(uchar v);
    void setC(uchar v);
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

class Opcode {
public:
    QString m_name;
    int m_type;
    Opcode(QString name, int type) {
        m_name=name;
        m_type=type;
    }
    Opcode() {}
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
    uchar cmp, cpx, cpy;
    Mos6502();
    void Initialize();
//    QMap<QString, QByteArray*> m_opcodes;
    QMap<uchar, Opcode> m_opcodes;
    void LoadOpcodes();


    uint getAbs();
    uchar getImm();
    void push(uchar c);
    uchar pop();

    int m_cycles = 0;
    void ClearCycles();
    void SpendCycles(int cnt);



    void pushI(ushort c);
    ushort popI();


//    bool Evaluate(uchar instruction, QString op,int type);
    void SetPC(int i);
    int LoadProgram(QString fn);
    bool Eat();
    void Execute();

};

#endif // MOS6502_H
