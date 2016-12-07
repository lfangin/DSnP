/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
     if(gid < (index[0]+index[3]+1)) return id2gate[gid];
     else return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates();
   //const;
   void writeAag(ostream&) const;
   void resetallflag()const ;
   void dfs_push(unsigned n){
     _dfsList.push_back(n);
   }
   void dfs()const;
private:
  unsigned index[5];
  vector< vector<unsigned> > _totalList;
  GateList id2gate;
  IdList _floatinglist;
  IdList _dfsList;
  unsigned myStr2Unsigned(const string& str);
  void getTokens(const string&, vector<string>&);
  bool readHeader( string& h);
  void setNet(CirGate* G, unsigned gin);
  //void cleardfs()const { _dfsList.clear(); }

};

#endif // CIR_MGR_H
