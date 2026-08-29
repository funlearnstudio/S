#pragma once

#include "s/value.hpp"
#include <istream>
#include <ostream>

namespace s {
class Interpreter {
public:
  Interpreter(std::istream& in,std::ostream& out);
  void run(const ast::Program&); Value evaluate(const ast::ExprPtr&);
  std::shared_ptr<Environment> environment() const{return env_;}
private:
  std::istream& in_; std::ostream& out_; std::shared_ptr<Environment> env_; int loop_steps_=0;
  void execute(const ast::StmtPtr&); void execute_block(const ast::Block&,std::shared_ptr<Environment>);
};
} // namespace s
