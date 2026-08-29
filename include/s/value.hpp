#pragma once

#include "s/error.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace s {
class Value;
class Environment;
struct ListData;
struct MapData;
struct SetData;
struct CallableData;
struct ObjectData;
struct TypeData;
struct ModuleData;
struct FileData;
struct NativeHandleData;
struct ByteBufferData;

struct DurationData { std::int64_t milliseconds=0; };
struct TimeData { std::chrono::system_clock::time_point point; };
struct PathData { std::filesystem::path path; };
struct ErrorData { std::string message; std::string source; int line=0; std::string kind="Error"; };
struct ByteBufferData { std::vector<std::uint8_t> bytes; };

class Value {
public:
  using Data=std::variant<std::monostate,std::int64_t,double,bool,std::string,DurationData,TimeData,PathData,
                          std::shared_ptr<ErrorData>,std::shared_ptr<ListData>,std::shared_ptr<MapData>,std::shared_ptr<SetData>,
                          std::shared_ptr<CallableData>,std::shared_ptr<ObjectData>,std::shared_ptr<TypeData>,
                          std::shared_ptr<ModuleData>,std::shared_ptr<FileData>,std::shared_ptr<NativeHandleData>,
                          std::shared_ptr<ByteBufferData>>;
  Value()=default; template<class T> Value(T v):data_(std::move(v)){}
  const Data& data() const{return data_;} Data& data(){return data_;}
  std::string type_name() const; std::string text() const; bool truth(SourcePos) const;
private: Data data_;
};

struct ListData { std::vector<Value> items; };
struct MapData { std::vector<std::pair<std::string,Value>> items; };
struct SetData { std::vector<Value> items; };
struct CallableData {
  std::string name;
  std::size_t min_args=0,max_args=0;
  bool variadic=false;
  std::function<Value(const std::vector<Value>&,SourcePos)> call;
};
struct ObjectData { std::shared_ptr<TypeData> type; std::unordered_map<std::string,Value> fields; };
struct ModuleData { std::string name; std::unordered_map<std::string,Value> exports; };
struct FileData {
  std::filesystem::path path;
  std::shared_ptr<std::fstream> stream;
  bool closed=false;
  ~FileData(){ if(stream&&stream->is_open()) stream->close(); }
};
struct NativeHandleData {
  std::shared_ptr<void> resource;
  std::string tag;
};

class RuntimeFailure : public std::exception {
public:
  explicit RuntimeFailure(ErrorData e):error_(std::move(e)),what_(error_.kind+": "+error_.message){}
  const char* what() const noexcept override{return what_.c_str();}
  const ErrorData& error() const{return error_;}
private: ErrorData error_; std::string what_;
};

class Environment : public std::enable_shared_from_this<Environment> {
public:
  explicit Environment(std::shared_ptr<Environment> parent={},std::shared_ptr<ObjectData> self={}):parent_(std::move(parent)),self_(std::move(self)){}
  Value get(const std::string&,SourcePos) const; void set(const std::string&,Value); void define(const std::string&,Value); bool has(const std::string&) const;
  const std::unordered_map<std::string,Value>& values() const{return values_;}
  std::shared_ptr<ObjectData> self() const{return self_;}
private:
  std::unordered_map<std::string,Value> values_; std::shared_ptr<Environment> parent_; std::shared_ptr<ObjectData> self_;
};

bool value_equal(const Value&,const Value&);
Value binary(TokenKind,const Value&,const Value&,SourcePos);
std::string path_text(const Value&,SourcePos);

} // namespace s
