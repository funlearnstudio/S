#include "s/value.hpp"
#include <cmath>
#include <sstream>

namespace s {
std::string Value::type_name() const {
  static const char* names[]={"None","Int","Num","Bool","Text","List","Function"};
  return names[data_.index()];
}
std::string Value::text() const {
  if(std::holds_alternative<std::monostate>(data_)) return "none";
  if(auto p=std::get_if<std::int64_t>(&data_)) return std::to_string(*p);
  if(auto p=std::get_if<double>(&data_)){ std::ostringstream o;o<<*p;return o.str(); }
  if(auto p=std::get_if<bool>(&data_)) return *p?"true":"false";
  if(auto p=std::get_if<std::string>(&data_)) return *p;
  if(auto p=std::get_if<std::shared_ptr<ListData>>(&data_)){ std::string r="["; for(std::size_t i=0;i<(*p)->items.size();++i){if(i)r+=", ";r+=(*p)->items[i].text();}return r+"]"; }
  return "<function>";
}
bool Value::truth(SourcePos pos) const {
  if(auto p=std::get_if<bool>(&data_)) return *p;
  throw Error(pos,"An 'if' or 'while' condition must be Bool, but this is "+type_name()+".");
}
Value Environment::get(const std::string& n,SourcePos p) const { auto i=values_.find(n);if(i!=values_.end())return i->second;if(parent_)return parent_->get(n,p);throw Error(p,"I don't know what \""+n+"\" is.","Create it first with "+n+" = value."); }
void Environment::set(const std::string& n,Value v){auto i=values_.find(n);if(i!=values_.end()){i->second=std::move(v);return;}if(parent_&&parent_->has(n)){parent_->set(n,std::move(v));return;}values_[n]=std::move(v);}
void Environment::define(const std::string& n,Value v){values_[n]=std::move(v);}
bool Environment::has(const std::string& n) const{return values_.contains(n)||(parent_&&parent_->has(n));}

static bool number(const Value& v){return std::holds_alternative<std::int64_t>(v.data())||std::holds_alternative<double>(v.data());}
static double num(const Value& v){return std::holds_alternative<std::int64_t>(v.data())?static_cast<double>(std::get<std::int64_t>(v.data())):std::get<double>(v.data());}
Value binary(TokenKind op,const Value& a,const Value& b,SourcePos p){
  if(op==TokenKind::EqualEqual||op==TokenKind::BangEqual){ bool same=false;
    if(a.data().index()==b.data().index()) same=a.text()==b.text(); else if(number(a)&&number(b)) same=num(a)==num(b);
    return op==TokenKind::EqualEqual?same:!same;
  }
  if(op==TokenKind::And||op==TokenKind::Or){bool x=a.truth(p),y=b.truth(p);return op==TokenKind::And?x&&y:x||y;}
  if(op==TokenKind::Plus && std::holds_alternative<std::string>(a.data())&&std::holds_alternative<std::string>(b.data())) return std::get<std::string>(a.data())+std::get<std::string>(b.data());
  if(!number(a)||!number(b)) throw Error(p,"'"+std::string(token_name(op))+"' cannot combine "+a.type_name()+" and "+b.type_name()+".","Use values of the same suitable type.");
  double x=num(a),y=num(b);
  switch(op){
    case TokenKind::Plus: if(std::holds_alternative<std::int64_t>(a.data())&&std::holds_alternative<std::int64_t>(b.data())) return std::get<std::int64_t>(a.data())+std::get<std::int64_t>(b.data()); return x+y;
    case TokenKind::Minus: if(std::holds_alternative<std::int64_t>(a.data())&&std::holds_alternative<std::int64_t>(b.data())) return std::get<std::int64_t>(a.data())-std::get<std::int64_t>(b.data()); return x-y;
    case TokenKind::Star: if(std::holds_alternative<std::int64_t>(a.data())&&std::holds_alternative<std::int64_t>(b.data())) return std::get<std::int64_t>(a.data())*std::get<std::int64_t>(b.data()); return x*y;
    case TokenKind::Slash: if(y==0)throw Error(p,"You cannot divide by zero.");return x/y;
    case TokenKind::Percent: if(y==0)throw Error(p,"You cannot divide by zero.");return std::fmod(x,y);
    case TokenKind::Power:return std::pow(x,y); case TokenKind::Greater:return x>y;case TokenKind::Less:return x<y;case TokenKind::GreaterEqual:return x>=y;case TokenKind::LessEqual:return x<=y;
    default:break;
  }
  throw Error(p,"This operation is not supported yet.");
}
} // namespace s
