#include "s/component_web.hpp"
#include "s/error.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace s {
namespace {

using Bindings=std::unordered_map<std::string,ast::ExprPtr>;

struct Component {
  std::shared_ptr<ast::Function> function;
  std::vector<std::shared_ptr<ast::WebSection>> html;
  std::vector<std::shared_ptr<ast::WebSection>> css;
  std::vector<std::shared_ptr<ast::WebSection>> js;
  std::vector<ast::StmtPtr> simple;
};

std::string html_escape(const std::string& value){
  std::string out;
  for(char c:value){
    if(c=='&')out+="&amp;";
    else if(c=='<')out+="&lt;";
    else if(c=='>')out+="&gt;";
    else if(c=='\"')out+="&quot;";
    else out+=c;
  }
  return out;
}

std::string js_quote(const std::string& value){
  std::ostringstream out;out<<'\"';
  for(unsigned char c:value){
    switch(c){
      case '\\':out<<"\\\\";break;
      case '\"':out<<"\\\"";break;
      case '\n':out<<"\\n";break;
      case '\r':out<<"\\r";break;
      case '\t':out<<"\\t";break;
      default:
        if(c<0x20)out<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<static_cast<int>(c)<<std::dec;
        else out<<static_cast<char>(c);
    }
  }
  out<<'\"';return out.str();
}

ast::ExprPtr resolve(const ast::ExprPtr& value,const Bindings& bindings){
  if(auto variable=std::dynamic_pointer_cast<ast::Variable>(value)){
    auto found=bindings.find(variable->name);
    if(found!=bindings.end())return resolve(found->second,bindings);
  }
  return value;
}

std::string text_value(const ast::ExprPtr& raw,const Bindings& bindings,const std::string& what){
  auto value=resolve(raw,bindings);
  if(auto literal=std::dynamic_pointer_cast<ast::Literal>(value)){
    if(auto text=std::get_if<std::string>(&literal->value))return *text;
    if(auto integer=std::get_if<std::int64_t>(&literal->value))return std::to_string(*integer);
    if(auto number=std::get_if<double>(&literal->value)){std::ostringstream out;out<<std::setprecision(15)<<*number;return out.str();}
    if(auto boolean=std::get_if<bool>(&literal->value))return *boolean?"true":"false";
  }
  throw Error(raw->pos,what+" needs a value that can be known while building this page.","Pass Text, a number, Bool, or a component parameter with one of those values.");
}

std::string css_property(std::string name){std::replace(name.begin(),name.end(),'_','-');return name;}

bool unitless_css(const std::string& name){
  static const std::unordered_set<std::string> names={"opacity","z-index","font-weight","line-height","order","flex","flex-grow","flex-shrink","scale","zoom"};
  return names.contains(css_property(name));
}

std::string css_value(const ast::ExprPtr& raw,const Bindings& bindings,const std::string& property){
  auto value=resolve(raw,bindings);
  if(auto literal=std::dynamic_pointer_cast<ast::Literal>(value)){
    if(auto text=std::get_if<std::string>(&literal->value))return *text;
    if(auto integer=std::get_if<std::int64_t>(&literal->value))return std::to_string(*integer)+(unitless_css(property)?"":"px");
    if(auto number=std::get_if<double>(&literal->value)){std::ostringstream out;out<<std::setprecision(15)<<*number;if(!unitless_css(property))out<<"px";return out.str();}
    if(auto boolean=std::get_if<bool>(&literal->value))return *boolean?"true":"false";
  }
  throw Error(raw->pos,"CSS value for '"+property+"' must be known while building the page.");
}

bool is_web_component(const std::shared_ptr<ast::Function>& function){
  for(const auto& statement:function->body)if(std::dynamic_pointer_cast<ast::WebSection>(statement))return true;
  return false;
}

Component component_from(const std::shared_ptr<ast::Function>& function){
  Component out;out.function=function;
  for(const auto& statement:function->body){
    if(auto section=std::dynamic_pointer_cast<ast::WebSection>(statement)){
      if(section->kind=="html")out.html.push_back(section);
      else if(section->kind=="css"||section->kind=="style")out.css.push_back(section);
      else if(section->kind=="js")out.js.push_back(section);
    }else out.simple.push_back(statement);
  }
  return out;
}

std::string root_tag(const Component& component){
  for(const auto& section:component.html)if(!section->elements.empty()&&section->elements.front().tag!="native"&&section->elements.front().tag!="text")return section->elements.front().tag;
  return "div";
}

std::string render_element(const ast::WebElement& element,const Bindings& bindings,const std::string& component_class,const std::string& instance,bool root){
  if(element.tag=="native"){
    if(element.values.size()!=1)throw Error(element.pos,"native HTML needs exactly one Text value.");
    return text_value(element.values.front(),bindings,"native HTML");
  }
  if(element.tag=="text"){
    if(element.values.empty())return "";
    return html_escape(text_value(element.values.front(),bindings,"HTML text"));
  }
  std::ostringstream out;out<<'<'<<element.tag;
  if(root){out<<" class=\""<<component_class<<"\" data-se-instance=\""<<instance<<"\"";}
  if(!element.values.empty()){
    auto possible_map=std::dynamic_pointer_cast<ast::Map>(resolve(element.values.front(),bindings));
    if(possible_map){
      for(const auto& [key,value]:possible_map->items){
        auto name=text_value(key,bindings,"HTML attribute name");
        auto val=text_value(value,bindings,"HTML attribute value");
        out<<' '<<name<<"=\""<<html_escape(val)<<"\"";
      }
    }
  }
  out<<'>';
  if(!element.values.empty()&&!std::dynamic_pointer_cast<ast::Map>(resolve(element.values.front(),bindings)))out<<html_escape(text_value(element.values.front(),bindings,"HTML content"));
  for(const auto& child:element.children)out<<render_element(child,bindings,component_class,instance,false);
  static const std::unordered_set<std::string> void_tags={"area","base","br","col","embed","hr","img","input","link","meta","param","source","track","wbr"};
  if(!void_tags.contains(element.tag))out<<"</"<<element.tag<<'>';
  return out.str();
}

std::string simple_component_html(const Component& component,const Bindings& bindings,const std::string& component_class,const std::string& instance){
  std::ostringstream content;
  for(const auto& statement:component.simple){
    auto expression_statement=std::dynamic_pointer_cast<ast::ExprStmt>(statement);
    auto call=expression_statement?std::dynamic_pointer_cast<ast::Call>(expression_statement->value):nullptr;
    auto callee=call?std::dynamic_pointer_cast<ast::Variable>(call->callee):nullptr;
    if(callee&&callee->name=="text"&&call->args.size()==1){content<<html_escape(text_value(call->args.front(),bindings,"component text"));continue;}
    if(expression_statement)throw Error(statement->pos,"This simple component line is not a Web content form.","Use 'text value', or add an explicit html section.");
  }
  return "<div class=\""+component_class+"\" data-se-instance=\""+instance+"\">"+content.str()+"</div>";
}

std::string component_html(const Component& component,const Bindings& bindings,const std::string& instance){
  const auto class_name="se-"+component.function->name;
  if(component.html.empty())return simple_component_html(component,bindings,class_name,instance);
  std::vector<ast::WebElement> roots;
  for(const auto& section:component.html)roots.insert(roots.end(),section->elements.begin(),section->elements.end());
  if(roots.empty())return "<div class=\""+class_name+"\" data-se-instance=\""+instance+"\"></div>";
  if(roots.size()==1)return render_element(roots.front(),bindings,class_name,instance,true);
  std::ostringstream out;out<<"<div class=\""<<class_name<<"\" data-se-instance=\""<<instance<<"\">";
  for(const auto& root:roots)out<<render_element(root,bindings,class_name,instance,false);
  out<<"</div>";return out.str();
}

void emit_css_items(std::ostringstream& out,const std::vector<ast::WebCssItem>& items,const std::string& selector,const Bindings& bindings,const std::string& root_element){
  std::vector<const ast::WebCssItem*> declarations;
  for(const auto& item:items)if(item.children.empty())declarations.push_back(&item);
  if(!declarations.empty()){
    out<<selector<<" {\n";
    for(const auto* item:declarations){
      if(item->name=="native"){
        if(item->values.size()!=1)throw Error(item->pos,"native CSS needs exactly one Text value.");
        out<<"  "<<text_value(item->values.front(),bindings,"native CSS")<<"\n";
        continue;
      }
      if(item->values.size()!=1)throw Error(item->pos,"CSS property '"+item->name+"' needs one value.");
      out<<"  "<<css_property(item->name)<<": "<<css_value(item->values.front(),bindings,item->name)<<";\n";
    }
    out<<"}\n\n";
  }
  for(const auto& item:items)if(!item.children.empty()){
    std::string nested=item.name;
    if(nested.rfind(root_element+":",0)==0)nested=selector+nested.substr(root_element.size());
    else if(nested.rfind("&",0)==0)nested=selector+nested.substr(1);
    else nested=selector+" "+nested;
    emit_css_items(out,item.children,nested,bindings,root_element);
  }
}

std::string expression_js(const ast::ExprPtr& raw,const Bindings& bindings){
  auto e=resolve(raw,bindings);
  if(auto literal=std::dynamic_pointer_cast<ast::Literal>(e)){
    if(auto integer=std::get_if<std::int64_t>(&literal->value))return std::to_string(*integer);
    if(auto number=std::get_if<double>(&literal->value)){std::ostringstream out;out<<std::setprecision(17)<<*number;return out.str();}
    if(auto text=std::get_if<std::string>(&literal->value))return js_quote(*text);
    return std::get<bool>(literal->value)?"true":"false";
  }
  if(auto variable=std::dynamic_pointer_cast<ast::Variable>(e))return variable->name;
  if(auto unary=std::dynamic_pointer_cast<ast::Unary>(e))return std::string(unary->op==TokenKind::Not?"!":"-")+"("+expression_js(unary->value,bindings)+")";
  if(auto binary=std::dynamic_pointer_cast<ast::Binary>(e)){
    std::string op;
    switch(binary->op){
      case TokenKind::Plus:op="+";break;case TokenKind::Minus:op="-";break;case TokenKind::Star:op="*";break;case TokenKind::Slash:op="/";break;case TokenKind::Percent:op="%";break;case TokenKind::Power:op="**";break;
      case TokenKind::EqualEqual:op="===";break;case TokenKind::BangEqual:op="!==";break;case TokenKind::Greater:op=">";break;case TokenKind::Less:op="<";break;case TokenKind::GreaterEqual:op=">=";break;case TokenKind::LessEqual:op="<=";break;case TokenKind::And:op="&&";break;case TokenKind::Or:op="||";break;
      default:throw Error(e->pos,"This expression is not supported in component JavaScript yet.");
    }
    return "("+expression_js(binary->left,bindings)+" "+op+" "+expression_js(binary->right,bindings)+")";
  }
  if(auto member=std::dynamic_pointer_cast<ast::Member>(e))return expression_js(member->value,bindings)+"."+member->name;
  if(auto call=std::dynamic_pointer_cast<ast::Call>(e)){
    std::string out=expression_js(call->callee,bindings)+"(";
    for(std::size_t i=0;i<call->args.size();++i){if(i)out+=",";out+=expression_js(call->args[i],bindings);}return out+")";
  }
  throw Error(e->pos,"This SE value cannot be lowered to browser JavaScript yet.");
}

class EventEmitter {
public:
  explicit EventEmitter(Bindings bindings):bindings_(std::move(bindings)){}
  std::string block(const ast::Block& body,int depth=0){for(const auto& statement:body)emit(statement,depth);return out_.str();}
private:
  Bindings bindings_;
  std::ostringstream out_;
  std::unordered_set<std::string> locals_;
  void indent(int depth){for(int i=0;i<depth;++i)out_<<"  ";}
  void emit(const ast::StmtPtr& statement,int depth){
    if(auto say=std::dynamic_pointer_cast<ast::Say>(statement)){indent(depth);out_<<"console.log("<<expression_js(say->value,bindings_)<<");\n";return;}
    if(auto expr=std::dynamic_pointer_cast<ast::ExprStmt>(statement)){
      indent(depth);
      if(auto variable=std::dynamic_pointer_cast<ast::Variable>(expr->value))out_<<variable->name<<"();\n";
      else out_<<expression_js(expr->value,bindings_)<<";\n";
      return;
    }
    if(auto assign=std::dynamic_pointer_cast<ast::Assign>(statement)){
      auto variable=std::dynamic_pointer_cast<ast::Variable>(assign->target);
      indent(depth);
      if(variable&&!bindings_.contains(variable->name)&&!locals_.contains(variable->name)){locals_.insert(variable->name);out_<<"let ";}
      out_<<expression_js(assign->target,bindings_)<<" = "<<expression_js(assign->value,bindings_)<<";\n";return;
    }
    if(auto branch=std::dynamic_pointer_cast<ast::If>(statement)){
      indent(depth);out_<<"if("<<expression_js(branch->condition,bindings_)<<"){\n";for(const auto& item:branch->then_block)emit(item,depth+1);indent(depth);out_<<"}";
      if(!branch->else_block.empty()){out_<<"else{\n";for(const auto& item:branch->else_block)emit(item,depth+1);indent(depth);out_<<"}";}out_<<"\n";return;
    }
    if(auto repeat=std::dynamic_pointer_cast<ast::Repeat>(statement)){
      auto name="__seRepeat"+std::to_string(locals_.size());indent(depth);out_<<"for(let "<<name<<"=0;"<<name<<"<"<<expression_js(repeat->count,bindings_)<<";"<<name<<"+=1){\n";for(const auto& item:repeat->body)emit(item,depth+1);indent(depth);out_<<"}\n";return;
    }
    if(auto loop=std::dynamic_pointer_cast<ast::While>(statement)){indent(depth);out_<<"while("<<expression_js(loop->condition,bindings_)<<"){\n";for(const auto& item:loop->body)emit(item,depth+1);indent(depth);out_<<"}\n";return;}
    if(auto give=std::dynamic_pointer_cast<ast::Give>(statement)){indent(depth);out_<<"return "<<expression_js(give->value,bindings_)<<";\n";return;}
    if(auto fail=std::dynamic_pointer_cast<ast::Fail>(statement)){indent(depth);out_<<"throw new Error(String("<<expression_js(fail->value,bindings_)<<"));\n";return;}
    throw Error(statement->pos,"This SE statement is not supported inside a component event yet.");
  }
};

Bindings bind_component(const Component& component,const std::shared_ptr<ast::Call>& call){
  if(call->args.size()!=component.function->params.size())throw Error(call->pos,"Component '"+component.function->name+"' needs "+std::to_string(component.function->params.size())+" value(s), but got "+std::to_string(call->args.size())+".");
  Bindings bindings;for(std::size_t i=0;i<call->args.size();++i)bindings[component.function->params[i]]=call->args[i];return bindings;
}

std::shared_ptr<ast::Call> component_call(const ast::StmtPtr& statement){
  auto expression=std::dynamic_pointer_cast<ast::ExprStmt>(statement);
  return expression?std::dynamic_pointer_cast<ast::Call>(expression->value):nullptr;
}

std::string component_name(const std::shared_ptr<ast::Call>& call){
  if(!call)return {};
  auto variable=std::dynamic_pointer_cast<ast::Variable>(call->callee);
  return variable?variable->name:std::string{};
}

} // namespace

