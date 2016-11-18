#include <iostream>
#include <string>
#include <cstdlib>
#include "p2Table.h"

using namespace std;

int main()
{
   Table table;

   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;

   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else exit(-1);

   while (true)
   {
     string fnc_name;
     cout<<"enter the commands"<<endl;
     cin >> fnc_name;
     if(fnc_name == "PRINT") table.print();
     else if(fnc_name=="SUM") table.sum();
     else if(fnc_name=="MAX") table.max();
     else if(fnc_name=="MIN") table.min();
     else if(fnc_name=="COUNT") table.count();
     else if(fnc_name=="AVE") table.ave();
     else if(fnc_name=="ADD") table.add();
     else if(fnc_name=="EXIT") {table.exit_table(); break;};
    // else cout << "wrong commands, please enter a new one" << endl;
   }
}
