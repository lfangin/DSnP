/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#define NEG 0x1
#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   //CirGate() {}
   CirGate( GateType _type,unsigned _gateId = 0, unsigned _lineNo = 0):
      _type(_type),_gateID(_gateId), _lineNo(_lineNo), flag(false),_symbol("") {}
   virtual ~CirGate() {}
   // Basic access methods
   string getTypeStr() const {
     switch(_type){
       case PI_GATE: return "PI";
       case PO_GATE: return "PO";
       case AIG_GATE: return "AIG";
       case UNDEF_GATE: return "UNDEF";
       case CONST_GATE: return "CONST";
       default: return "TOT";
     }
   }
   GateType getType() const{ return _type;}
   unsigned getLineNo() const { return _lineNo; }
   unsigned getgateID() const {return _gateID; }
   void resetflag() { flag = false; }
   void setflag() { flag = true;}
   bool getflag() { return flag;}
   string getSymbol() const { return _symbol;}
   void setSymbol(const string& str) { _symbol = str;}

   CirGate* getfanin(size_t) const;
   CirGate* getfanout(size_t) const;
   size_t fanoutsize() const { return _fanoutList.size();}
   size_t faninsize() const { return _faninList.size(); }
   void setfanin(CirGate*,size_t phase = 0);
   bool isInv(size_t in_num) const{ return (size_t(_faninList[in_num])& NEG); }
   void setfanout(CirGate*);
   bool fanoutinv(CirGate*)const;

   // Printing functions
   virtual void printGate(int& ) const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void dofanin(int level,unsigned count, bool inv) const;
   void dofanout(int level,unsigned count, bool inv) const;
   void dfstraversal()const;
private:

protected:
  GateType _type;
  unsigned _gateID;
  unsigned _lineNo;
  mutable bool flag;
  GateList _faninList;
  GateList _fanoutList;
  string _symbol;

};

class AIGgate : public CirGate
{
  public:
  AIGgate(unsigned _gateId = 0,unsigned _lineNo = 0):
    CirGate (AIG_GATE,_gateId, _lineNo ){};
  ~AIGgate() {}

  void printGate(int& count) const{
    if(flag) return;
    if(!_faninList.empty()){
      for(size_t i = 0;i!= _faninList.size();i++)
        getfanin(i) -> printGate(count);
    }
    //AIG 19 *!15 7
    cout << "[" << count << "] " << "AIG " << _gateID ;
    size_t n = 0;
    while(n!=2){
      cout << ' ';
      if((getfanin(n) -> getType()) == UNDEF_GATE) cout << "*";
      if(isInv(n)) cout << "!";
      cout << getfanin(n) -> getgateID();
      n++;
    }
    cout << endl;
    flag = true;
    count++;
  }

};

class PIgate : public CirGate
{
  public:
  PIgate(unsigned _gateId = 0,unsigned _lineNo = 0):
    CirGate (PI_GATE,_gateId, _lineNo){};
  ~PIgate() {}
  void printGate(int& count) const{
    //[0] PI  1 (1GAT)
    if(flag) return;
    cout << "[" << count << "] " << "PI  " << _gateID;
    if(_symbol!= "")
      cout << " (" << _symbol << ")";
    cout << endl;
    count++;
    flag = true;
  }

};

class POgate : public CirGate
{
  public:
  POgate(unsigned _gateId = 0,unsigned _lineNo = 0):
    CirGate (PO_GATE,_gateId , _lineNo ){};
  ~POgate() {}
  void printGate(int& count) const{
    if(!_faninList.empty()){
      for(size_t i = 0;i!= _faninList.size();i++)
        getfanin(i) -> printGate(count);
    }
    cout << "[" << count << "] " << "PO  " << _gateID << ' ';
    if((getfanin(0) -> getType()) == UNDEF_GATE) cout << "*";
    if(isInv(0)) cout << "!";
    cout << getfanin(0) -> getgateID();
    if(_symbol!= "")
      cout << " (" << _symbol << ")";
    cout << endl;
    count++;
    //flag = true;
  }

};

class CONSTgate : public CirGate
{
  public:
  CONSTgate(unsigned _gateId = 0,unsigned _lineNo = 0):
    CirGate (CONST_GATE,_gateId , _lineNo ){};
  ~CONSTgate() {}
  void printGate(int& count) const{
    if(flag) return;
    cout << "[" << count << "] " << "CONST0" << endl;
    count++;
    flag = true;
  }

};

class UNDEFgate : public CirGate
{
  public:
  UNDEFgate(unsigned _gateId = 0,unsigned _lineNo = 0):
    CirGate (UNDEF_GATE,_gateId , _lineNo ){};
  ~UNDEFgate() {}
  void printGate(int& count) const{
    flag = true;
    return;
  }

};




#endif // CIR_GATE_H
