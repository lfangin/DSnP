/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
//static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
  ifstream fs;
  vector<string> buffer;
  string line;
  fs.open(fileName.c_str());
  if(!fs.is_open()) return false;
  while(getline(fs,line)){
    buffer.push_back(line);
  }
  if(!readHeader(buffer[0])) return false;

  lineNo = 1;
  if(index[2]!=0) return false;

  id2gate.resize(index[0]+index[3]+1,0);
  id2gate[0] = new CONSTgate(0);

  for(unsigned i = 0 ; i < index[1];i++){
    vector<unsigned> tmp;
    tmp.push_back(myStr2Unsigned(buffer[lineNo]));
    _totalList.push_back(tmp);
    id2gate[tmp[0]/2] = new PIgate(tmp[0]/2,lineNo+1);
    lineNo++;
  }
  for(unsigned i = 0 ; i < index[3];i++){
    vector<unsigned> tmp;
    tmp.push_back(myStr2Unsigned(buffer[lineNo]));
    _totalList.push_back(tmp);
    id2gate[index[0]+i+1] = new POgate(index[0]+i+1,lineNo+1);
    lineNo++;
  }

  for(unsigned i = 0 ; i < index[4];i++){
    string tok;
    vector<unsigned> tmp;
    vector<string> t;
    getTokens(buffer[lineNo],t);
    for(size_t i = 0;i< t.size();i++)
      tmp.push_back(myStr2Unsigned(t[i]));
    _totalList.push_back(tmp);
    id2gate[tmp[0]/2] = new AIGgate(tmp[0]/2,lineNo+1);
    lineNo++;
  }
  while(1){
    if(lineNo == buffer.size()) break;
    string tok;
    //cout << buffer[lineNo] <<endl;
    size_t pos = myStrGetTok(buffer[lineNo],tok);
    if(tok == "c") break;
    else if(tok[0] == 'i'){
      //cout << tok[0].substr(1) <<endl;
      unsigned n = myStr2Unsigned(tok.substr(1));
      id2gate[_totalList[n][0]/2] -> setSymbol(buffer[lineNo].substr(pos+1));
    }
    else if(tok[0] == 'o'){
      unsigned n = myStr2Unsigned(tok.substr(1));
      id2gate[index[0]+1+n] -> setSymbol(buffer[lineNo].substr(pos+1));
    }
    //else return false;
    lineNo++;
  }

  for(unsigned i = 0;i < (index[3]);i++){
    CirGate* G = getGate(index[0]+1+i);
    setNet( G, _totalList[index[1]+i][0]);
  }
  for(unsigned i = (index[1]+index[3]);i < (index[1]+index[3]+index[4]);i++){
    CirGate* G = getGate(_totalList[i][0]/2);
    setNet(G,_totalList[i][1]);
    setNet(G,_totalList[i][2]);
  }

  fs.close();
  dfs();
  resetallflag();

  lineNo = 0;
  colNo = 0;
  return true;
}

void CirMgr::setNet(CirGate* G, unsigned gin){
  CirGate* Gin = getGate(gin/2);
  if(!Gin){
    unsigned tmp = G->getgateID();
    if(_floatinglist.size()==0) _floatinglist.push_back(tmp);
    else if(_floatinglist.back()!= tmp)
      _floatinglist.push_back(tmp);
    Gin = new UNDEFgate(gin/2);
  }
  G -> setfanin(Gin,gin%2);
  Gin -> setfanout(G);
}

bool CirMgr::readHeader( string& h){
  if(h.substr(0,3)!="aag"){
    parseError(ILLEGAL_IDENTIFIER);
    return false;
  }
  else{
    string id = h.substr(3);
    size_t white,pos;
    int diff ;
    int num;
    unsigned count = 0;
    //if(diff < 1){ parseError(MISSING_SPACE); return false; }
    //else if (diff >1){parseError(EXTRA_SPACE); return false; }

    white = id.find_first_of(' ',0);
    while(1){
      pos = id.find_first_not_of(' ',white);
      diff = pos - white;
      if(diff < 1){ parseError(MISSING_SPACE); return false; }
      else if (diff >1){ parseError(EXTRA_SPACE); return false; }

      white = id.find_first_of(' ',pos);
      if(white != string::npos){
        if(!myStr2Int(id.substr(pos,white-pos),num)){ parseError(ILLEGAL_NUM); return false;}
        index[count] = unsigned(num);
      }
      else{
        if(!myStr2Int(id.substr(pos),num)){ parseError(ILLEGAL_NUM); return false;}
        index[count] = unsigned(num);
        break;
      }
      count++;
      if(count == 5){ parseError(EXTRA_SPACE); return false;}
    }
    if(count < 4){ parseError(MISSING_NUM); return false;}
  }
  return true;
}



