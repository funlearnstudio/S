#include "s/error.hpp"
#include <sstream>
#include <vector>

namespace s {
std::string format_error(const Error& e, const std::string& source) {
  std::istringstream in(source); std::string line; int n=1;
  while (n<e.pos().line && std::getline(in,line)) ++n;
  std::getline(in,line);
  std::ostringstream out;
  out << "Error on line " << e.pos().line << "\n\n" << e.what() << "\n\n";
  if (!line.empty()) out << e.pos().line << " | " << line << "\n  | " << std::string(std::max(0,e.pos().column-1),' ') << "^\n";
  if (!e.hint().empty()) out << "\nTry: " << e.hint() << "\n";
  return out.str();
}
}
