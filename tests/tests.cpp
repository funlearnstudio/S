#include "s/checker.hpp"
#include "s/error.hpp"
#include "s/interpreter.hpp"
#include "s/lexer.hpp"
#include "s/parser.hpp"
#include <iostream>
#include <sstream>

static int failures=0;
#define EXPECT(c) do{if(!(c)){std::cerr<<"FAIL "<<__LINE__<<": "#c"\n";++failures;}}while(0)
s::ast::Program parse(const std::string&x){s::Lexer l(x);s::Parser p(l.scan());return p.parse();}
std::string run(const std::string&x){auto p=parse(x);s::Checker{}.check(p);std::istringstream in;std::ostringstream out;s::Interpreter vm(in,out);vm.run(p);return out.str();}
template<class F>bool throws(F f){try{f();return false;}catch(const s::Error&){return true;}}
int main(){
  {s::Lexer l("if true\n    say 1\nsay 2\n");auto t=l.scan();int i=0,d=0;for(auto&x:t){i+=x.kind==s::TokenKind::Indent;d+=x.kind==s::TokenKind::Dedent;}EXPECT(i==1&&d==1);}
  EXPECT(run("say 2 + 3 * 4\n")=="14\n");
  EXPECT(run("if 3 > 2\n    repeat 2\n        say \"yes\"\nelse\n    say \"no\"\n")=="yes\nyes\n");
  EXPECT(run("total = 0\nfor x in 1..4\n    total = total + x\nsay total\n")=="10\n");
  EXPECT(run("make add a b\n    give a + b\nsay add 4 5\n")=="9\n");
  EXPECT(run("x = [1, 2]\nx[0] = 9\nsay x[0]\nsay x.len\n")=="9\n2\n");
  EXPECT(throws([]{auto p=parse("say missing\n");s::Checker{}.check(p);}));
  EXPECT(throws([]{auto p=parse("x = \"5\" + 10\n");s::Checker{}.check(p);}));
  EXPECT(throws([]{run("x = [1]\nsay x[4]\n");}));
  EXPECT(throws([]{parse("if true\n   say 1\n");}));
  if(failures){std::cerr<<failures<<" test(s) failed\n";return 1;}std::cout<<"All S tests passed.\n";
}
