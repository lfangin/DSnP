#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <math.h>

using namespace std;

class Row
{
public:
   const int operator[] (size_t i) const { return _data[i];}
   int& operator[] (size_t i) { return _data[i]; }
   Row(int num_col){ _data= new int[num_col];}
   void del(){ delete [] _data; _data=0; }
private:
   int  *_data;
};

class Table
{
public:
   const Row& operator[] (size_t i) const;
   Row& operator[] (size_t i);

   bool read(const string&);
   void print();
   void sum();
   void max();
   void min();
   void count();
   void ave();
   void add();
   void exit_table();
private:
   vector<Row>  _rows;
   size_t _num_row;
   size_t _num_col;
};

#endif // P2_TABLE_H
