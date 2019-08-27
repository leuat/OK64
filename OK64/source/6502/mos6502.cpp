#include "mos6502.h"
#include <QFile>
#include <QTextStream>

Constants Constants::c;
Memory Mos6502::m;


void Mos6502::Initialize()
{
    m.m_data.resize(65536);
    m_impl = new mos6502(BusRead, BusWrite);
    m_impl->pc = 0x400;
}

QString Mos6502::GetAddressOrSymbol(ushort pos)
{
    if (m_symbols.contains(pos)) {
        return m_symbols[pos];// + " (" + Util::numToHex(pos) +")";
    }
    return Util::numToHex(pos);
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

void Mos6502::LoadSybols(QString symFile)
{
    QFile file(symFile);
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine().trimmed().simplified();
        if (line.startsWith(";"))
            continue;

        QStringList s = line.split(" ");
        if (s[0].toLower()=="al") {
            m_symbols[Util::NumberFromStringHex(s[1])] = s[2].remove(0,1);
        }


    }
}

void Mos6502::ClearCycles()
{
    m_cycles=0;
}

void Mos6502::SpendCycles(int cnt)
{
    m_cycles+=cnt;
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
    m_impl->Run(1);

    r.a = m_impl->A;
    r.x = m_impl->X;
    r.y = m_impl->Y;
    r.pc = m_impl->pc;
    return true;



    ushort pc = r.pc;
//    qDebug() << getInstructionAt(pc).replace("&nbsp;"," ");

    uchar instruction = m.m_data[r.pc];
    r.pc++;
    if (instruction == 0x4C) { // jmp abs
        SpendCycles(3);
        r.pc = getAbs();

        return true;
    }
    if (instruction == 0xee) { // inc abs
        int pos = getAbs();
        SpendCycles(6);
        //      qDebug() << "inc " << pos;
        m.m_data[pos] = m.m_data[pos]+1;
        return true;
    }
    if (instruction == 0xce) { // dec abs
        int pos = getAbs();
        SpendCycles(6);
        //      qDebug() << "inc " << pos;
        m.m_data[pos] = m.m_data[pos]-1;
        return true;
    }
    if (instruction == 0xde) { // dec abs,x
        int pos = getAbs();
        SpendCycles(7);
        //      qDebug() << "inc " << pos;
        m.m_data[pos+r.x] = m.m_data[pos+r.x]-1;
        return true;
    }
    if (instruction == 0xfe) { // inc abs,x
        int pos = getAbs();
        SpendCycles(7);
        //      qDebug() << "inc " << pos;
        m.m_data[pos+r.x] = m.m_data[pos+r.x]+1;
        return true;
    }
    if (instruction == 0xD0) { // bne d0 rel
        char pos = getImm();
        int cyc = 2;
        if (r.Z!=1) { r.pc = r.pc+(char)pos; cyc++; }
        SpendCycles(cyc);
        return true;
    }
    if (instruction == 0xF0) { // beq d0 rel
        char pos = getImm();
        int cyc = 2;
        if (r.Z==1) { r.pc = r.pc+(char)pos; cyc++; }
        SpendCycles(cyc);
        return true;
    }
    if (instruction == 0x90) { // bcc abs egentlig rel
        /*          ushort pos = getAbs();
          if (r.C==0) r.pc = pos;*/
        char pos = getImm();
        int cyc = 2;
        if (r.C==0) {
            cyc++;
            r.pc = r.pc+(char)pos;
        }
        SpendCycles(cyc);
        return true;
    }
    if (instruction == 0xb0) { // bcs abs egentlig rel
        /*          ushort pos = getAbs();
          if (r.C==0) r.pc = pos;*/
        char pos = getImm();
        int cyc = 2;
        if (r.C!=0) {
            cyc++;
            r.pc = r.pc+(char)pos;
        }
        SpendCycles(cyc);
        return true;


    }
    if (instruction == 0x30) { // bmi abs egentlig rel
        /*          ushort pos = getAbs();
          if (r.C==0) r.pc = pos;*/
        char pos = getImm();
        int cyc = 2;
        if (r.N==1) {
            cyc++;
            r.pc = r.pc+(char)pos;
        }
        SpendCycles(cyc);
        return true;
    }
    if (instruction == 0x10) { // bpl abs egentlig rel
        /*          ushort pos = getAbs();
          if (r.C==0) r.pc = pos;*/
        char pos = getImm();
        int cyc = 2;
        if (r.N==0) {
            cyc++;
            r.pc = r.pc+(char)pos;
        }
        SpendCycles(cyc);
        return true;
    }
    if (instruction == 0xcd) { // cmp abs
        int pos = getAbs();
        //      qDebug() << "cmp " << pos;
        uchar cmp = m.m_data[pos]-r.a;
        SpendCycles(4);
        if (cmp>127) { r.N = 1; r.C = 1; } else {r.C = 0; r.N=0;}
        if (cmp==0) {r.Z = 1;} else {r.Z= 0;}
        return true;
    }
    if (instruction == 0xdd) { // cmp abs,x
        int pos = getAbs();
        uchar cmp = m.m_data[pos+r.x]-r.a;
        SpendCycles(4);
        if (cmp>127) { r.N = 1; r.C = 1; } else {r.C = 0; r.N=0;}
        if (cmp==0) {r.Z = 1;} else {r.Z= 0;}
        return true;
    }
    if (instruction == 0xd9) { // cmp abs,y
        int pos = getAbs();
        uchar cmp = m.m_data[pos+r.y]-r.a;
        SpendCycles(4);
        if (cmp>127) { r.N = 1; r.C = 1; } else {r.C = 0; r.N=0;}
        if (cmp==0) {r.Z = 1;} else {r.Z= 0;}
        return true;
    }
    if (instruction == 0xc9) { // cmp imm
        uchar val = getImm();
        SpendCycles(2);
        int cmp = (uchar)val - (uchar)r.a;
        if (cmp<0) { r.N = 1; r.C = 1; } else {r.C = 0; r.N=0;}

        if (cmp==0) {r.Z = 1;} else {r.Z= 0;}
        return true;
    }
    if (instruction == 0xe0) { // cpx imm
        uchar val = getImm();
        SpendCycles(2);
        int cmp = (uchar)val - (uchar)r.x;
        if (cmp<0) { r.N = 1; r.C = 1; } else {r.C = 0; r.N=0;}

        if (cmp==0) {r.Z = 1;} else {r.Z= 0;}
        return true;
    }
    if (instruction == 0xad) { // lda abs
        r.a = m.m_data[getAbs()];
        SpendCycles(4);
        r.setZN(r.a);
        //      qDebug() << "lda (abs) " << Util::numToHex(r.pc);
        return true;
    }
    if (instruction == 0xbd) { // lda abs,x
        r.a = m.m_data[getAbs()+r.x];
        SpendCycles(5);
        r.setZN(r.a);
        //      qDebug() << "lda (abs) " << Util::numToHex(r.pc);
        return true;
    }
    if (instruction == 0xb9) { // lda abs,y
        r.a = m.m_data[getAbs()+r.y];
        SpendCycles(5);
        r.setZN(r.a);
        //      qDebug() << "lda (abs) " << Util::numToHex(r.pc);
        return true;
    }
    if (instruction == 0xA9) { // lda imm
        SpendCycles(2);
        r.a = getImm();
        r.setZN(r.a);
        //       qDebug() << "lda (imm) " << Util::numToHex(r.a);
        return true;
    }

    if (instruction == 0xB5) { // lda zp,x
        uchar zp = getImm();
        r.a = m.m_data[zp+r.x];
  //      qDebug() << "sta (abs) " << Util::numToHex(pos);

        return true;
    }


    if (instruction == 0xA0) { // ldy imm
        SpendCycles(2);
        r.y = getImm();
        r.setZN(r.y);
        return true;
    }
    if (instruction == 0xA2) { // ldx imm
        SpendCycles(2);
        r.x = getImm();
        r.setZN(r.x);
        return true;
    }
    if (instruction == 0xAE) { // ldx abs
        SpendCycles(4);
        r.x = m.m_data[getAbs()];
        r.setZN(r.x);
        return true;
    }
    if (instruction == 0xAC) { // ldy abs
        SpendCycles(4);
        r.y = m.m_data[getAbs()];
        r.setZN(r.y);
        return true;
    }
    if (instruction == 0xBC) { // ldy abs,x
        SpendCycles(4);
        r.y = m.m_data[getAbs()+r.x];
        r.setZN(r.y);
        return true;
    }




    if (instruction == 0x49) { // eor imm
        r.a = r.a^getImm();
        SpendCycles(2);
        return true;
    }
    if (instruction == 0x4D) { // eor abs
        r.a = r.a^m.m_data[getAbs()];
        SpendCycles(4);
        return true;
    }
    if (instruction == 0x05) { // ora zp
        r.a = r.a|m.m_data[getImm()];
        r.setZN(r.a);
        SpendCycles(3);
        return true;
    }
    if (instruction == 0x09) { // ora imm
        r.a = r.a|getImm();
        r.setZN(r.a);
        SpendCycles(2);
        return true;
    }
    if (instruction == 0x0D) { // ora abs
        r.a = r.a|m.m_data[getAbs()];
        r.setZN(r.a);
        SpendCycles(4);
        return true;
    }
    if (instruction == 0x1D) { // ora abs,x
        r.a = r.a|m.m_data[getAbs()+r.x];
        r.setZN(r.a);
        SpendCycles(4);
        return true;
    }
    if (instruction == 0x29) { // and imm
        r.a = r.a&getImm();
        r.setZN(r.a);
        SpendCycles(2);
        return true;
    }
    if (instruction == 0x2D) { // and abs
        r.a = r.a&m.m_data[getAbs()];
        r.setZN(r.a);
        SpendCycles(4);
        return true;
    }
    if (instruction == 0x3D) { // and abs,x
        r.a = r.a&m.m_data[getAbs()+r.x];
        r.setZN(r.a);
        SpendCycles(4);
        return true;
    }


    if (instruction == 0x25) { // and zp
        r.a = r.a&m.m_data[getImm()];
        r.setZN(r.a);
        SpendCycles(3);
        return true;
    }

    if (instruction == 0x65) { // adc zp
        ushort pos = getImm();
        SpendCycles(4);
        r.setC(((int)r.a+m.m_data[pos])>255);
        r.a += m.m_data[pos];
//           N V Z C
        r.setZN(r.a);
        return true;
    }

    if (instruction == 0x20) { // jsr abs
        ushort pos = getAbs();
//          r.pc+=2;
           SpendCycles(6);
          pushI(r.pc);
          r.pc = pos;
          return true;
      }
      if (instruction == 0x6D) { // adc abs
          ushort pos = getAbs();
          SpendCycles(4);
          r.setC(((int)r.a+m.m_data[pos])>255);
          r.a += m.m_data[pos];
//           N V Z C
          r.setZN(r.a);
          return true;
      }
      if (instruction == 0xED) { // sbc abs
          ushort pos = getAbs();
          SpendCycles(4);
          r.setC(((int)r.a-m.m_data[pos])<0);
          r.a -= m.m_data[pos];
//           N V Z C
          r.setZN(r.a);
          return true;
      }
      if (instruction == 0x7D) { // adc abs,x
          ushort pos = getAbs()+r.x;
          SpendCycles(5);
          r.setC(((int)r.a+m.m_data[pos])>255);
          r.a += m.m_data[pos];
          r.setC(r.a==0);
//           N V Z C
          r.setZN(r.a);
          return true;
      }
      if (instruction == 0x69) { // adc imm
          uchar val = getImm();
          SpendCycles(2);
          r.setC(((int)r.a+val)>255);
          r.a += val;

          r.setZN(r.a);
          return true;
      }
      if (instruction == 0xe9) { // sbc imm
          uchar val = getImm();
          SpendCycles(2);
          r.setC(((int)r.a-val)<0);
          r.a -= val;

          r.setZN(r.a);
          return true;
      }
      if (instruction == 0xe5) { // sbc zp
          uchar val = m.m_data[getImm()];
          SpendCycles(2);
          r.setC(((int)r.a-val)<0);
          r.a -= val;

          r.setZN(r.a);
          return true;
      }

      if (instruction == 0x60) { // rts
          SpendCycles(6);
          r.pc = popI();
          return true;
      }
      if (instruction == 0x40) { // rti
          SpendCycles(6);
//          r.fromFlag(pop());
          r.pc = popI();
          return true;
      }

      if (instruction == 0x18) { // clc
          r.C = 0;
          SpendCycles(2);
//          r.pc++;
          return true;
      }
      if (instruction == 0x38) { // clc
          r.C = 1;
          SpendCycles(2);
//          r.pc++;
          return true;
      }

      if (instruction == 0xA8) { // tay
          r.y = r.a;
          return true;
      }
      if (instruction == 0xAA) { // tax
          r.x = r.a;
          return true;
      }
      if (instruction == 0x8a) { // txa
          r.a = r.x;
          return true;
      }
      if (instruction == 0x98) { // tya
          r.a = r.y;
          return true;
      }
      if (instruction == 0x9A) { // txs
          r.sp = r.x;
          return true;
      }
      if (instruction == 0xbA) { // tsx
          r.x = r.sp;
          return true;
      }

      if (instruction == 0x0A) { // asl
          r.C = ((r.a&128)==128);
          r.a<<=1;
          r.setZN(r.a);
          SpendCycles(2);

          return true;
      }
      if (instruction == 0x4a) { // lsr
          r.C = ((r.a&1)==1);
          r.a>>=1;
          //qDebug() << r.a;
          r.setZN(r.a);
          SpendCycles(2);

          return true;
      }

      if (instruction == 0x8D) { // sta abs
          uint pos = getAbs();
          SpendCycles(4);

          m.m_data[pos] = r.a;
    //      qDebug() << "sta (abs) " << Util::numToHex(pos);

          return true;
      }
      if (instruction == 0x8C) { // sta abs
          uint pos = getAbs();
          SpendCycles(4);
          m.m_data[pos] = r.y;

          return true;
      }
      if (instruction == 0x8E) { // stx abs
          uint pos = getAbs();
          SpendCycles(4);
          m.m_data[pos] = r.x;

          return true;
      }


      if (instruction == 0xB1) { // lda (zp),y
          uchar zp = getImm();
          uint pos =m.m_data[zp] |(m.m_data[zp+1]<<8);
//          qDebug() << "lda izy " << Util::numToHex(zp) << Util::numToHex(pos);
          SpendCycles(5);
//          m.m_data[pos+r.y] = r.a;
          r.a = m.m_data[pos+r.y];
          r.setZN(r.a);
          return true;
      }
      if (instruction == 0x91) { // sta (zp),y
          uchar zp = getImm();
          uint pos =m.m_data[zp] |(m.m_data[zp+1]<<8);
//          qDebug() << "lda izy " << Util::numToHex(zp) << Util::numToHex(pos);
          SpendCycles(5);
//          r.pc++;
//          m.m_data[pos+r.y] = r.a;
          m.m_data[pos+r.y] = r.a;

          return true;
      }


      if (instruction == 0xC8) { //iny
          r.y++;
          r.setZN(r.y);
          r.setC(r.y==0);
          return true;
       }
      if (instruction == 0xE8) {
          r.x++;
          r.setZN(r.x);
          r.setC(r.x==0);
          return true;
       }
      if (instruction == 0x88) {
          r.y--;
          r.setZN(r.y);
          r.setC(r.y==255);

          return true;
       }
      if (instruction == 0xca) {
          r.x--;
          r.setZN(r.x);
          r.setC(r.x==255);
          return true;
       }

      if (instruction == 0x9D) { // sta abs,x
          ushort pos = getAbs();
          m.m_data[pos+r.x]=r.a;
          SpendCycles(5);
          return true;
      }
      if (instruction == 0x99) { // sta abs,y
          ushort pos = getAbs();
          m.m_data[pos+r.y]=r.a;
          SpendCycles(5);
          return true;
      }

      if (instruction == 0x85) { // sta zp
          uchar zp = getImm();
          m.m_data[zp] = r.a;
    //      qDebug() << "sta (abs) " << Util::numToHex(pos);

          return true;
      }
      if (instruction == 0x95) { // sta zp,x
          uchar zp = getImm();
          m.m_data[zp+r.x] = r.a;
    //      qDebug() << "sta (abs) " << Util::numToHex(pos);

          return true;
      }
//    if (instruction==0xff)
  //      return true;
    qDebug() << "UNKNOWN opcode " << Util::numToHex(instruction);
    return false;
//    exit(1);
}

