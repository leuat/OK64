#ifndef MOS6502_H
#define MOS6502_H
#include <QByteArray>
#include <QMap>
#include <QDebug>
#include "source/misc/util.h"
#include "source/6502/impl.h"
/*extern "C" {
    #include "source/s6502/cpu.h"
}*/
#include "source/okmemory.h"

#define uchar unsigned char



class MOS6502Registers {
public:
    uchar a=0,x=0,y=0;
    ushort sp=0xFF, pc=0x400;
    int N=0,V=0,B=0,D=0,I=0,Z=0,C=0;

    void setZN(uchar v);
    void setC(uchar v);
    uchar toFlag();
    void fromFlag(uchar f);
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
    const int izy = 6;
    const int abs = 7;
    const int abx = 8;
    const int aby = 9;
    const int ind = 10;
    const int rel = 11;

    mos6502* m_impl = nullptr;
    MOS6502Registers r;
    static OKMemory* m;
    uchar cmp, cpx, cpy;
    Mos6502() {}
    void Initialize(OKMemory* pram);
    QString GetAddressOrSymbol(ushort pos);
//    QMap<QString, QByteArray*> m_opcodes;
    QMap<uchar, Opcode> m_opcodes;
    void LoadOpcodes();
    QMap<ushort, QString> m_symbols;
    QVector<ushort> m_breakPoints;



    static void BusWrite(uint16_t a, uint8_t v) {
        m->set(a,v);
    }
    static uint8_t BusRead(uint16_t a) {
        return m->get(a);
    }


  //  uint getAbs();
  //  uchar getImm();
   // void push(uchar c);
  //  uchar pop();
    void LoadSybols(QString symFile);
    int m_cycles = 0;
    void ClearCycles();
    void SpendCycles(int cnt);



//    void pushI(ushort c);
//    ushort popI();


//    bool Evaluate(uchar instruction, QString op,int type);
    void SetPC(int i);
    bool Eat();
    void Execute();
    QString getInstructionAt(ushort& pc);

};

#endif // MOS6502_H
