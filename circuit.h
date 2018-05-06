// The circuit class enables the user to add components and declare their connections and values. The printNetlist function
// writes a netlist of the circuit in a text file. Every component inherits from the Component class. Each has a name, value
// left connection and right connection (pointers to other components). You can reuse code from the refactoring assignment.

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <iostream>
using namespace std;

class Component;


class Circuit
{
public:
  Circuit();
  void addComponent(Component & comp, int lnode, int rnode);
  void printNetlist();
private:
  vector<vector<int>> adjacencyMatrix;
  vector<Component> components;
};


struct Component
{
  string name;
  int value, leftNode, rightNode;
  void print()
  {
    cout<<"COMPONENT"<<"\n"
    cout<<"Name:\t"<<name<<"\n";
    cout<<"Value:\t"<<value<<"\n";
    cout<<"Connections:\t"<<leftNode<<" "<<rightNode<<"\n";
  }
};

#endif /* end of include guard: CIRCUIT_H*/
