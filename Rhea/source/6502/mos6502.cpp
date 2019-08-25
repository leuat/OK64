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
        for (int i=1;i<fields.length();i++) {
            uchar op =(uchar)fields[i].trimmed().toInt(&ok,16);
            if (op!=0)
                m_opcodes[op] = Opcode(fields[0],i-1);
        }
       //     b.append((uchar)fields[i].trimmed().toInt(&ok,16));


        //m_opcodes[op] = new QByteArray(b);
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

void Mos6502::push(unsigned char c)
{
    r.sp++;
    m.m_data[r.sp]=c;
}

unsigned char Mos6502::pop()
{
    r.sp--;
    return m.m_data[r.sp+1];
}

void Mos6502::pushI(ushort c)
{
    push(c>>8);
    push(c&0xFF);
}

ushort Mos6502::popI()
{
   return pop() | pop()<<8;
}

/*bool Mos6502::Evaluate(unsigned char instruction, QString op, int type)
{
    return ((uchar)m_opcodes[op]->at(type)==(uchar)instruction);
}
*/
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

    if (instruction == 0x4C) { // jmp abs
          r.pc = getAbs();
      //    qDebug() << "jmp " << Util::numToHex(r.pc);
          return true;
      }
      if (instruction == 0xee) { // inc abs
          int pos = getAbs();
    //      qDebug() << "inc " << pos;
          m.m_data[pos] = m.m_data[pos]+1;
          return true;
      }
      if (instruction == 0xD0) { // bne d0 rel
          uchar pos = getImm();
          if (r.Z!=1) r.pc = r.pc+(char)pos;
          return true;
      }
      if (instruction == 0x90) { // bcc abs egentlig rel
/*          ushort pos = getAbs();
          if (r.C==0) r.pc = pos;*/
          uchar pos = getImm();
          if (r.C==0) {
              r.pc = r.pc+(char)pos;
           }
          return true;
      }
      if (instruction == 0xcd) { // cmp abs
          int pos = getAbs();
    //      qDebug() << "cmp " << pos;
          uchar cmp = m.m_data[pos]-r.a;

          if (cmp>127) { r.N = 1; r.C = 1; } else {r.C = 0; r.N=0;}
          if (cmp==0) {r.Z = 1;} else {r.Z= 0;}
          return true;
      }
      if (instruction == 0xc9) { // cmp imm
          uchar val = getImm();
          int cmp = (uchar)val - (uchar)r.a;
          if (cmp<0) { r.N = 1; r.C = 1; } else {r.C = 0; r.N=0;}

          if (cmp==0) {r.Z = 1;} else {r.Z= 0;}
          return true;
      }
      if (instruction == 0xad) { // lda abs
          r.a = m.m_data[getAbs()];
    //      qDebug() << "lda (abs) " << Util::numToHex(r.pc);
          return true;
      }

      if (instruction == 0xA9) { // lda imm
          r.a = getImm();
   //       qDebug() << "lda (imm) " << Util::numToHex(r.a);
          return true;
      }

      if (instruction == 0x49) { // eor imm
          r.a = r.a^getImm();
   //       qDebug() << "lda (imm) " << Util::numToHex(r.a);
          return true;
      }
      if (instruction == 0x4D) { // eor abs
          r.a = r.a^m.m_data[getAbs()];
   //       qDebug() << "lda (imm) " << Util::numToHex(r.a);
          return true;
      }

      if (instruction == 0x20) { // jsr abs
          ushort pos = getAbs();
//          r.pc+=2;
          pushI(r.pc);
          r.pc = pos;
          return true;
      }
      if (instruction == 0x6D) { // adc abs
          ushort pos = getAbs();
          r.a += m.m_data[pos];
          return true;
      }

      if (instruction == 0x60) { // rts
          r.pc = popI();
          return true;
      }

      if (instruction == 0x18) { // clc
          r.C = 0;
//          r.pc++;
          return true;
      }
      if (instruction == 0x0A) { // asl
          r.C = (r.a&128==128);
          r.a<<=1;
          r.setZ();
          return true;
      }

      if (instruction == 0x8D) { // sta abs
          uint pos = getAbs();

          m.m_data[pos] = r.a;
    //      qDebug() << "sta (abs) " << Util::numToHex(pos);

          return true;
      }

    qDebug() << "UNKNOWN opcode " << Util::numToHex(instruction);
    exit(1);
}

void Mos6502::Execute()
{
    SetPC(Constants::c.programStartAddress);
    bool ok = true;
    while (ok)
        ok = Eat();
}

void MOS6502Registers::setZ()
{
    Z = a==0;
}
