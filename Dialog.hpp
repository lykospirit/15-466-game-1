#pragma once

#include <map>
#include <string>
#include <vector>

struct Instruction {
  std::vector<std::string> pred, toTrue, toFalse, spriteOn, spriteOff;
  std::string text;
};

struct Dialog {
  Dialog(std::string const &filebase);
  ~Dialog();

  std::vector<Instruction> const &lookup(std::string const &name) const;

  // internal data
  std::map< std::string, std::vector<Instruction> > rules;
};