bool has_component_web(const ast::Program& program){
  for(const auto& statement:program.statements){
    if(std::dynamic_pointer_cast<ast::Page>(statement))return true;
    if(auto function=std::dynamic_pointer_cast<ast::Function>(statement))if(is_web_component(function))return true;
  }
  return false;
}

WebBundle ComponentWebCompiler::generate(const ast::Program& program){
  std::unordered_map<std::string,Component> components;
  std::vector<std::shared_ptr<ast::Page>> pages;
  std::vector<std::shared_ptr<ast::WebSection>> global_css;
  for(const auto& statement:program.statements){
    if(auto function=std::dynamic_pointer_cast<ast::Function>(statement);function&&is_web_component(function))components.emplace(function->name,component_from(function));
    else if(auto page=std::dynamic_pointer_cast<ast::Page>(statement))pages.push_back(page);
    else if(auto section=std::dynamic_pointer_cast<ast::WebSection>(statement);section&&(section->kind=="css"||section->kind=="style"))global_css.push_back(section);
  }
  if(pages.empty())throw Error({1,1},"This component-style web build needs a page block.","Add page \"/\" and place your components inside it.");
  std::shared_ptr<ast::Page> page=pages.front();
  for(const auto& candidate:pages){auto literal=std::dynamic_pointer_cast<ast::Literal>(candidate->route);if(literal){if(auto route=std::get_if<std::string>(&literal->value);route&&*route=="/"){page=candidate;break;}}}
  auto route_literal=std::dynamic_pointer_cast<ast::Literal>(page->route);
  if(!route_literal||!std::get_if<std::string>(&route_literal->value))throw Error(page->pos,"page route must be literal Text, like page \"/\".");

  std::ostringstream body,css,js;
  std::size_t instance_number=0;
  std::unordered_set<std::string> emitted_css;
  for(const auto& section:global_css)emit_css_items(css,section->css,"body",{},"body");

  for(const auto& statement:page->body){
    auto call=component_call(statement);auto name=component_name(call);
    if(name.empty())throw Error(statement->pos,"A page currently contains component calls.","Define a component with make, then write ComponentName values inside page.");
    auto found=components.find(name);
    if(found==components.end())throw Error(statement->pos,"'"+name+"' is not a Web component.","Give it an html, css/style, or js section inside make.");
    auto bindings=bind_component(found->second,call);
    auto instance="se-"+name+"-"+std::to_string(++instance_number);
    body<<"    "<<component_html(found->second,bindings,instance)<<"\n";
    if(!emitted_css.contains(name)){
      const auto selector=".se-"+name;const auto tag=root_tag(found->second);
      for(const auto& section:found->second.css)emit_css_items(css,section->css,selector,{},tag);
      emitted_css.insert(name);
    }
    for(const auto& section:found->second.js){
      for(const auto& native:section->native)js<<text_value(native,bindings,"native JavaScript")<<"\n";
      for(const auto& event:section->events){
        js<<"document.querySelector("<<js_quote("[data-se-instance=\""+instance+"\"]")<<").addEventListener("<<js_quote(event.event)<<", event => {\n";
        js<<EventEmitter(bindings).block(event.body,1);
        js<<"});\n";
      }
    }
  }

  std::ostringstream html;
  html<<"<!doctype html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"utf-8\">\n  <meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n  <title>SE Web</title>\n  <link rel=\"stylesheet\" href=\"./style.css\">\n</head>\n<body>\n";
  html<<body.str();
  html<<"  <script type=\"module\" src=\"./app.js\"></script>\n</body>\n</html>\n";
  std::string banner="// Generated by SE component web build. Edit the .se source, not this file.\n";
  auto javascript=banner+js.str();
  auto typescript="// TypeScript-compatible output generated from SE.\n"+javascript;
  return {html.str(),css.str(),std::move(javascript),std::move(typescript)};
}

} // namespace s