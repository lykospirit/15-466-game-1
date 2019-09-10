#include "Dialog.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cassert>

Dialog::Dialog(std::string const &filebase) {
  std::string dialog_path = filebase + ".cpl";
  std::ifstream in(dialog_path, std::ifstream::in);

  std::string s;
  while (in.good()) {
    std::getline(in, s);
    if (s.find(':') != std::string::npos) {
      std::string event = s.substr(0, s.find(':'));

      if (s.find('{') == std::string::npos || s.find('}') == std::string::npos ||
          s.find('<') == std::string::npos || s.find('>') == std::string::npos ||
          s.find('!') == std::string::npos ||
          s.find('(') == std::string::npos || s.find(')') == std::string::npos ||
          s.find('[') == std::string::npos || s.find(']') == std::string::npos ||
          s.find('"') == std::string::npos) {
        std::cout << s << std::endl;
        throw std::runtime_error("Malformed line in dialog file.");
      }

      Instruction i;
      auto extract_vars = [&s](std::vector<std::string> &list, char const left_delim, char const right_delim){
        size_t left_pos = s.find(left_delim);
        size_t right_pos = s.find(right_delim, left_pos+1);
        if (right_pos - left_pos > 1) {
          std::string extract = s.substr(left_pos+1, right_pos-left_pos-1);
          while (extract.find(',') != std::string::npos) {
            list.push_back(extract.substr(0, extract.find(',')));
            extract = extract.substr(extract.find(',')+1);
          }
          list.push_back(extract);
        }
      };
      extract_vars(i.pred, '{', '}');
      extract_vars(i.toTrue, '<', '>');
      extract_vars(i.toFalse, '!', '!');
      extract_vars(i.spriteOn, '(', ')');
      extract_vars(i.spriteOff, '[', ']');
      size_t left_pos = s.find('"');
      size_t right_pos = s.find('"', left_pos+1);
      i.text = s.substr(left_pos+1, right_pos-left_pos-1);

      if (rules.find(event) != rules.end()) { //Event already in rules: extend vector of Instructions
        rules[event].push_back(i);
      } else { //Event not yet in rules: create new vector and insert to map
        std::vector<Instruction> is = {i};
        rules[event] = is;
      }
    }
  }

  in.close();
}

Dialog::~Dialog(){}

std::vector<Instruction> const &Dialog::lookup(std::string const &event) const {
  auto f = rules.find(event);
  if (f == rules.end()) {
    throw std::runtime_error("Rule regarding event '" + event + "' not found.");
  }
  return f->second;
}
