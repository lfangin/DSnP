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
#include <algorithm>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { return id2gate[gid]; }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() ;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

   void resetallflag()const ;
   void dfs_push(unsigned n){
     _dfsList.push_back(n);
   }
   void dfs();
   void resetalldfs()const;
   //void revFloat(unsigned id);

private:
   ofstream           *_simLog;

   unsigned index[5];
   vector< vector<unsigned> > _totalList;
   GateList id2gate;
   IdList _floatinglist;
   IdList _dfsList;
   IdList notusedList;
   unsigned myStr2Unsigned(const string& str);
   void getTokens(const string&, vector<string>&);
   bool readHeader( string& h);
   void setNet(CirGate* G, unsigned gin);
};

#endif // CIR_MGR_H
