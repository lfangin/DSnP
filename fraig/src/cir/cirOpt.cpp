/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void CirMgr::sweep(){
  size_t s = id2gate.size();
  for(size_t i = 0; i<s;i++){
    CirGate* g = id2gate[i];
    if(!g) continue;
    //cout << g->getgateID() << '\n';
    if(!g->get_inDFS()){
      //cout<<"not in dfs"<<endl;
      g->cleanFanin();
      //cout << "afterclean fanin"<<'\n';
      g->cleanFanout();
      //cout << "afterclean fanout"<<'\n';
      if(id2gate[i]->getType()!= PI_GATE&&id2gate[i]->getType()!= CONST_GATE){
        cout << "Sweeping: "<< id2gate[i]->getTypeStr()<< '('
          <<id2gate[i]->getgateID()<<") removed..."<<'\n';
        id2gate[i] = 0;
      }
    }
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void CirMgr::optimize(){
  size_t s = _dfsList.size();
  //for(size_t i =0;i < s;i++)
    //cout << _dfsList[i]<<endl;

  for(size_t i =0;i < s;i++){
    CirGate* g = id2gate[_dfsList[i]];
    if(!g) continue;
    if(g->getType()!=AIG_GATE) continue;
    bool with0 = false;
    bool with1 = false;
    size_t tmp ; //const at tmp;
    size_t pos ;
    for(tmp = 0;tmp < 2;tmp++){
      //cout << "fanin of "<< g->getgateID() <<"is"<<g->getfanin(tmp)->getgateID();
      //if(g->isInv(tmp)) cout<<"!";
      //cout<<'\n';

      if(g->getfanin(tmp)->getType()==CONST_GATE){
        if(g->isInv(tmp)) with1 = true;
        else with0 = true;
        break;
      }
    }
    if(tmp!=2){
      pos = (tmp==1)?0:1;//with const,non-const at pos
      //if(getfanin(pos)->getType()==UNDEF_GATE) revFL = true;
    }
    else if((g->getfanin(0)->getgateID())==(g->getfanin(1)->getgateID())){
      if(!(g->isInv(0))==(g->isInv(1))) with0 = true;
      else{ with1 = true; pos = 0; }
    }
    if(with0){
      //bool p = 0;
      g->resetfanin(0,0);
      g->resetfanout();
      //if(revFL) revFloat(i);
      cout << "Simplifying: 0 merging " <<  g->getgateID() << "..."<<endl;
      id2gate[_dfsList[i]] = 0;
    }
    else if(with1){
      CirGate* r = g->getfanin(pos);
      bool p = g->isInv(pos);
      g->resetfanin(r,p);
      g->resetfanout(r);
      cout << "Simplifying: " << r->getgateID() <<" merging ";
      if(g->isInv(pos)) cout << '!';
      cout << g->getgateID() <<"..." <<endl;
      //if(revFL){revFloat(i); _floatinglist.push_back();}
      id2gate[_dfsList[i]] = 0;
    }
  }
  dfs();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
