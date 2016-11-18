/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstdlib>
#include "dbTable.h"

using namespace std;

extern DBTable dbtbl;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <cvsfile>
      cerr << "Error: using testdb <cvsfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbtbl) {
      cout << "Table is resetting..." << endl;
      dbtbl.reset();
   }
   if (!(inf >> dbtbl)) {
      cerr << "Error in reading csv file!!" << endl;
      exit(-1);
   }

   cout << "========================" << endl;
   cout << " Print table " << endl;
   cout << "========================" << endl;
   int IND = 0;
   cout << dbtbl << endl;
   dbtbl.printCol(IND);
   cout << endl;
   dbtbl.printSummary();
   cout << endl;
   cout <<"max "<< dbtbl.getMax(IND) << endl;
   cout <<"min " << dbtbl.getMin(IND) << endl;
   cout <<"sum " << dbtbl.getSum(IND) << endl;
   cout <<"count " << dbtbl.getCount(IND) << endl;
   cout <<"ave " << dbtbl.getAve(IND) <<endl;
   vector <int> t;
   for(int i=0;i<dbtbl.nCols();i++) t.push_back(i);
   dbtbl.addRow(t);
   cout << "addrow" <<'\n'<< dbtbl << endl;
   //cout << "ncol " << dbtbl.nCols() <<endl;
   t.clear();
   for(int i=0;i<dbtbl.nRows();i++) t.push_back(i);
   dbtbl.addCol(t);
   cout << "addcol" <<'\n'<< dbtbl << endl;
   t.clear();
   dbtbl.delCol(IND);
   cout << "delcol" <<'\n'<< dbtbl << endl;
   dbtbl.delRow(IND);
   cout << "delrow" <<'\n'<< dbtbl << endl;


   // TODO
   // Insert what you want to test here by calling DBTable's member functions

   return 0;
}
