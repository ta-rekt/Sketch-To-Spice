#include "circuit.h"

Circuit::Circuit()
{}

void Circuit::addComponent(Component & comp)
{
  components.push_back(comp);
}

void Circuit::printNetlist()
{
  ofstream file;
  file.open("netlist.txt");

  file<<"*** NETLIST DESCRIPTION ***"<<endl;
  for (vector<Component>::iterator i = components.begin(); i != components.end(); i++)
    file<<i->name<<" "<<i->leftNode<<" "<<i->rightNode<<" "<<i->value<<endl;

  file<<"*** SIMULATION COMMANDS ***"<<endl;
  file<<".op"<<endl;

  file<<".end"<<endl;

  file.close();
}

// void addSimulationOption(string opt)
// {
//   simulationOptions.push_back(opt);
// }