void Mos6502::Execute()
{
    SetPC(Constants::c.programStartAddress);
    bool ok = true;
    while (ok)
        ok = Eat();
}

QString Mos6502::getInstructionAt(ushort &pc)
{
    Opcode cur = m_opcodes[m.m_data[pc]];
    uchar b1 = m.m_data[pc+1];
    uchar b2 = m.m_data[pc+2];
    QString output;
    output+=Util::numToHex(pc)+":&nbsp;"+cur.m_name + "&nbsp;";
    if (cur.m_name.trimmed()=="") {
        pc++;
        return Util::numToHex(m.m_data[pc]);
    }

    if (cur.m_type == abs)
        output += GetAddressOrSymbol(b1|b2<<8);// + "  ("+Util::numToHex((uchar)m.m_data[b1|b2<<8]) + ")";
    if (cur.m_type == abx)
        output += GetAddressOrSymbol(b1|b2<<8)+","+Util::numToHex(r.x);// + "   ("+Util::numToHex((uchar)(m.m_data[b1|b2<<8+r.x])) + ")";
    if (cur.m_type == aby)
        output += GetAddressOrSymbol(b1|b2<<8)+","+Util::numToHex(r.y);// + "   ("+Util::numToHex((uchar)(m.m_data[b1|b2<<8+r.y])) + ")";
    if (cur.m_type == imm)
        output += "#"+Util::numToHex(b1);
    if (cur.m_type == zp)
        output += Util::numToHex(b1);
    if (cur.m_type == izx)
        output += Util::numToHex(b1)+",x ("+Util::numToHex(r.x)+")";
    if (cur.m_type == izy)
        output += Util::numToHex(b1)+",y ("+Util::numToHex(r.y)+")";

    if (cur.m_type == abs || cur.m_type == abx || cur.m_type == aby )
        pc+=3;
    if (cur.m_type == imm || cur.m_type == izx ||cur.m_type == zp || cur.m_type == zpx)
        pc+=2;
    if (cur.m_type == none)
        pc+=1;


    return output;
}

void MOS6502Registers::setZN(uchar c)
{
    Z = c==0;
    N = c>127;
}

void MOS6502Registers::setC(unsigned char v)
{
    C = v;
}

unsigned char MOS6502Registers::toFlag()
{
    return C | Z<<1 | I<<2 | D<<3 | B<<4 | 1<<5 | V<<6 | N<<7;
}

void MOS6502Registers::fromFlag(unsigned char f)
{
    C = (f&1) == 1;
    Z = (f&2) == 2;
    I = (f&4) == 4;
    D = (f&8) == 8;
    B = (f&16) == 16;
    V = (f&64) == 64;
    N = (f&128) == 128;
}
