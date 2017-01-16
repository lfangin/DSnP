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
#include <algorithm>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
CirGate* CirGate::getfanin(size_t in_num) const{
  return (CirGate*)(size_t(_faninList[in_num])& ~size_t(NEG));
}
CirGate* CirGate::getfanout(size_t in_num) const{
  return (CirGate*)_fanoutList[in_num];
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
  if(_type == PI_GATE || _type == CONST_GATE){
    if(!inDFS){
      cirMgr->dfs_push(_gateID);
      this->set_inDFS() ;
    }
    return;
  }
  for(size_t i = 0; i < faninsize();i++){
    if(!getfanin(i)-> getflag()){
        getfanin(i)-> setflag();
        getfanin(i)-> dfstraversal();
    }
  }
  this->set_inDFS();
  cirMgr -> dfs_push(_gateID);
  //if(_type!= PO_GATE )
}

void CirGate::cleanFanin(){
  //cout << "before cleanFanin"<<endl;
  CirGate* gin;
  for(size_t i =0;i<faninsize();i++){
    //cout << i<<" fanin"<<endl;
    gin = getfanin(i);
    //cout << "faninlist size"<<faninsize()<<endl;;
    //cout << gin->getgateID()<<"id of gin"<<endl;
    for(size_t j =0;j < gin->fanoutsize();j++){
      //cout << j<<" fanout fanoutsize"<< gin->fanoutsize()<<endl;
      //cout << gin->getfanout(j)->getType()<<endl;
      if(gin->getfanout(j)->getgateID()==_gateID){
        gin->erasefanout(j);
        break;
      }
    }
    //if(gin->getType()==UNDEF_GATE)
      //if(gin->fanoutsize()==0)
        //cirMgr->revFloat(gin->getgateID());
  }
  _faninList.resize(0);
}

void CirGate::erasefanout(unsigned n){
  size_t s = _fanoutList.size();
  //cout << "fanoutsize" << s<<endl;
  //cout << "before erasefanout"<<'\n';
  swap(_fanoutList[n],_fanoutList[s-1]);
  _fanoutList.resize(s-1);
  //cout << "after erasefanout"<<'\n';
}

void CirGate::erasefanin(unsigned n){
  size_t s = _faninList.size();
  swap(_faninList[n],_faninList[s-1]);
  _faninList.resize(s-1);
}

void CirGate::resetfanin(CirGate* r,bool& org_phase){
  CirGate* tmp;
  bool phase = 0;
   for(size_t i =0;i<_fanoutList.size();i++){
     tmp = _fanoutList[i];
     for(int j=0; j < tmp->faninsize();j++){
       if(_gateID==tmp->getfanin(j)->getgateID()){
         if(tmp->isInv(j)) phase = 1;
         tmp->erasefanin(j);
         //cout << "erase "<<_gateID<<"from fanout" << tmp->getgateID()<<endl;
         break;
       }
     }
     org_phase = (phase^org_phase);
     //if(org_phase) cout<<"I did set the phase errrrrr"<<endl;
     if(!r) tmp -> setfanin(cirMgr->getGate(0),org_phase);
     else tmp -> setfanin(r,org_phase);
   }
   _faninList.resize(0);
}//reset the faninlist of fanout
void CirGate::resetfanout(CirGate* r ){
  CirGate* tmp;
   for(size_t i =0;i<_faninList.size();i++){
     tmp = _faninList[i];
     for(int j=0; j < tmp->fanoutsize();j++){
       if(_gateID== tmp->getfanout(j)->getgateID()){
         tmp->erasefanout(j);
         break;
       }
     }
     if(!r) tmp -> setfanout(cirMgr->getGate(0));
     else tmp -> setfanout(r);
   }
   _faninList.resize(0);
}//reset the fanoutlist of fanin
