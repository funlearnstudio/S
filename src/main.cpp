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

void write_new_file(const std::filesystem::path&path,const std::string&content){
  if(std::filesystem::exists(path))throw std::runtime_error("Refusing to overwrite "+path.string());
  if(!path.parent_path().empty())std::filesystem::create_directories(path.parent_path());
  std::ofstream out(path,std::ios::binary);
  if(!out)throw std::runtime_error("Could not create "+path.string());
  out<<content;
}

void print_help(){
  std::cout<<
    "SE - simple at every level\n\n"
    "Commands:\n"
    "  se run file.se                 Run an SE program\n"
    "  se check file.se               Parse and type-check without running\n"
    "  se build file.se               Build a native executable\n"
    "  se bind module.sbind [dir]     Generate C ABI bindings\n"
    "  se new app NAME                Create a normal SE application\n"
    "  se new web NAME                Create an SE + HTML/CSS/JS/TS web project\n"
    "  se doctor                      Show local toolchain diagnostics\n"
    "  se help                        Show this help\n"
    "  se --version                   Show the SE version\n\n"
    "Source files use .se. Legacy .s files are still accepted during migration.\n";
}

int doctor_command(){
  std::cout<<"SE doctor\n";
  std::cout<<"  version: SE 0.4.0-dev\n";
#ifdef _WIN32
  std::cout<<"  platform: Windows\n";
#elif __APPLE__
  std::cout<<"  platform: macOS\n";
#elif __linux__
  std::cout<<"  platform: Linux\n";
#else
  std::cout<<"  platform: unknown\n";
#endif
  if(const char*cxx=std::getenv("CXX"))std::cout<<"  CXX: "<<cxx<<"\n";
  else std::cout<<"  CXX: not set (SE will use c++)\n";
  if(const char*home=std::getenv("SE_HOME"))std::cout<<"  SE_HOME: "<<home<<"\n";
  else if(const char*legacy=std::getenv("S_HOME"))std::cout<<"  S_HOME: "<<legacy<<" (legacy)\n";
  else std::cout<<"  package home: default search paths\n";
  std::cout<<"  current directory: "<<std::filesystem::current_path().string()<<"\n";
  std::cout<<"Doctor finished. Use 'se check file.se' for source diagnostics.\n";
  return 0;
}

int new_project(const std::string&kind,const std::filesystem::path&root){
  if(root.empty())throw std::runtime_error("Project name cannot be empty.");
  if(std::filesystem::exists(root))throw std::runtime_error("Project path already exists: "+root.string());
  std::filesystem::create_directories(root);
  if(kind=="app"){
    write_new_file(root/"src/main.se","say \"Hello from SE\"\n");
    write_new_file(root/"README.md","# "+root.filename().string()+"\n\nRun:\n\n```sh\nse run src/main.se\n```\n\nBuild:\n\n```sh\nse build src/main.se\n```\n");
    std::cout<<"Created SE app at "<<root.string()<<"\n";
    std::cout<<"Next: cd "<<root.string()<<" && se run src/main.se\n";
    return 0;
  }
  if(kind=="web"){
    write_new_file(root/"backend/main.se",
      "# SE backend entry point.\n"
      "# The HTTP runtime is being built on top of this project layout.\n"
      "say \"SE web backend ready\"\n");
    write_new_file(root/"frontend/index.html",
      "<!doctype html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"utf-8\">\n  <meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n  <title>SE Web</title>\n  <link rel=\"stylesheet\" href=\"./style.css\">\n</head>\n<body>\n  <main id=\"app\">Hello from SE Web</main>\n  <script type=\"module\" src=\"./app.js\"></script>\n</body>\n</html>\n");
    write_new_file(root/"frontend/style.css","body { font-family: system-ui, sans-serif; margin: 2rem; }\n");
    write_new_file(root/"frontend/app.js","const app = document.querySelector('#app');\nconsole.log('SE Web frontend ready', app);\n");
    write_new_file(root/"frontend/app.ts","const message: string = 'SE Web TypeScript frontend ready';\nconsole.log(message);\n");
    write_new_file(root/"README.md",
      "# "+root.filename().string()+"\n\nThis project keeps the SE backend and browser frontend together.\n\n- `backend/main.se` - SE backend entry point\n- `frontend/index.html` - HTML\n- `frontend/style.css` - CSS\n- `frontend/app.js` - JavaScript\n- `frontend/app.ts` - TypeScript source\n\nRun the current backend entry point with:\n\n```sh\nse run backend/main.se\n```\n\nThe built-in SE HTTP server/API layer is not implemented yet; this template establishes the stable project structure for it.\n");
    std::cout<<"Created SE web project at "<<root.string()<<"\n";
    std::cout<<"Frontend supports HTML, CSS, JavaScript, and TypeScript files side-by-side with SE.\n";
    return 0;
  }
  throw std::runtime_error("Unknown project kind '"+kind+"'. Use app or web.");
}

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
  std::cout<<"SE 0.4.0-dev\nType SE code. Use a blank line to finish a block. Ctrl-D exits.\n";
  std::string pending,line; s::Checker checker; s::Interpreter vm(std::cin,std::cout);
  auto execute=[&]{if(pending.empty())return;try{s::Lexer lexer(pending);s::Parser parser(lexer.scan());auto program=parser.parse();checker.check(program);vm.run(program);}catch(const s::Error&e){std::cerr<<s::format_error(e,pending);}catch(const s::RuntimeFailure&e){std::cerr<<e.what()<<'\n';}pending.clear();};
  while(true){std::cout<<(pending.empty()?"> ":". ");if(!std::getline(std::cin,line)){execute();break;}if(line.empty()&&!pending.empty()){execute();continue;}pending+=line+'\n';if(line.find_first_not_of(' ')==0&&line.rfind("if ",0)!=0&&line.rfind("for ",0)!=0&&line.rfind("while ",0)!=0&&line.rfind("repeat ",0)!=0&&line.rfind("make ",0)!=0&&line.rfind("type ",0)!=0&&line!="try")execute();}
}
}
int main(int argc,char**argv){
  try{
    if(argc==1){repl();return 0;}
    if(argc==2&&std::string(argv[1])=="--version"){std::cout<<"SE 0.4.0-dev\n";return 0;}
    if(argc==2&&(std::string(argv[1])=="help"||std::string(argv[1])=="--help"||std::string(argv[1])=="-h")){print_help();return 0;}
    if(argc==2&&std::string(argv[1])=="doctor")return doctor_command();
    if(argc==4&&std::string(argv[1])=="new")return new_project(argv[2],argv[3]);
    if(argc>=3&&std::string(argv[1])=="bind"){
      if(argc>4){std::cerr<<"Use: se bind module.sbind [output-directory]\n";return 2;}
      std::filesystem::path definition=argv[2];
      std::filesystem::path output=argc==4?std::filesystem::path(argv[3]):definition.parent_path();
      if(output.empty())output=".";
      return bind_command(definition,output);
    }
    if(argc!=3){print_help();return 2;}
    std::string cmd=argv[1];if(cmd!="run"&&cmd!="check"&&cmd!="build"){std::cerr<<"Unknown command '"<<cmd<<"'.\n\n";print_help();return 2;}return file_command(cmd,argv[2]);
  }catch(const std::exception&e){std::cerr<<"Error: "<<e.what()<<'\n';return 1;}
}
