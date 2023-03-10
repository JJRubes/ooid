#include<string>
#include<vector>

#include"BoxFinder.hpp"
#include"CallBox.hpp"
#include"DefinitionBox.hpp"
#include"FindConnections.hpp"
#include"Pin.hpp"
#include"UnparsedBox.hpp"

UnparsedBox::UnparsedBox(int x, int y, std::vector<std::string> lines) {
  tlx = x;
  tly = y;

  contents.swap(lines);

  // this assumes every line is consistent in length
  width = contents[0].size();
  height = contents.size();

  definition = false;

  parseEdges();
}

bool UnparsedBox::isDef() {
  return definition;
}

void UnparsedBox::parseEdges() {
  parseName();

  // parse the pins
  // top
  for(int i = 0; i < width; i++) {
    if(isPin(at(i, 0))) {
      if(isDef())
        pins.push_back(Pin(at(i, 0), i, 0, Pin::DIRECTION::SOUTH));
      else
        pins.push_back(Pin(at(i, 0), i, 0, Pin::DIRECTION::NORTH));
    }
  }

  // left
  for(int i = 0; i < height; i++) {
    if(isPin(at(0, i))) {
      if(isDef())
        pins.push_back(Pin(at(0, i), 0, i, Pin::DIRECTION::EAST));
      else
        pins.push_back(Pin(at(0, i), 0, i, Pin::DIRECTION::WEST));
    }
  }

  // right
  for(int i = 0; i < height; i++) {
    if(isPin(at(width - 1, i))) {
      if(isDef())
        pins.push_back(Pin(at(width - 1, i), width - 1, i, Pin::DIRECTION::WEST));
      else
        pins.push_back(Pin(at(width - 1, i), width - 1, i, Pin::DIRECTION::EAST));
    }
  }

  // bottom
  for(int i = 0; i < width; i++) {
    if(isPin(at(i, height - 1))) {
      if(isDef())
        pins.push_back(Pin(at(i, height - 1), i, height - 1, Pin::DIRECTION::NORTH));
      else
        pins.push_back(Pin(at(i, height - 1), i, height - 1, Pin::DIRECTION::SOUTH));
    }
  }
}

DefinitionBox UnparsedBox::makeDefinition() {
  // checks to make sure that the Unparsed box is being used correctly
  if(!definition)
    std::cout << "error" << std::endl;


  // find all the boxes within this box
  BoxFinder bf;
  for(std::size_t i = 1; i < contents.size() - 1; i++) {
    std::size_t len = contents[i].size();
    // we want to cut off the edges of the box
    // so that they are not included in any of the
    // internal boxes
    // the 2 comes from taking one character off each side
    bf.process(contents[i].substr(1, len - 2));
  }
  std::vector<UnparsedBox> boxes;
  bf.move(boxes);

  // parse these boxes and add them to the appropriate list
  std::vector<DefinitionBox> definitions;
  std::vector<CallBox> calls;
  for(UnparsedBox b : boxes) {
    if(b.isDef()) {
      definitions.push_back(b.makeDefinition());
    } else {
      calls.push_back(b.makeCall());
    }
  }

  // find which pins are connected to which
  std::vector<Connection> connections;
  FindConnections fc = FindConnections(contents, pins, calls, definitions, tlx, tly);
  fc.process();
  fc.move(connections);

  return DefinitionBox(name, pins, definitions, calls, connections, tlx, tly);
}

CallBox UnparsedBox::makeCall() {
  if(definition)
    std::cout << "error" << std::endl;

  name = "";
  // cuts whitespace from the start of each line in the box
  // and ignores blank lines
  for(std::size_t i = 1; i < contents.size() - 1; i++) {
    std::string l = "";
    bool maybeEnd = false;
    int spaceCount;

    // remove starting spaces
    int startPos = 1;
    while(contents.at(i).at(startPos) == ' ') {
      startPos++;
    }

    // remove ending spaces while not removing
    // spaces in between
    for(size_t j = startPos; j < contents.at(i).size() - 1; j++) {
      char c = contents.at(i).at(j);
      if(c == ' ') {
        if(maybeEnd) {
          spaceCount++;
        } else {
          maybeEnd = true;
          spaceCount = 1;
        }
      } else if(maybeEnd) {
        maybeEnd = false;
        std::string s(spaceCount, ' ');
        l += s;
        l += c;
      } else {
        l += c;
      }
    }

    // add the line to the name
    if(l != "") {
      if(name != "") {
        name += " ";
      }
      name += l;
    }
  }

  return CallBox(name, pins, tlx, tly);
}

