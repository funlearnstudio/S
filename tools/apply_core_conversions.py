from pathlib import Path


def replace_once(path: str, old: str, new: str) -> None:
    p = Path(path)
    text = p.read_text()
    if old not in text:
        raise SystemExit(f"expected text not found in {path}: {old[:80]!r}")
    if text.count(old) != 1:
        raise SystemExit(f"expected exactly one match in {path}, found {text.count(old)}")
    p.write_text(text.replace(old, new, 1))


# Runtime conversion helpers and globally available conversion functions.
replace_once(
    "src/interpreter/interpreter.cpp",
    "#include <fstream>\n#include <random>",
    "#include <fstream>\n#include <limits>\n#include <random>",
)

helper_anchor = '''std::int64_t int_value(const Value&v,SourcePos p,const std::string&name){
  if(auto n=std::get_if<std::int64_t>(&v.data()))return *n;
  throw Error(p,name+" needs an Int.");
}
'''
helper_code = helper_anchor + '''std::string trim_conversion_text(std::string text){
  auto ws=[](unsigned char c){return std::isspace(c)!=0;};
  while(!text.empty()&&ws(static_cast<unsigned char>(text.front())))text.erase(text.begin());
  while(!text.empty()&&ws(static_cast<unsigned char>(text.back())))text.pop_back();
  return text;
}
bool one_utf8_character(const std::string& text){
  if(text.empty())return false;
  const auto first=static_cast<unsigned char>(text[0]);
  std::size_t size=0;
  if(first<=0x7f)size=1;
  else if(first>=0xc2&&first<=0xdf)size=2;
  else if(first>=0xe0&&first<=0xef)size=3;
  else if(first>=0xf0&&first<=0xf4)size=4;
  else return false;
  if(text.size()!=size)return false;
  for(std::size_t i=1;i<size;++i)if((static_cast<unsigned char>(text[i])&0xc0)!=0x80)return false;
  if(size==3){
    const auto second=static_cast<unsigned char>(text[1]);
    if(first==0xe0&&second<0xa0)return false;
    if(first==0xed&&second>=0xa0)return false;
  }
  if(size==4){
    const auto second=static_cast<unsigned char>(text[1]);
    if(first==0xf0&&second<0x90)return false;
    if(first==0xf4&&second>0x8f)return false;
  }
  return true;
}
std::string utf8_from_codepoint(std::int64_t code,SourcePos p){
  if(code<0||code>0x10ffff||(code>=0xd800&&code<=0xdfff))throw Error(p,"char needs a valid Unicode code point or one-character Text.");
  std::string out;
  if(code<=0x7f)out.push_back(static_cast<char>(code));
  else if(code<=0x7ff){out.push_back(static_cast<char>(0xc0|(code>>6)));out.push_back(static_cast<char>(0x80|(code&0x3f)));}
  else if(code<=0xffff){out.push_back(static_cast<char>(0xe0|(code>>12)));out.push_back(static_cast<char>(0x80|((code>>6)&0x3f)));out.push_back(static_cast<char>(0x80|(code&0x3f)));}
  else{out.push_back(static_cast<char>(0xf0|(code>>18)));out.push_back(static_cast<char>(0x80|((code>>12)&0x3f)));out.push_back(static_cast<char>(0x80|((code>>6)&0x3f)));out.push_back(static_cast<char>(0x80|(code&0x3f)));}
  return out;
}
Value convert_int_value(const Value& value,SourcePos p){
  if(auto n=std::get_if<std::int64_t>(&value.data()))return Value(*n);
  if(auto n=std::get_if<double>(&value.data())){
    if(!std::isfinite(*n)||std::trunc(*n)!=*n||*n<static_cast<double>(std::numeric_limits<std::int64_t>::min())||*n>static_cast<double>(std::numeric_limits<std::int64_t>::max()))throw Error(p,"int needs a whole number in Int range.");
    return Value(static_cast<std::int64_t>(*n));
  }
  auto text=std::get_if<std::string>(&value.data());
  if(!text)throw Error(p,"int needs Text, Int, or a whole Num.");
  auto cleaned=trim_conversion_text(*text);
  try{
    std::size_t used=0;
    auto parsed=std::stoll(cleaned,&used,10);
    if(used!=cleaned.size())throw Error(p,"Could not convert '"+*text+"' to Int.");
    return Value(static_cast<std::int64_t>(parsed));
  }catch(const Error&){throw;}catch(...){throw Error(p,"Could not convert '"+*text+"' to Int.");}
}
Value convert_num_value(const Value& value,SourcePos p){
  if(auto n=std::get_if<std::int64_t>(&value.data()))return Value(static_cast<double>(*n));
  if(auto n=std::get_if<double>(&value.data()))return Value(*n);
  auto text=std::get_if<std::string>(&value.data());
  if(!text)throw Error(p,"num needs Text, Int, or Num.");
  auto cleaned=trim_conversion_text(*text);
  try{
    std::size_t used=0;
    auto parsed=std::stod(cleaned,&used);
    if(used!=cleaned.size()||!std::isfinite(parsed))throw Error(p,"Could not convert '"+*text+"' to Num.");
    return Value(parsed);
  }catch(const Error&){throw;}catch(...){throw Error(p,"Could not convert '"+*text+"' to Num.");}
}
Value convert_bool_value(const Value& value,SourcePos p){
  if(auto b=std::get_if<bool>(&value.data()))return Value(*b);
  auto text=std::get_if<std::string>(&value.data());
  if(!text)throw Error(p,"bool needs Bool or Text 'true'/'false'.");
  auto cleaned=trim_conversion_text(*text);
  std::transform(cleaned.begin(),cleaned.end(),cleaned.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
  if(cleaned=="true")return Value(true);
  if(cleaned=="false")return Value(false);
  throw Error(p,"Could not convert '"+*text+"' to Bool. Use true or false.");
}
Value convert_char_value(const Value& value,SourcePos p){
  if(auto text=std::get_if<std::string>(&value.data())){
    if(!one_utf8_character(*text))throw Error(p,"char needs exactly one Unicode character.");
    return Value(*text);
  }
  if(auto code=std::get_if<std::int64_t>(&value.data()))return Value(utf8_from_codepoint(*code,p));
  throw Error(p,"char needs one-character Text or an Int Unicode code point.");
}
'''
replace_once("src/interpreter/interpreter.cpp", helper_anchor, helper_code)

