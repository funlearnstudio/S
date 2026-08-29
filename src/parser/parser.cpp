#include "s/parser.hpp"
#include "s/error.hpp"
#include <charconv>

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

ast::StmtPtr Parser::statement(){
  Token start=peek();
  if(match(TokenKind::Say)){ auto e=expression(); line_end(); return std::make_shared<ast::Say>(start.pos,e); }
  if(match(TokenKind::If)){
    auto c=expression(); auto yes=block(); ast::Block no;
    if(match(TokenKind::Else)) no=block();
    return std::make_shared<ast::If>(start.pos,c,std::move(yes),std::move(no));
  }
  if(match(TokenKind::Repeat)){ auto n=expression(); auto b=block(); return std::make_shared<ast::Repeat>(start.pos,n,std::move(b)); }
  if(match(TokenKind::For)){
    auto name=take(TokenKind::Identifier,"Give the loop value a name after 'for'.");
    take(TokenKind::In,"Use 'in' before the values to loop over."); auto values=expression(); auto b=block();
    return std::make_shared<ast::For>(start.pos,name.text,values,std::move(b));
  }
  if(match(TokenKind::While)){ auto c=expression(); auto b=block(); return std::make_shared<ast::While>(start.pos,c,std::move(b)); }
  if(match(TokenKind::Make)){
    auto name=take(TokenKind::Identifier,"Give this function a name."); std::vector<std::string> params;
    while(check(TokenKind::Identifier)) params.push_back(tokens_[at_++].text);
    auto b=block(); return std::make_shared<ast::Function>(start.pos,name.text,std::move(params),std::move(b));
  }
  if(match(TokenKind::Give)){ auto e=expression(); line_end(); return std::make_shared<ast::Give>(start.pos,e); }
  auto left=expression();
  if(match(TokenKind::Equal)){ auto value=expression(); line_end(); return std::make_shared<ast::Assign>(start.pos,left,value); }
  line_end(); return std::make_shared<ast::ExprStmt>(start.pos,left);
}

int Parser::precedence(TokenKind k) const {
  switch(k){
    case TokenKind::Or:return 1; case TokenKind::And:return 2;
    case TokenKind::EqualEqual:case TokenKind::BangEqual:return 3;
    case TokenKind::Greater:case TokenKind::Less:case TokenKind::GreaterEqual:case TokenKind::LessEqual:return 4;
    case TokenKind::Range:return 5; case TokenKind::Plus:case TokenKind::Minus:return 6;
    case TokenKind::Star:case TokenKind::Slash:case TokenKind::Percent:return 7;
    case TokenKind::Power:return 8; default:return -1;
  }
}
bool Parser::expression_start(TokenKind k) const { return k==TokenKind::Identifier||k==TokenKind::Integer||k==TokenKind::Number||k==TokenKind::String||k==TokenKind::True||k==TokenKind::False||k==TokenKind::LeftParen||k==TokenKind::LeftBracket||k==TokenKind::Ask; }

ast::ExprPtr Parser::expression(int min_prec){
  auto left=postfix(prefix());
  while(true){ int p=precedence(peek().kind); if(p<min_prec) break; Token op=tokens_[at_++]; auto right=expression(p+(op.kind==TokenKind::Power?0:1));
    if(op.kind==TokenKind::Range) left=std::make_shared<ast::Range>(op.pos,left,right); else left=std::make_shared<ast::Binary>(op.pos,left,op.kind,right);
  }
  return left;
}

ast::ExprPtr Parser::prefix(){
  Token t=peek();
  if(match(TokenKind::Integer)) return std::make_shared<ast::Literal>(t.pos,static_cast<std::int64_t>(std::stoll(t.text)));
  if(match(TokenKind::Number)) return std::make_shared<ast::Literal>(t.pos,std::stod(t.text));
  if(match(TokenKind::String)) return std::make_shared<ast::Literal>(t.pos,t.text);
  if(match(TokenKind::True)||match(TokenKind::False)) return std::make_shared<ast::Literal>(t.pos,t.kind==TokenKind::True);
  if(match(TokenKind::Identifier)) return std::make_shared<ast::Variable>(t.pos,t.text);
  if(match(TokenKind::Minus)||match(TokenKind::Not)) return std::make_shared<ast::Unary>(t.pos,t.kind,expression(8));
  if(match(TokenKind::Ask)) return std::make_shared<ast::Ask>(t.pos,expression(9));
  if(match(TokenKind::LeftParen)){ auto e=expression(); take(TokenKind::RightParen,"Close this expression with ')'."); return e; }
  if(match(TokenKind::LeftBracket)){
    std::vector<ast::ExprPtr> items;
    if(!check(TokenKind::RightBracket)) do { items.push_back(expression()); } while(match(TokenKind::Comma));
    take(TokenKind::RightBracket,"Close this list with ']'."); return std::make_shared<ast::List>(t.pos,std::move(items));
  }
  throw Error(t.pos,"I expected a value here.");
}

ast::ExprPtr Parser::postfix(ast::ExprPtr value){
  while(true){
    if(match(TokenKind::LeftBracket)){ auto i=expression(); take(TokenKind::RightBracket,"Close this index with ']'."); value=std::make_shared<ast::Index>(value->pos,value,i); continue; }
    if(match(TokenKind::Dot)){ auto n=take(TokenKind::Identifier,"Write a member name after '.'."); value=std::make_shared<ast::Member>(value->pos,value,n.text); continue; }
    break;
  }
  // Low-punctuation calls: only a named function followed by space-separated atomic arguments.
  if(std::dynamic_pointer_cast<ast::Variable>(value) && expression_start(peek().kind)){
    std::vector<ast::ExprPtr> args;
    while(expression_start(peek().kind)){ auto arg=postfix(prefix()); args.push_back(arg); }
    value=std::make_shared<ast::Call>(value->pos,value,std::move(args));
  }
  return value;
}

} // namespace s
