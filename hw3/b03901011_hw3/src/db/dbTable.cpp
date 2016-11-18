/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'

  for(unsigned int i=0;i < r.size()-1; i++){
    if(r[i] == INT_MAX) os << "." << " ";
    else os << r[i] << " " ;
  }
  if(r[r.size()-1] == INT_MAX) os << ".";
  else os << r[r.size()-1];
  os << endl;
  return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells should be left blank (printed as ' ').
   for(size_t i=0;i < t.nRows(); i++)
  {
    for(size_t j=0;j < t.nCols(); j++)
    {
      if(t[i][j]== INT_MAX) cout << setw(6) << right<<".";
      else os << setw(6) << right << t[i][j];
    }
    os<<endl;
  }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.

  string line;
  string tmp_str;
  DBRow r;
  while(getline(ifs,line,'\r'))
  {
    if(line.length()==0) break;
    /*
    if(line.length()==0){
      r.addData(INT_MAX);
      r.reset();
      //if(t.nCols()==1)continue;
      // assume that only 1 ^M at the end
    }
    */
    size_t pos=0;
    size_t found=line.find(',',pos);
    while(found!=string::npos)
    {

      if(found-pos==0) r.addData(INT_MAX);
      else
      {
        tmp_str=line.substr(pos,found-pos);
        r.addData(atoi(tmp_str.c_str()));
      }
      pos = found+1;
      found = line.find(',',pos);
    }
    if(pos==line.size()) r.addData(INT_MAX);
    else
    {
      tmp_str=line.substr(pos);
      r.addData(atoi(tmp_str.c_str()));
    }
    t.addRow(r);
    r.reset();
  }
  ifs.clear();
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
  _data.erase(_data.begin()+c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for(size_t i=0;i < _sortOrder.size();i++){
     if(r1[_sortOrder[i]]== r2[_sortOrder[i]]) continue;
     else return (r1[_sortOrder[i]] < r2[_sortOrder[i]]);
   }
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
  _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   for(size_t i=0;i< nRows();i++)
      _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
  float max= INT_MIN;
  bool allnull = true;
  for(size_t i=0;i< nRows();i++)
  {
    if(_table[i][c]!=INT_MAX){
      if(_table[i][c]>max) max=_table[i][c];
      allnull = false;
    }
  }
  if(allnull) return NAN;
  return max;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   float min= INT_MAX-1;
   bool allnull = true;
   for(size_t i=0;i< nRows();i++)
   {
     if(_table[i][c]!=INT_MAX){
        if(_table[i][c] < min) min =_table[i][c];
        allnull = false;
     }
   }
   if(allnull) return NAN;
   return min;
}

float
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
  float sum=0;
  bool allnull = true;
  for(size_t i=0;i< nRows();i++)
  {
    if(_table[i][c]!= INT_MAX){
        sum+=_table[i][c];
        allnull = false;
    }
  }
  if(allnull) return NAN;
  return sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   vector<int> counter;
   counter.push_back(_table[0][c]);
   for(size_t i=0;i< nRows();i++)
   {
     for(size_t j=0;j<counter.size();j++)
     {
       if(counter[j]==_table[i][c]) break;
       else if(j==counter.size()-1 && _table[i][c]!=INT_MAX) counter.push_back(_table[i][c]);
     }
   }
   if(_table[0][c] == INT_MAX) counter.erase(counter.begin());
   int counter_size = counter.size();
   counter.clear();
   return counter_size;
 }

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
  float counter= nRows();
  for(size_t i=0;i< nRows();i++)
  {
    if(_table[i][c]== INT_MAX) counter--;
  }
  if(counter == 0) return NAN;
  return getSum(c)/ counter;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   std::sort(_table.begin(), _table.end(), s);

}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(unsigned int i=0;i < nRows()-1; i++){
     if(_table[i][c] == INT_MAX) cout << "." << " ";
     else cout << _table[i][c] << " " ;
   }
   if(_table[nRows()-1][c] == INT_MAX) cout << ".";
   else cout << _table[nRows()-1][c];
   cout << endl;

}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}
