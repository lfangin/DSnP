/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include <algorithm>

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  //Strashing: 4 merging 5...
  size_t s=_dfsList.size();
  size_t fi0,fi1;
  HashMap<HashKey,CirGate*> h(getHashSize(s));
  for(size_t i=0; i<s ;i++){
    CirGate* g = id2gate[_dfsList[i]];
    CirGate* r = g;
    //r as input of insert(),if check() r becomes the gate to replace
    //
    if(g->getType()!=AIG_GATE) continue;
    fi0 = 2*(g->getfanin(0)->getgateID() <<1);
    fi1 = 2*(g->getfanin(1)->getgateID() <<1);
    if(g->isInv(0)) fi0++;
    if(g->isInv(1)) fi1++;
    if(fi1<fi0) swap(fi0,fi1);
    HashKey k(fi0,fi1);
    //cout << "gate " << r->getgateID() << endl;
    if(!h.insert(k,r)){
      g->resetfanin(r,0);
      g->resetfanout(r);
      cout << "Strashing: " << r->getgateID() <<" merging ";
      //if(g->isInv(pos)) cout << '!';
      cout << g->getgateID() <<"..." <<endl;
      id2gate[_dfsList[i]] = 0;
    }
    //if(_dfsList[i] == 773) break;//debug message
  }
  //id2gate[773]->reportFanin(3);
  //id2gate[686]->reportFanin(3);
  dfs();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
