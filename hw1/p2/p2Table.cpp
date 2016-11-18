#include "p2Table.h"

using namespace std;

const Row& Table::operator[] (size_t i) const {return _rows[i];}
Row& Table::operator[] (size_t i){return _rows[i];}

bool Table::read(const string& csvFile)
{
  ifstream fs;
  fs.open(csvFile.c_str());
  if(!fs.is_open()) return false;

  vector<int> buffer;
  string line;
  int num_row=0;
  int num_col=1;
  int tmp_col=1;
  string tmp_str;

  while(getline(fs,line,'\r'))
  {
    if(line.length()==0) break;
    tmp_col=1;
    size_t pos=0;
    size_t found=line.find(',',pos);
    while(found!=string::npos)
    {
      if(found-pos==0) buffer.push_back(1024);
      else
      {
        tmp_str=line.substr(pos,found-pos);
        buffer.push_back(atoi(tmp_str.c_str()));
      }
      pos=found+1;
      found=line.find(',',pos);
      tmp_col++;
    }
    if(pos==line.size()) buffer.push_back(1024);
    else
    {
      tmp_str=line.substr(pos);
      buffer.push_back(atoi(tmp_str.c_str()));
    }
    if(tmp_col>num_col) num_col=tmp_col;
    num_row++;
  }

  _num_col=num_col;
  _num_row=num_row;

  int count_element=0;

  for(int i=0;i<num_row;i++)
  {
    Row buffer_row(num_col);
    for(int j=0;j<num_col;j++)
    {
      buffer_row[j]=buffer[count_element];
      count_element++;
    }
    _rows.push_back(buffer_row);
  }

  fs.close();
  return true;
}

void Table::print()
{
  for(size_t i=0;i<_num_row;i++)
  {
    for(size_t j=0;j<_num_col;j++)
    {
      if(_rows[i][j]==1024) cout<<setw(4)<<right<<" ";
      else cout<<setw(4)<<right<<_rows[i][j];
    }
    cout<<endl;
  }
}

void Table::sum()
{
  int col;
  int sum=0;
  cin >> col;
  for(size_t i=0;i<_num_row;i++)
  {
    if(_rows[i][col]!=1024)
      sum+=_rows[i][col];
  }
  cout << "The summation of data in column #" << col << " is "<< sum << endl;
}

void Table::max()
{
  int col;
  cin >> col;
  int max=-100;
  for(size_t i=0;i<_num_row;i++)
  {
    if(_rows[i][col]!=1024 && _rows[i][col]>max)
      max=_rows[i][col];
  }
  cout << "The maximum of data in column #" << col << " is "<< max << endl;
}

void Table::min()
{
  int col;
  cin >> col;
  int min=101;
  for(size_t i=0;i<_num_row;i++)
  {
    if( _rows[i][col]<min)
      min=_rows[i][col];
  }
  cout << "The minimum of data in column #" << col << " is "<< min << endl;
}

void Table::count()
{
  int col;
  cin >> col;
  vector<int> counter;
  counter.push_back(_rows[0][col]);
  for(size_t i=0;i<_num_row;i++)
  {
    for(size_t j=0;j<counter.size();j++)
    {
      if(counter[j]==_rows[i][col]) break;
      else if(j==counter.size()-1 && _rows[i][col]!=1024) counter.push_back(_rows[i][col]);
    }
  }
  cout << "The distinct count of data in column #" << col << " is "<< counter.size() << endl;
  counter.clear();
}

void Table::ave()
{
  int col;
  cin >> col;

  int sum=0;
  int counter=0;
  for(size_t i=0;i<_num_row;i++)
  {
    if(_rows[i][col]!=1024)
    {
      sum+=_rows[i][col];
      counter++;
    }
  }
  double ave = round(sum*10/counter)/10;
  cout <<  "The average of data in column #" << col << " is "<< fixed << setprecision(1) <<ave << endl;
}

void Table::add()
{
  string tmp_str;
  vector<string> add_vec;
  size_t counter=0;
  while(counter!=_num_col)
  {
    cin >> tmp_str;
    counter++;
    if(tmp_str=="-") add_vec.push_back("1024");
    else add_vec.push_back(tmp_str);
    tmp_str.clear();
  }
  Row buffer_row(_num_col);
  for(size_t j=0;j<_num_col;j++)
  {
    buffer_row[j]=atoi(add_vec[j].c_str());
  }
  _rows.push_back(buffer_row);
  _num_row++;
  add_vec.clear();
}

void Table::exit_table()
{
  for(size_t i=0;i<_num_row;i++) _rows[i].del();
  _rows.clear();
}