builtin_anchor = '''  e->define("bytes",callable("bytes",1,1,[](const std::vector<Value>&a,SourcePos p){auto text=std::get_if<std::string>(&a[0].data());if(!text)throw Error(p,"bytes needs Text.");auto out=std::make_shared<ByteBufferData>();out->bytes.assign(text->begin(),text->end());return Value(out);}));
'''
builtin_code = builtin_anchor + '''  e->define("text",callable("text",1,1,[](const std::vector<Value>&a,SourcePos){return Value(a[0].text());}));
  e->define("string",callable("string",1,1,[](const std::vector<Value>&a,SourcePos){return Value(a[0].text());}));
  e->define("int",callable("int",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_int_value(a[0],p);}));
  e->define("integer",callable("integer",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_int_value(a[0],p);}));
  e->define("num",callable("num",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_num_value(a[0],p);}));
  e->define("double",callable("double",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_num_value(a[0],p);}));
  e->define("float",callable("float",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_num_value(a[0],p);}));
  e->define("bool",callable("bool",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_bool_value(a[0],p);}));
  e->define("boolean",callable("boolean",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_bool_value(a[0],p);}));
  e->define("char",callable("char",1,1,[](const std::vector<Value>&a,SourcePos p){return convert_char_value(a[0],p);}));
'''
replace_once("src/interpreter/interpreter.cpp", builtin_anchor, builtin_code)

# Static checker signatures. These are conversion functions, not new runtime storage types.
checker_anchor = '  g["bytes"]=fn({TypeInfo(TypeKind::Text)},TypeInfo(TypeKind::Bytes));\n'
checker_code = checker_anchor + '''  g["text"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
  g["string"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
  g["int"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Int));
  g["integer"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Int));
  g["num"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Num));
  g["double"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Num));
  g["float"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Num));
  g["bool"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));
  g["boolean"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Bool));
  g["char"]=fn({TypeInfo(TypeKind::Unknown)},TypeInfo(TypeKind::Text));
'''
replace_once("src/type/checker.cpp", checker_anchor, checker_code)

# Regression tests for aliases, parsing and Unicode char conversion.
test_anchor = '  EXPECT(run("x = [1, 2]\\nx[0] = 9\\nsay x[0]\\nsay x.len\\n")=="9\\n2\\n");\n'
test_code = test_anchor + '''
  // Core conversions and familiar aliases.
  EXPECT(run("say int \\\"42\\\"\\nsay integer \\\"-7\\\"\\nsay double \\\"3.5\\\"\\nsay float 2\\nsay string 99\\nsay text true\\nsay bool \\\"TRUE\\\"\\nsay boolean \\\"false\\\"\\nsay char \\\"é\\\"\\nsay char 65\\n")=="42\\n-7\\n3.5\\n2\\n99\\ntrue\\ntrue\\nfalse\\né\\nA\\n");
  EXPECT(run("number = int \\\"12\\\"\\nwhile number < 14\\n    number = number + 1\\nsay number\\n")=="14\\n");
  EXPECT(throws_error([]{run("say int \\\"12x\\\"\\n");}));
  EXPECT(throws_error([]{run("say char \\\"ab\\\"\\n");}));
'''
replace_once("tests/tests.cpp", test_anchor, test_code)

print("core conversions applied")