/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
  cout << '\n' <<"Circuit Statistics"<< endl;
  cout << "=================="<< endl;
  cout << "  PI" << setw(12) << right << index[1] << endl;
  cout << "  PO" << setw(12) << right << index[3] << endl;
  cout << "  AIG" << setw(11) << right << index[4] << endl;
  cout << "------------------" << endl;
  cout << "  Total" << setw(9) << right << index[1]+index[3]+index[4] << endl;
}

void
CirMgr::printNetlist() const
{
  int count = 0;
  cout << '\n';
  for(unsigned i = 0; i < index[3];i++)
    getGate(index[0]+1+i) -> printGate(count);
  resetallflag();
}

void CirMgr::resetallflag()const{
  for(unsigned i = 0; i < id2gate.size();i++){
    if(id2gate[i]==0) continue;
    id2gate[i] -> resetflag();
  }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(unsigned i = 0;i < index[1];i++)
     cout << " " << _totalList[i][0]/2 ;
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned i = 0;i < index[3];i++)
     cout << " " << index[0]+1+i ;
   cout << endl;
}

void
CirMgr::printFloatGates() //const
{
  if(_floatinglist.size()!=0){
    std::sort(_floatinglist.begin(),_floatinglist.end());
    cout << "Gates with floating fanin(s):";
    for(vector<unsigned>::iterator i = _floatinglist.begin(); i!= _floatinglist.end();++i)
      cout << " " << *i ;
    cout << endl;
  }
  vector<unsigned> notusedList;
  for(unsigned i = 1;i <= index[0];i++){
    if(!id2gate[i]){continue;}
    if(id2gate[i] -> fanoutsize() == 0)
      notusedList.push_back(i);
  }
  if(notusedList.size()!=0){
    cout << "Gates defined but not used  :";
    for(size_t i = 0;i < notusedList.size();i++)
      cout << ' ' << notusedList[i];
    cout << endl;
  }
  //cout <<endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{

  //filebuf fb;
  //stringstream ss;
  //ss << outfile.rdbuf();
  //if(outfile != cout) fb.open (ss.str(),std::ios::out);

  //for(unsigned i =0 ; i < _dfsList.size();i++)
    //cout << _dfsList[i] << ' ';
  outfile << "aag" ;
  for(unsigned i = 0;i !=4;i++)
    outfile << ' ' << index[i];
  outfile << ' ' << _dfsList.size() << '\n';
  for(unsigned i = 0;i < index[1];i++)
    outfile << _totalList[i][0] << '\n';
  for(unsigned i = index[1]; i < index[1]+index[3];i++){
    outfile << _totalList[i][0] << '\n';
  }

  //outfile << _dfsList.size() << endl;
  for(unsigned i = 0 ; i < _dfsList.size();i++ ){
    outfile << (_dfsList[i]*2) ;
    unsigned num;
    CirGate* g;
    for(unsigned j = 0; j < 2;j++){
      g = id2gate[_dfsList[i]] -> getfanin(j);
      num = g -> getgateID();
      num *= 2;
      if(id2gate[_dfsList[i]]-> isInv(j)) num++;
      outfile << ' ' << num ;
    }
    outfile << '\n';
  }
  string str;
  for(unsigned i = 0;i < index[1];i++){
    str = id2gate[_totalList[i][0]/2] -> getSymbol();
    if(str.size()!=0) outfile << 'i' << i << ' ' << str << '\n';
  }
  for(unsigned i = 0; i < index[3];i++){
    str = id2gate[index[0]+i+1] -> getSymbol();
    if(str.size()!=0) outfile << 'o' << i << ' ' << str << '\n';
  }
  outfile << "c\n";
  outfile << "AAG output by queline\n";
  //cleardfs();
  //if(outfile != cout) fb.close();
}

void CirMgr::dfs()const{
  for(unsigned i = 0; i< index[3];i++ )
    id2gate[index[0]+i+1] -> dfstraversal();
}

unsigned
CirMgr::myStr2Unsigned(const string& str)
{
  unsigned num = 0;
	for (size_t i = 0; i < str.size(); ++i) {
		if (isdigit(str[i])) {
			num *= 10;
			num += int(str[i] - '0');
		}
		else return num;
	}
	return num;
}
void
CirMgr::getTokens(const string& str, vector<string>& tok){
  size_t pos = 0;
  string tmp;
  while(1){
    pos = myStrGetTok(str,tmp,pos);
    if(tmp.size()==0) break;
    tok.push_back(tmp);
  }
}
