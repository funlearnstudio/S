#pragma once

#include "s/ast.hpp"
#include "s/error.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace s {
struct ListData;
struct FunctionData;
class Value {
public:
  using Data=std::variant<std::monostate,std::int64_t,double,bool,std::string,std::shared_ptr<ListData>,std::shared_ptr<FunctionData>>;
  Value()=default; template<class T> Value(T v):data_(std::move(v)){}
  const Data& data() const{return data_;} Data& data(){return data_;}
  std::string type_name() const; std::string text() const; bool truth(SourcePos) const;
private: Data data_;
};
struct ListData { std::vector<Value> items; };
class Environment;
struct FunctionData { std::vector<std::string> params; ast::Block body; std::shared_ptr<Environment> closure; };

class Environment : public std::enable_shared_from_this<Environment> {
public:
  explicit Environment(std::shared_ptr<Environment> parent={}):parent_(std::move(parent)){}
  Value get(const std::string&,SourcePos) const; void set(const std::string&,Value); void define(const std::string&,Value); bool has(const std::string&) const;
private: std::unordered_map<std::string,Value> values_; std::shared_ptr<Environment> parent_;
};

Value binary(TokenKind,const Value&,const Value&,SourcePos);
} // namespace s
