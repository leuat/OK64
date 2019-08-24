#ifndef MOS6502_H
#define MOS6502_H
#include <QByteArray>
#define uchar unsigned char
class MOS6502Registers {
public:
    int a,x,y;
    int sp, pc;
    int N,V,B,D,I,Z,C;

};

class Memory {
public:
    QByteArray m_data;
};


class Mos6502
{
public:
    MOS6502Registers r;
    Memory m;
    Mos6502();
    void Initialize();

    void Eat();

};

#endif // MOS6502_H
