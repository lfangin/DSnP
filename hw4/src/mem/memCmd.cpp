/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   int numO;
   int sizeArr;
   bool doArr = false;
   if(!CmdExec::lexOptions(option,options)) return CMD_EXEC_ERROR;
   size_t op_size = options.size();

   if(options.empty()||op_size==2)
      return CmdExec::errorOption(CMD_OPT_MISSING,"");
   else if(op_size > 3) return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
   else if(op_size ==1){
     if(!(myStr2Int(options[0],numO))||numO < 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
   }
   else{
     doArr = true;
     if(!myStr2Int(options[0],numO)){
       if(myStrNCmp("-Array",options[0],2)!=0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
       if(!myStr2Int(options[1],sizeArr)||sizeArr < 1)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
       if(!myStr2Int(options[2],numO)|| numO < 1)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
     }
     else {
       if(numO < 1) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
       if(myStrNCmp("-Array",options[1],2)!=0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
       if(!myStr2Int(options[2],sizeArr)||sizeArr < 1)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
     }
   }

   if(doArr){
     try{ mtest.newArrs(numO,sizeArr);}
     catch(std::bad_alloc){ return CMD_EXEC_ERROR;}
   }
   else{
     try{ mtest.newObjs(numO);}
     catch(std::bad_alloc){ return CMD_EXEC_ERROR;}
   }

   return CMD_EXEC_DONE;

}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
/*
  vector<string> options;
  if (!CmdExec::lexOptions(option, options))
    return CMD_EXEC_ERROR;

  int objId = -1 , numRandId = 0, randIdx = -1;
  bool is_read = false, random = false, arr = false;

  for( size_t i = 0; i < options.size(); i++ ) {
    if (myStrNCmp("-Array", options[i], 2) == 0) {
      if(arr) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      else arr = true;
    }
    else if(myStrNCmp("-Index", options[i], 2) == 0) {
      if(is_read) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      else if(options[++i].empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
      else if(!myStr2Int(options[i], objId) || objId < 0)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      else is_read = true;
    }
    else if(myStrNCmp("-Random", options[i], 2) == 0) {
      if(is_read) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      else if(options[++i].empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
      else if(!myStr2Int(options[i], numRandId) || numRandId < 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      else { is_read = true; random = true; randIdx = i - 1;}
    }
    else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
  }

  if(!options.size() || !is_read)
    return CmdExec::errorOption(CMD_OPT_MISSING, "");

  if(!random) {
    if(arr) {
      if(objId >= mtest.getArrListSize()) {
        cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << objId << "!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(objId));
      }
      else mtest.deleteArr(objId);
    }
    else {
      if(objId >= mtest.getObjListSize()) {
        cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objId << "!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, to_string(objId));
      }
      else mtest.deleteObj(objId);
    }
  }
  else {
    if(arr) {
      if(!mtest.getArrListSize()) {
        cerr << "Size of array list is 0!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[randIdx]);
      }
      for (int i = 0; i < numRandId; i++){
        size_t n = rnGen( mtest.getArrListSize() );
        cout << n<<" ";
        mtest.deleteArr( n );
      }
    }
    else {
      if(!mtest.getObjListSize()) {
        cerr << "Size of object list is 0!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[randIdx]);
      }
      for (int i = 0; i < numRandId; i++){
        size_t n =  rnGen( mtest.getObjListSize() );
        cout << n <<" ";
        mtest.deleteObj( n);
      }
    }
  }
  return CMD_EXEC_DONE;
 */
///*
  vector<string> options;
  int id_num;
  bool doRandom = false;
  bool doArr = false;
  if(!CmdExec::lexOptions(option,options)) return CMD_EXEC_ERROR;
  if(options.size() > 3) return CmdExec::errorOption(CMD_OPT_EXTRA,options[3]);
  if(options.size() < 2) return CmdExec::errorOption(CMD_OPT_MISSING,"");
  if(options.size() == 2){
    if(!(myStr2Int(options[1],id_num))||id_num < 0)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
    if(myStrNCmp("-Random",options[0],2) == 0) doRandom = true;
    else if (myStrNCmp("-Index",options[0],2) != 0)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
  }
  else {
    doArr = true;
    //cout << "do arr flag " <<endl;
    if(myStrNCmp("-Array",options[0],2)==0){
      if(!(myStr2Int(options[2],id_num))||id_num < 0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      if(myStrNCmp("-Random",options[1],2) == 0) doRandom = true;
      else if (myStrNCmp("-Index",options[1],2) != 0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
    }
    else {
      if(myStrNCmp("-Array",options[2],2) !=0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[2]);
      if(!(myStr2Int(options[1],id_num))||id_num < 0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      if(myStrNCmp("-Random",options[0],2) == 0) doRandom = true;
      else if (myStrNCmp("-Index",options[0],2) != 0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[0]);
    }
  }

  if(doRandom){
    if(id_num == 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
    if(doArr){
      if(int(mtest.getArrListSize()) == 0){
        cerr << "Size of array list is 0!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      }
      //cout << mtest.getArrListSize();
      for(size_t i = 0;i < id_num;i++){
        int n = rnGen(mtest.getArrListSize());
        //cout << n <<" ";
        mtest.deleteArr(n);
      }
    }
    else{
      if(int(mtest.getObjListSize()) ==0){
        cerr << "Size of object list is " << mtest.getObjListSize() << "!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      }
    //  cout << mtest.getObjListSize();
      for(size_t i = 0;i < id_num;i++){
        int n = rnGen(mtest.getObjListSize());
        //cout << n <<" ";
        mtest.deleteObj(n);
      }
    }
  }
  else{
    if(doArr){
      if(id_num > mtest.getArrListSize()){
        cerr << "Size of array list (" << mtest.getArrListSize()
              << ") is <= " << id_num <<"!!"<< endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      }
      mtest.deleteArr(id_num);
    }
    else{
      if(id_num > mtest.getObjListSize()){
        cerr << "Size of object list (" << mtest.getObjListSize()
              << ") is <= " << id_num <<"!!"<< endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[1]);
      }
      mtest.deleteObj(id_num);
    }
  }
   return CMD_EXEC_DONE;
//*/
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}