void UnparsedBox::print() {
  if(isDef()) {
    std::cout << "Definition ";
  } else {
    std::cout << "Call ";
  }
  std::cout << name << ": " << std::endl;
  for(Pin p : pins) {
    p.print(0, 2);
  }
  std::cout << "At (" << tlx << ", " << tly << ")";
}

const bool UnparsedBox::isPin(char p) {
  return p != '+' &&
         p != '-' &&
         p != '|' &&
         p != ' ';
}

char UnparsedBox::at(int x, int y) {
  return contents.at(y)[x];
}

void UnparsedBox::parseName() {
  // check the top line
  int start = scanHorizontal(0, true);
  if(start != -1) {
    int end = scanHorizontal(0, false);
    if(end < start) {
      std::cout << "Error: found spaces on top edge that didn't contain a name." << std::endl;
      return;
    }
    getName(start, end, 0, true);
    return;
  }

  // left edge
  start = scanVertical(0, true);
  if(start != -1) {
    int end = scanVertical(0, false);
    if(end < start) {
      std::cout << "Error: found spaces on left edge that didn't contain a name." << std::endl;
      return;
    }
    getName(start, end, 0, false);
    return;
  }

  // right edge
  start = scanVertical(width - 1, true);
  if(start != -1) {
    int end = scanVertical(width - 1, false);
    if(end < start) {
      std::cout << "Error: found spaces on right edge that didn't contain a name." << std::endl;
      return;
    }
    getName(start, end, width - 1, false);
    return;
  }

  // bottom edge
  start = scanHorizontal(height - 1, true);
  if(start != -1) {
    int end = scanHorizontal(height - 1, false);
    if(end < start) {
      std::cout << "Error: found spaces on bottom edge that didn't contain a name." << std::endl;
      return;
    }
    getName(start, end, height - 1, true);
    return;
  }
}

int UnparsedBox::scanHorizontal(int line, bool forwards) {
  bool spaces = false;
  int last = -1;
  int start = forwards ? 0 : width - 1;
  int end = forwards ? width : -1;
  int inc = forwards ? 1 : -1;
  for(int i = start; inc * i < end; i += inc) {
    if(spaces) {
      if(at(i, line) != ' ') {
        last = i;
        break;
      }
    } else {
      if(at(i, line) == ' ') {
        spaces = true;
      }
    }
  }
  return last;
}

int UnparsedBox::scanVertical(int line, bool forwards) {
  bool spaces = false;
  int last = -1;
  int start = forwards ? 0 : height - 1;
  int end = forwards ? height : -1;
  int inc = forwards ? 1 : -1;
  for(int i = start; i < end; i += inc) {
    if(spaces) {
      if(at(line, i) != ' ') {
        last = i;
        break;
      }
    } else {
      if(at(line, i) == ' ') {
        spaces = true;
      }
    }
  }
  return last;
}

void UnparsedBox::getName(int start, int end, int line, bool horizontal) {
  if(horizontal) {
    name = std::string(contents.at(line), start, end - start + 1);
    // then blank it
    for(int i = start; i <= end; i++) {
      contents[line][i] = '-';
    }
    for(size_t i = 0; i < contents.at(line).size(); i++) {
      if(contents[line][i] == ' ') {
        contents[line][i] = '-';
      }
    }
  } else {
    name = std::string(start - end + 1, '.');
    for(int i = start; i <= end; i++) {
      name[i] = at(line, i);
    }
    // blank it
    for(int i = start; i <= end; i++) {
      contents[i][line] = '|';
    }
    for(size_t i = 0; i < contents.size(); i++) {
      if(contents[i][line] == ' ') {
        contents[i][line] = '|';
      }
    }
  }

  definition = true;
}
