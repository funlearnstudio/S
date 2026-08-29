#include "s/bindgen.hpp"
#include "s/checker.hpp"
#include "s/compiler.hpp"
#include "s/error.hpp"
#include "s/interpreter.hpp"
#include "s/lexer.hpp"
#include "s/modules.hpp"
#include "s/parser.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {
std::string read_file(const std::filesystem::path&p){std::ifstream f(p);if(!f)throw std::runtime_error("Could not open "+p.string());return {std::istreambuf_iterator<char>(f),{}};}
s::ast::Program frontend_file(const std::filesystem::path&path){s::ModuleLoader loader;auto program=loader.load(path);s::Checker{}.check(program);return program;}
std::string shell_quote(const std::string&s){std::string r="'";for(char c:s){if(c=='\'')r+="'\\''";else r+=c;}return r+"'";}
int file_command(const std::string&cmd,const std::filesystem::path&path){
  auto source=read_file(path);
  try{auto p=frontend_file(path);if(cmd=="check"){std::cout<<path.string()<<" is valid SE.\n";return 0;}if(cmd=="run"){s::Interpreter vm(std::cin,std::cout);vm.run(p);return 0;}
    auto cpp=s::CppCompiler{}.generate(p);auto cpp_path=path;cpp_path.replace_extension(".se.cpp");auto output=path;output.replace_extension();{std::ofstream f(cpp_path);f<<cpp;}
    const char*cxx=std::getenv("CXX");std::string compiler=cxx?cxx:"c++";
#ifdef S_SOURCE_ROOT
    std::filesystem::path root=S_SOURCE_ROOT;
#else
    std::filesystem::path root=std::filesystem::current_path();
#endif
    std::string command=compiler+" -std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror -I"+shell_quote((root/"include").string())+" "+shell_quote(cpp_path.string())+" "+shell_quote((root/"src/runtime/error.cpp").string())+" "+shell_quote((root/"src/runtime/value.cpp").string())+" "+shell_quote((root/"src/interpreter/interpreter.cpp").string())+" "+shell_quote((root/"src/ffi/ffi.cpp").string())+" -pthread";
#ifdef __linux__
    command+=" -ldl";
#endif
    command+=" -o "+shell_quote(output.string());int code=std::system(command.c_str());std::filesystem::remove(cpp_path);if(code!=0)throw std::runtime_error("The C++ compiler could not build the generated program.");std::cout<<"Built "<<output.string()<<"\n";return 0;
  }catch(const s::Error&e){std::cerr<<s::format_error(e,source);return 1;}catch(const s::RuntimeFailure&e){auto&x=e.error();std::cerr<<x.kind;if(!x.source.empty())std::cerr<<" in "<<x.source;if(x.line)std::cerr<<" on line "<<x.line;std::cerr<<": "<<x.message<<'\n';return 1;}
}
int bind_command(const std::filesystem::path&definition,const std::filesystem::path&output){
  try{
    auto files=s::generate_bindings(definition,output);
    std::cout<<"Generated "<<files.metadata.string()<<'\n';
    std::cout<<"Generated "<<files.header.string()<<'\n';
    std::cout<<"Generated "<<files.source.string()<<'\n';
    return 0;
  }catch(const s::Error&e){std::string source;try{source=read_file(definition);}catch(...){ }std::cerr<<s::format_error(e,source);return 1;}
}
void repl(){
  std::cout<<"SE 0.3.0\nType SE code. Use a blank line to finish a block. Ctrl-D exits.\n";
  std::string pending,line; s::Checker checker; s::Interpreter vm(std::cin,std::cout);
  auto execute=[&]{if(pending.empty())return;try{s::Lexer lexer(pending);s::Parser parser(lexer.scan());auto program=parser.parse();checker.check(program);vm.run(program);}catch(const s::Error&e){std::cerr<<s::format_error(e,pending);}catch(const s::RuntimeFailure&e){std::cerr<<e.what()<<'\n';}pending.clear();};
  while(true){std::cout<<(pending.empty()?"> ":". ");if(!std::getline(std::cin,line)){execute();break;}if(line.empty()&&!pending.empty()){execute();continue;}pending+=line+'\n';if(line.find_first_not_of(' ')==0&&line.rfind("if ",0)!=0&&line.rfind("for ",0)!=0&&line.rfind("while ",0)!=0&&line.rfind("repeat ",0)!=0&&line.rfind("make ",0)!=0&&line.rfind("type ",0)!=0&&line!="try")execute();}
}
}
int main(int argc,char**argv){
  try{
    if(argc==1){repl();return 0;}
    if(argc==2&&std::string(argv[1])=="--version"){std::cout<<"SE 0.3.0\n";return 0;}
    if(argc>=3&&std::string(argv[1])=="bind"){
      if(argc>4){std::cerr<<"Use: se bind module.sbind [output-directory]\n";return 2;}
      std::filesystem::path definition=argv[2];
      std::filesystem::path output=argc==4?std::filesystem::path(argv[3]):definition.parent_path();
      if(output.empty())output=".";
      return bind_command(definition,output);
    }
    if(argc!=3){std::cerr<<"Use: se run file.se | se check file.se | se build file.se | se bind module.sbind [output-directory]\n";return 2;}
    std::string cmd=argv[1];if(cmd!="run"&&cmd!="check"&&cmd!="build"){std::cerr<<"Unknown command '"<<cmd<<"'. Use run, check, build, or bind.\n";return 2;}return file_command(cmd,argv[2]);
  }catch(const std::exception&e){std::cerr<<"Error: "<<e.what()<<'\n';return 1;}
}
