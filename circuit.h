// The circuit class enables the user to add components and declare their connections and values. The printNetlist function
// writes a netlist of the circuit in a text file. Every component has a name, value, left connection and right connection
// (pointers to other components). You can reuse code from the refactoring assignment.


#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#ifndef CIRCUIT_H
#define CIRCUIT_H

struct Component
{
  Component(){};
  virtual void initializeName(){}

  string name;    // make this netlist format i.e. R1, V1. starting with the right letter
  int value, leftNode, rightNode;

  void print()
  {
    cout<<"COMPONENT"<<"\n";
    cout<<"Name:\t"<<name<<"\n";
    cout<<"Value:\t"<<value<<"\n";
    cout<<"Connections:\t"<<leftNode<<" "<<rightNode<<"\n";
  }
};

// first letter of type has to be capitalized for netlist to recognize it
#define COMP_DECLARE(type)\
struct type: Component{\
  void initializeName(int i){\
    name = #type[0] + to_string(i);\
  }\
};\


class Circuit
{
public:
  Circuit();
  void addComponent(Component & comp);
  //void addSimulationOption(string opt);
  void printNetlist();
private:
  vector<Component> components;
  // vector<string> simulationOptions;
  // string simulationParameters;    // DC analysis example: Vin 0 5 0.1 Vdd 0 5 0.5
};




#endif /* end of include guard: CIRCUIT_H*/
