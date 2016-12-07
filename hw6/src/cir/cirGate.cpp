/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
CirGate* CirGate::getfanin(size_t in_num) const{
  return (CirGate*)(size_t(_faninList[in_num])& ~size_t(NEG));
}
//CirGate* CirGate::getfanout(size_t out_num) const{}

void CirGate::setfanin(CirGate* g,size_t phase ){
  g = (CirGate*)(size_t(g) + phase);
  _faninList.push_back(g);
}
void CirGate::setfanout(CirGate* g){
  _fanoutList.push_back(g);
}

void
CirGate::reportGate() const
{
  stringstream tmp;
  tmp << "= " << getTypeStr() << "(" << _gateID << ")";
  if(_symbol != "") tmp << "\"" << _symbol << "\"";
  tmp << ", line " << _lineNo ;
  string str = tmp.str();
  str.resize(49,' ');
  cout << "==================================================" << endl;
  cout << str << "=" <<endl;
  cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   if(_type == UNDEF_GATE) reportGate();
   else dofanin(level,level,false);
   cirMgr -> resetallflag();
}

void CirGate::dofanin(int level,unsigned count, bool inv) const{
  for(int i = count ; i!= level; i++)
    cout << "  ";
  if(inv) cout <<'!';
  cout << getTypeStr() << ' ' << getgateID();

  bool donext = true;
  unsigned n = 0;
  for(size_t j = 0; j < _faninList.size();j++){
    if(getfanin(j) -> getflag()) n++;
    if(n == _faninList.size()&& flag) donext = false;
  }
  if(!donext && count!=0) cout << " (" << '*' << ")";
  cout << endl;
  if(donext && count!=0){
    for(size_t j = 0; j < _faninList.size();j++){
      getfanin(j) -> dofanin(level,count-1,isInv(j));
    }
  }
  flag = true;
}
void CirGate::dofanout(int level,unsigned count, bool inv) const{
  for(int i = count ; i!= level; i++)
    cout << "  ";
  if(inv) cout <<'!';
  cout << getTypeStr() << ' ' << getgateID();

  bool donext = true;
  unsigned n = 0;
  for(size_t j = 0; j < _fanoutList.size();j++){
    if(_fanoutList[j] -> getflag()) n++;
  }
  if(n == _fanoutList.size()&& flag) donext = false;
  if(!donext && count!=0) cout << " (" << '*' << ")";
  cout << endl;
  if(donext && count!=0){
    for(size_t j = 0; j < _fanoutList.size();j++){
      _fanoutList[j] -> dofanout(level,count-1,fanoutinv(_fanoutList[j]));
    }
  }
  flag = true;
}

bool CirGate::fanoutinv(CirGate* out) const{
  for(size_t i = 0; i < out->faninsize();i++)
    if(out->getfanin(i)->getgateID() == _gateID)
      return out -> isInv(i);
  return false;
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   if(_type == UNDEF_GATE) reportGate();
   else dofanout(level,level,false);
   cirMgr -> resetallflag();
}

void CirGate::dfstraversal()const{
  if(_type == PI_GATE || _type == CONST_GATE) return;
  for(size_t i = 0; i < faninsize();i++){
    if(!getfanin(i)-> getflag()){
        getfanin(i)-> setflag();
        getfanin(i)-> dfstraversal();
    }
  }
  if(_type!= PO_GATE )
    cirMgr -> dfs_push(_gateID);
}
