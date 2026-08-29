#include "s/checker.hpp"
#include "s/error.hpp"

namespace s {
static bool numeric(TypeKind t){return t==TypeKind::Int||t==TypeKind::Num;}
static std::string type_text(TypeKind t){static const char* n[]={"value","Int","Num","Text","Bool","List","Function","None"};return n[static_cast<int>(t)];}
TypeKind Checker::find(const std::string& n,SourcePos p) const{for(auto i=scopes_.rbegin();i!=scopes_.rend();++i)if(auto x=i->find(n);x!=i->end())return x->second;throw Error(p,"I don't know what \""+n+"\" is.","Create it first with "+n+" = value.");}
void Checker::put(const std::string& n,TypeKind t,SourcePos p){for(auto i=scopes_.rbegin();i!=scopes_.rend();++i)if(auto x=i->find(n);x!=i->end()){if(x->second!=TypeKind::Unknown&&t!=TypeKind::Unknown&&x->second!=t&&!(numeric(x->second)&&numeric(t)))throw Error(p,"'"+n+"' was "+type_text(x->second)+" before, so it cannot become "+type_text(t)+".");x->second=t;return;}scopes_.back()[n]=t;}
void Checker::check(const ast::Program&p){for(auto&s:p.statements)stmt(s);}
void Checker::block(const ast::Block&b){scopes_.push_back({});for(auto&s:b)stmt(s);scopes_.pop_back();}
TypeKind Checker::expr(const ast::ExprPtr&e){
  if(auto x=std::dynamic_pointer_cast<ast::Literal>(e))return static_cast<TypeKind>(x->value.index()+1);
  if(auto x=std::dynamic_pointer_cast<ast::Variable>(e))return find(x->name,x->pos);
  if(auto x=std::dynamic_pointer_cast<ast::List>(e)){for(auto&i:x->items)expr(i);return TypeKind::List;}
  if(auto x=std::dynamic_pointer_cast<ast::Ask>(e)){if(expr(x->question)!=TypeKind::Text)throw Error(x->pos,"ask needs Text.");return TypeKind::Text;}
  if(auto x=std::dynamic_pointer_cast<ast::Range>(e)){if(expr(x->start)!=TypeKind::Int||expr(x->end)!=TypeKind::Int)throw Error(x->pos,"A range needs two Int values.");return TypeKind::List;}
  if(auto x=std::dynamic_pointer_cast<ast::Index>(e)){if(expr(x->value)!=TypeKind::List||expr(x->index)!=TypeKind::Int)throw Error(x->pos,"List indexing needs a List and an Int.");return TypeKind::Unknown;}
  if(auto x=std::dynamic_pointer_cast<ast::Member>(e)){auto t=expr(x->value);if(x->name=="len"&&(t==TypeKind::Text||t==TypeKind::List))return TypeKind::Int;if((x->name=="upper"||x->name=="lower")&&t==TypeKind::Text)return TypeKind::Text;throw Error(x->pos,type_text(t)+" has no member named '"+x->name+"'.");}
  if(auto x=std::dynamic_pointer_cast<ast::Unary>(e)){auto t=expr(x->value);if(x->op==TokenKind::Not&&t==TypeKind::Bool)return TypeKind::Bool;if(x->op==TokenKind::Minus&&numeric(t))return t;throw Error(x->pos,"This unary operation does not work with "+type_text(t)+".");}
  if(auto x=std::dynamic_pointer_cast<ast::Binary>(e)){auto a=expr(x->left),b=expr(x->right);if(x->op==TokenKind::And||x->op==TokenKind::Or){if(a!=TypeKind::Unknown&&a!=TypeKind::Bool)throw Error(x->pos,"'and' and 'or' need Bool values.");if(b!=TypeKind::Unknown&&b!=TypeKind::Bool)throw Error(x->pos,"'and' and 'or' need Bool values.");return TypeKind::Bool;}if(x->op==TokenKind::EqualEqual||x->op==TokenKind::BangEqual)return TypeKind::Bool;if(x->op==TokenKind::Greater||x->op==TokenKind::Less||x->op==TokenKind::GreaterEqual||x->op==TokenKind::LessEqual){if((a!=TypeKind::Unknown&&!numeric(a))||(b!=TypeKind::Unknown&&!numeric(b)))throw Error(x->pos,"Comparisons need numbers.");return TypeKind::Bool;}if(x->op==TokenKind::Plus&&a==TypeKind::Text&&b==TypeKind::Text)return TypeKind::Text;if(a==TypeKind::Unknown||b==TypeKind::Unknown)return TypeKind::Unknown;if(!numeric(a)||!numeric(b))throw Error(x->pos,"This math operation cannot combine "+type_text(a)+" and "+type_text(b)+".");return a==TypeKind::Num||b==TypeKind::Num||x->op==TokenKind::Slash||x->op==TokenKind::Power?TypeKind::Num:TypeKind::Int;}
  if(auto x=std::dynamic_pointer_cast<ast::Call>(e)){if(expr(x->callee)!=TypeKind::Function)throw Error(x->pos,"Only a function can be called.");for(auto&a:x->args)expr(a);return TypeKind::Unknown;}
  return TypeKind::Unknown;
}
void Checker::stmt(const ast::StmtPtr&s){
  if(auto x=std::dynamic_pointer_cast<ast::Say>(s)){expr(x->value);return;}if(auto x=std::dynamic_pointer_cast<ast::ExprStmt>(s)){expr(x->value);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Assign>(s)){auto t=expr(x->value);if(auto n=std::dynamic_pointer_cast<ast::Variable>(x->target)){put(n->name,t,x->pos);return;}if(auto i=std::dynamic_pointer_cast<ast::Index>(x->target)){expr(i);return;}throw Error(x->pos,"You can only assign to a name or list item.");}
  if(auto x=std::dynamic_pointer_cast<ast::If>(s)){auto t=expr(x->condition);if(t!=TypeKind::Bool&&t!=TypeKind::Unknown)throw Error(x->pos,"if needs a Bool condition.");block(x->then_block);block(x->else_block);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Repeat>(s)){if(expr(x->count)!=TypeKind::Int)throw Error(x->pos,"repeat needs an Int count.");block(x->body);return;}
  if(auto x=std::dynamic_pointer_cast<ast::For>(s)){if(expr(x->values)!=TypeKind::List)throw Error(x->pos,"for needs a List or range after 'in'.");scopes_.push_back({{x->name,TypeKind::Unknown}});for(auto&i:x->body)stmt(i);scopes_.pop_back();return;}
  if(auto x=std::dynamic_pointer_cast<ast::While>(s)){auto t=expr(x->condition);if(t!=TypeKind::Bool&&t!=TypeKind::Unknown)throw Error(x->pos,"while needs a Bool condition.");block(x->body);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Function>(s)){put(x->name,TypeKind::Function,x->pos);scopes_.push_back({});for(auto&p:x->params)scopes_.back()[p]=TypeKind::Unknown;++function_depth_;for(auto&i:x->body)stmt(i);--function_depth_;scopes_.pop_back();return;}
  if(auto x=std::dynamic_pointer_cast<ast::Give>(s)){if(!function_depth_)throw Error(x->pos,"give can only be used inside a function.");expr(x->value);return;}
}
} // namespace s
