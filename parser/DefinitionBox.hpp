#pragma once

#include<string>
#include<vector>

#include"CallBox.hpp"
#include"Connection.hpp"
#include"Pin.hpp"

// This should be the thing that contains everything,
// or maybe nothing??
// definitely needs some way of representing connections
class DefinitionBox {
  public:
    DefinitionBox(std::string n,
                  std::vector<Pin> p,
                  std::vector<DefinitionBox> defs,
                  std::vector<CallBox> cs,
                  std::vector<Connection> cns,
                  int X, int Y);
    void print(std::size_t indentation, std::size_t indentSize);

    const std::vector<Pin> getPins();
    Pin getPin(size_t p);
    const std::string getName();
    int getX();
    int getY();
  private:
    std::string name;
    std::vector<DefinitionBox> definitions;
    std::vector<CallBox> calls;
    std::vector<Connection> connections;
    std::vector<Pin> pins;
    int x, y;
};
