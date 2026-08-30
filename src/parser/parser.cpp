#include "s/parser.hpp"
#include "s/error.hpp"
#include <algorithm>
#include <cctype>

namespace s {

const Token& Parser::peek(int o) const { return tokens_[std::min(at_+static_cast<std::size_t>(o),tokens_.size()-1)]; }
bool Parser::check(TokenKind k) const { return peek().kind==k; }
bool Parser::match(TokenKind k) { if(!check(k)) return false; ++at_; return true; }
const Token& Parser::take(TokenKind k,const std::string& message) { if(!check(k)) throw Error(peek().pos,message); return tokens_[at_++]; }
void Parser::line_end(){ if(match(TokenKind::Newline)) return; if(check(TokenKind::Dedent)||check(TokenKind::End)) return; throw Error(peek().pos,"I expected the line to end here."); }

ast::Program Parser::parse(){ ast::Program p; while(!check(TokenKind::End)){ if(match(TokenKind::Newline)) continue; p.statements.push_back(statement()); } return p; }

ast::Block Parser::block(){
  take(TokenKind::Newline,"Start the block on the next line.");
  take(TokenKind::Indent,"This line needs to be indented by 4 spaces.");
  ast::Block result;
  while(!check(TokenKind::Dedent)&&!check(TokenKind::End)){ if(match(TokenKind::Newline)) continue; result.push_back(statement()); }
  take(TokenKind::Dedent,"This block was not closed correctly.");
  return result;
}

ast::TypeRef Parser::type_ref(){
  auto name=take(TokenKind::Identifier,"Write a type name here.");
  std::vector<ast::TypeRef> args;
  if(match(TokenKind::LeftBracket)){
    if(check(TokenKind::RightBracket)) throw Error(peek().pos,"A generic type needs at least one type argument.");
    do { args.push_back(type_ref()); } while(match(TokenKind::Comma));
    take(TokenKind::RightBracket,"Close type arguments with ']'.");
  }
  return ast::TypeRef{name.text,std::move(args)};
}

std::shared_ptr<ast::Function> Parser::function(SourcePos start){
  auto name=take(TokenKind::Identifier,"Give this function a name.");
  std::vector<std::string> generics;
  if(match(TokenKind::LeftBracket)){
    if(check(TokenKind::RightBracket)) throw Error(peek().pos,"A generic function needs at least one type name.");
    do { generics.push_back(take(TokenKind::Identifier,"Write a generic type name.").text); } while(match(TokenKind::Comma));
    take(TokenKind::RightBracket,"Close generic type names with ']'.");
  }
  std::vector<std::string> params;
  std::vector<ast::TypeRef> param_types;
  while(check(TokenKind::Identifier)){
    params.push_back(tokens_[at_++].text);
    ast::TypeRef type;
    if(match(TokenKind::Colon)) type=type_ref();
    param_types.push_back(std::move(type));
  }
  ast::TypeRef result_type;
  if(match(TokenKind::Arrow)) result_type=type_ref();
  auto b=block();
  return std::make_shared<ast::Function>(start,name.text,std::move(params),std::move(b),std::move(generics),std::move(param_types),std::move(result_type));
}

std::shared_ptr<ast::Type> Parser::type_decl(SourcePos start){
  auto name=take(TokenKind::Identifier,"Give this type a name.");
  std::vector<std::string> generics;
  if(match(TokenKind::LeftBracket)){
    if(check(TokenKind::RightBracket)) throw Error(peek().pos,"A generic type needs at least one type name.");
    do { generics.push_back(take(TokenKind::Identifier,"Write a generic type name.").text); } while(match(TokenKind::Comma));
    take(TokenKind::RightBracket,"Close generic type names with ']'.");
  }
  take(TokenKind::Newline,"Start the type on the next line.");
  take(TokenKind::Indent,"Indent the fields and methods inside this type.");
  std::vector<ast::FieldDecl> fields; std::vector<std::shared_ptr<ast::Function>> methods;
  while(!check(TokenKind::Dedent)&&!check(TokenKind::End)){
    if(match(TokenKind::Newline)) continue;
    Token item=peek();
    if(match(TokenKind::Make)){ methods.push_back(function(item.pos)); continue; }
    auto field=take(TokenKind::Identifier,"Inside a type, write a field or a make method.");
    ast::TypeRef annotation;
    if(match(TokenKind::Colon)) annotation=type_ref();
    bool has_default=false;
    ast::ExprPtr value;
    if(match(TokenKind::Equal)){ value=expression(); has_default=true; }
    else if(annotation.empty()) throw Error(peek().pos,"Give this field a default value with '=', or write its type with ':'.");
    else value=std::make_shared<ast::Literal>(field.pos,std::string{});
    line_end();
    fields.push_back({field.pos,field.text,std::move(value),std::move(annotation),has_default});
  }
  take(TokenKind::Dedent,"This type was not closed correctly.");
  return std::make_shared<ast::Type>(start,name.text,std::move(fields),std::move(methods),std::move(generics));
}

ast::StmtPtr Parser::statement(){
  Token start=peek();
  if(match(TokenKind::Say)){ auto e=expression(); line_end(); return std::make_shared<ast::Say>(start.pos,e); }
  if(match(TokenKind::Use)){ auto n=take(TokenKind::Identifier,"Write a module name after 'use'."); line_end(); return std::make_shared<ast::Use>(start.pos,n.text); }
  if(match(TokenKind::Type)) return type_decl(start.pos);
  if(match(TokenKind::Match)){
    auto value=expression();
    take(TokenKind::Newline,"Start match cases on the next line.");
    take(TokenKind::Indent,"Indent match cases by 4 spaces.");
    std::vector<ast::MatchCase> cases; ast::Block fallback;
    while(!check(TokenKind::Dedent)&&!check(TokenKind::End)){
      if(match(TokenKind::Newline)) continue;
      Token item=peek();
      if(match(TokenKind::Case)){
        auto pattern=expression();
        auto body=block();
        cases.push_back({item.pos,std::move(pattern),std::move(body)});
        continue;
      }
      if(match(TokenKind::Else)){
        fallback=block();
        while(match(TokenKind::Newline)){}
        if(!check(TokenKind::Dedent)) throw Error(peek().pos,"'else' must be the last branch in a match.");
        break;
      }
      throw Error(peek().pos,"Inside match, use 'case value' or 'else'.");
    }
    take(TokenKind::Dedent,"This match block was not closed correctly.");
    return std::make_shared<ast::Match>(start.pos,value,std::move(cases),std::move(fallback));
  }
  if(match(TokenKind::If)){
    auto c=expression(); auto yes=block(); ast::Block no;
    if(match(TokenKind::Else)) no=block();
    return std::make_shared<ast::If>(start.pos,c,std::move(yes),std::move(no));
  }
  if(match(TokenKind::Try)){
    if(check(TokenKind::Newline)){
      auto body=block(); take(TokenKind::Else,"A try block needs 'else' to handle an error.");
      std::string name="error"; if(check(TokenKind::Identifier)) name=tokens_[at_++].text;
      auto no=block(); return std::make_shared<ast::Try>(start.pos,std::move(body),std::move(name),std::move(no));
    }
    --at_;
  }
  if(match(TokenKind::Fail)){ auto e=expression(); line_end(); return std::make_shared<ast::Fail>(start.pos,e); }
  if(match(TokenKind::Repeat)){ auto n=expression(); auto b=block(); return std::make_shared<ast::Repeat>(start.pos,n,std::move(b)); }
  if(match(TokenKind::For)){
    std::vector<std::string> names;
    names.push_back(take(TokenKind::Identifier,"Give the loop value a name after 'for'.").text);
    if(check(TokenKind::Identifier)) names.push_back(tokens_[at_++].text);
    take(TokenKind::In,"Use 'in' before the values to loop over."); auto values=expression(); auto b=block();
    return std::make_shared<ast::For>(start.pos,std::move(names),values,std::move(b));
  }
  if(match(TokenKind::While)){ auto c=expression(); auto b=block(); return std::make_shared<ast::While>(start.pos,c,std::move(b)); }
  if(match(TokenKind::Make)) return function(start.pos);
  if(match(TokenKind::Give)){ auto e=expression(); line_end(); return std::make_shared<ast::Give>(start.pos,e); }
  auto left=expression();
  if(match(TokenKind::Equal)){
    auto value=expression(); ast::Block init;
    if(check(TokenKind::Newline)&&peek(1).kind==TokenKind::Indent) init=block(); else line_end();
    return std::make_shared<ast::Assign>(start.pos,left,value,std::move(init));
  }
  line_end(); return std::make_shared<ast::ExprStmt>(start.pos,left);
}

int Parser::precedence(TokenKind k) const {
  switch(k){
    case TokenKind::Or:return 1; case TokenKind::And:return 2;
    case TokenKind::EqualEqual:case TokenKind::BangEqual:return 3;
    case TokenKind::Greater:case TokenKind::Less:case TokenKind::GreaterEqual:case TokenKind::LessEqual:case TokenKind::In:return 4;
    case TokenKind::Range:return 5; case TokenKind::Plus:case TokenKind::Minus:return 6;
    case TokenKind::Star:case TokenKind::Slash:case TokenKind::Percent:return 7;
    case TokenKind::Power:return 8; default:return -1;
  }
}
bool Parser::expression_start(TokenKind k) const {
  return k==TokenKind::Identifier||k==TokenKind::Integer||k==TokenKind::Number||k==TokenKind::String||k==TokenKind::Duration||
         k==TokenKind::True||k==TokenKind::False||k==TokenKind::LeftParen||k==TokenKind::LeftBracket||k==TokenKind::Ask||
         k==TokenKind::Set||k==TokenKind::Map||k==TokenKind::Try;
}

ast::ExprPtr Parser::expression(int min_prec){
  auto left=postfix(prefix());
  while(true){ int p=precedence(peek().kind); if(p<min_prec) break; Token op=tokens_[at_++]; auto right=expression(p+(op.kind==TokenKind::Power?0:1));
    if(op.kind==TokenKind::Range) left=std::make_shared<ast::Range>(op.pos,left,right); else left=std::make_shared<ast::Binary>(op.pos,left,op.kind,right);
  }
  return left;
}

std::vector<ast::ExprPtr> Parser::bracket_items(TokenKind close){
  std::vector<ast::ExprPtr> items;
  if(!check(close)) do { items.push_back(expression()); } while(match(TokenKind::Comma));
  take(close,"Close this collection with ']'."); return items;
}

ast::ExprPtr Parser::prefix(){
  Token t=peek();
  if(match(TokenKind::Integer)) return std::make_shared<ast::Literal>(t.pos,static_cast<std::int64_t>(std::stoll(t.text)));
  if(match(TokenKind::Number)) return std::make_shared<ast::Literal>(t.pos,std::stod(t.text));
  if(match(TokenKind::String)) return std::make_shared<ast::Literal>(t.pos,t.text);
  if(match(TokenKind::Duration)){
    std::size_t cut=0; while(cut<t.text.size()&&std::isdigit(static_cast<unsigned char>(t.text[cut])))++cut;
    auto n=std::stoll(t.text.substr(0,cut)); auto unit=t.text.substr(cut);
    if(unit=="s") n*=1000; else if(unit=="min") n*=60000;
    return std::make_shared<ast::Duration>(t.pos,n);
  }
  if(match(TokenKind::True)||match(TokenKind::False)) return std::make_shared<ast::Literal>(t.pos,t.kind==TokenKind::True);
  if(match(TokenKind::Identifier)) return std::make_shared<ast::Variable>(t.pos,t.text);
  if(match(TokenKind::Minus)||match(TokenKind::Not)) return std::make_shared<ast::Unary>(t.pos,t.kind,expression(8));
  if(match(TokenKind::Ask)) return std::make_shared<ast::Ask>(t.pos,expression(9));
  if(match(TokenKind::Try)) return std::make_shared<ast::TryExpr>(t.pos,expression(9));
  if(match(TokenKind::LeftParen)){ auto e=expression(); take(TokenKind::RightParen,"Close this expression with ')'."); return e; }
  if(match(TokenKind::Set)){
    take(TokenKind::LeftBracket,"Write set values inside '[' and ']'."); auto items=bracket_items(TokenKind::RightBracket); return std::make_shared<ast::Set>(t.pos,std::move(items));
  }
  if(match(TokenKind::Map)){
    take(TokenKind::LeftBracket,"Write map entries inside '[' and ']'."); std::vector<std::pair<ast::ExprPtr,ast::ExprPtr>> items;
    if(!check(TokenKind::RightBracket)) do { auto k=expression(); take(TokenKind::Colon,"Separate a map key and value with ':'."); auto v=expression(); items.emplace_back(k,v); } while(match(TokenKind::Comma));
    take(TokenKind::RightBracket,"Close this map with ']'."); return std::make_shared<ast::Map>(t.pos,std::move(items));
  }
  if(match(TokenKind::LeftBracket)){
    if(check(TokenKind::RightBracket)){++at_;return std::make_shared<ast::List>(t.pos,std::vector<ast::ExprPtr>{});}
    auto first=expression();
    if(match(TokenKind::Colon)){
      std::vector<std::pair<ast::ExprPtr,ast::ExprPtr>> items; items.emplace_back(first,expression());
      while(match(TokenKind::Comma)){ auto k=expression(); take(TokenKind::Colon,"Separate a map key and value with ':'."); items.emplace_back(k,expression()); }
      take(TokenKind::RightBracket,"Close this map with ']'."); return std::make_shared<ast::Map>(t.pos,std::move(items));
    }
    std::vector<ast::ExprPtr> items{first}; while(match(TokenKind::Comma)) items.push_back(expression());
    take(TokenKind::RightBracket,"Close this list with ']'."); return std::make_shared<ast::List>(t.pos,std::move(items));
  }
  throw Error(t.pos,"I expected a value here.");
}

ast::ExprPtr Parser::postfix(ast::ExprPtr value){
  auto take_member_name=[&](){
    if(check(TokenKind::Identifier)||check(TokenKind::Map)) return tokens_[at_++];
    throw Error(peek().pos,"Write a member name after '.'.");
  };
  auto attach_members=[&](ast::ExprPtr arg){
    while(true){
      if(auto variable=std::dynamic_pointer_cast<ast::Variable>(arg);variable&&variable->type_args.empty()&&!variable->name.empty()&&std::isupper(static_cast<unsigned char>(variable->name.front()))&&match(TokenKind::LeftBracket)){
        if(check(TokenKind::RightBracket)) throw Error(peek().pos,"A generic type needs at least one type argument.");
        do { variable->type_args.push_back(type_ref()); } while(match(TokenKind::Comma));
        take(TokenKind::RightBracket,"Close type arguments with ']'.");
        continue;
      }
      if(match(TokenKind::LeftBracket)){auto i=expression();take(TokenKind::RightBracket,"Close this index with ']'.");arg=std::make_shared<ast::Index>(arg->pos,arg,i);continue;}
      if(match(TokenKind::Dot)){auto n=take_member_name();arg=std::make_shared<ast::Member>(arg->pos,arg,n.text);continue;}
      break;
    }
    return arg;
  };

  value=attach_members(value);
  if((std::dynamic_pointer_cast<ast::Variable>(value)||std::dynamic_pointer_cast<ast::Member>(value)) && expression_start(peek().kind)){
    std::vector<ast::ExprPtr> args;
    while(expression_start(peek().kind)){
      auto arg=attach_members(prefix());
      args.push_back(arg);
    }
    value=std::make_shared<ast::Call>(value->pos,value,std::move(args));
  }
  return value;
}

} // namespace s