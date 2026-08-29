#include "s/interpreter.hpp"
#include <algorithm>
#include <cctype>

namespace s {
namespace { struct ReturnSignal{Value value;}; }
Interpreter::Interpreter(std::istream& i,std::ostream& o):in_(i),out_(o),env_(std::make_shared<Environment>()){}
void Interpreter::run(const ast::Program& p){for(auto&s:p.statements)execute(s);}
void Interpreter::execute_block(const ast::Block& b,std::shared_ptr<Environment> e){auto old=env_;env_=std::move(e);try{for(auto&s:b)execute(s);}catch(...){env_=old;throw;}env_=old;}

Value Interpreter::evaluate(const ast::ExprPtr& e){
  if(auto x=std::dynamic_pointer_cast<ast::Literal>(e)){return std::visit([](auto v){return Value(v);},x->value);}
  if(auto x=std::dynamic_pointer_cast<ast::Variable>(e))return env_->get(x->name,x->pos);
  if(auto x=std::dynamic_pointer_cast<ast::Unary>(e)){auto v=evaluate(x->value);if(x->op==TokenKind::Not)return !v.truth(x->pos);if(auto n=std::get_if<std::int64_t>(&v.data()))return -*n;if(auto n=std::get_if<double>(&v.data()))return -*n;throw Error(x->pos,"Only a number can follow '-'.");}
  if(auto x=std::dynamic_pointer_cast<ast::Binary>(e)){auto a=evaluate(x->left);if(x->op==TokenKind::And&&!a.truth(x->pos))return false;if(x->op==TokenKind::Or&&a.truth(x->pos))return true;return binary(x->op,a,evaluate(x->right),x->pos);}
  if(auto x=std::dynamic_pointer_cast<ast::List>(e)){auto l=std::make_shared<ListData>();for(auto&i:x->items)l->items.push_back(evaluate(i));return l;}
  if(auto x=std::dynamic_pointer_cast<ast::Range>(e)){auto a=evaluate(x->start),b=evaluate(x->end);if(!std::holds_alternative<std::int64_t>(a.data())||!std::holds_alternative<std::int64_t>(b.data()))throw Error(x->pos,"A range needs two Int values.");auto l=std::make_shared<ListData>();auto from=std::get<std::int64_t>(a.data()),to=std::get<std::int64_t>(b.data());auto step=from<=to?1:-1;for(auto n=from;;n+=step){l->items.emplace_back(n);if(n==to)break;}return l;}
  if(auto x=std::dynamic_pointer_cast<ast::Index>(e)){auto v=evaluate(x->value),i=evaluate(x->index);if(!std::holds_alternative<std::int64_t>(i.data()))throw Error(x->pos,"A list index must be Int.");auto n=std::get<std::int64_t>(i.data());if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data())){if(n<0||static_cast<std::size_t>(n)>=(*l)->items.size())throw Error(x->pos,"List index "+std::to_string(n)+" is out of bounds. This list has "+std::to_string((*l)->items.size())+" items.");return (*l)->items[n];}throw Error(x->pos,"Only a List can use [index].");}
  if(auto x=std::dynamic_pointer_cast<ast::Member>(e)){auto v=evaluate(x->value);if(x->name=="len"){if(auto t=std::get_if<std::string>(&v.data()))return static_cast<std::int64_t>(t->size());if(auto l=std::get_if<std::shared_ptr<ListData>>(&v.data()))return static_cast<std::int64_t>((*l)->items.size());}if((x->name=="upper"||x->name=="lower")&&std::holds_alternative<std::string>(v.data())){auto t=std::get<std::string>(v.data());std::transform(t.begin(),t.end(),t.begin(),x->name=="upper"?static_cast<int(*)(int)>(std::toupper):static_cast<int(*)(int)>(std::tolower));return t;}throw Error(x->pos,v.type_name()+" has no member named '"+x->name+"'.");}
  if(auto x=std::dynamic_pointer_cast<ast::Ask>(e)){auto q=evaluate(x->question);if(!std::holds_alternative<std::string>(q.data()))throw Error(x->pos,"ask needs Text.");out_<<q.text()<<": ";std::string answer;std::getline(in_,answer);return answer;}
  if(auto x=std::dynamic_pointer_cast<ast::Call>(e)){auto c=evaluate(x->callee);auto f=std::get_if<std::shared_ptr<FunctionData>>(&c.data());if(!f)throw Error(x->pos,"Only a function can be called.");if(x->args.size()!=(*f)->params.size())throw Error(x->pos,"This function needs "+std::to_string((*f)->params.size())+" values, but got "+std::to_string(x->args.size())+".");auto local=std::make_shared<Environment>((*f)->closure);for(std::size_t i=0;i<x->args.size();++i)local->define((*f)->params[i],evaluate(x->args[i]));try{execute_block((*f)->body,local);}catch(ReturnSignal&r){return r.value;}return {};}
  throw Error(e->pos,"This expression is not implemented.");
}

void Interpreter::execute(const ast::StmtPtr& s){
  if(auto x=std::dynamic_pointer_cast<ast::Say>(s)){out_<<evaluate(x->value).text()<<'\n';return;}
  if(auto x=std::dynamic_pointer_cast<ast::ExprStmt>(s)){evaluate(x->value);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Assign>(s)){auto v=evaluate(x->value);if(auto n=std::dynamic_pointer_cast<ast::Variable>(x->target)){env_->set(n->name,v);return;}if(auto i=std::dynamic_pointer_cast<ast::Index>(x->target)){auto list=evaluate(i->value),index=evaluate(i->index);auto lp=std::get_if<std::shared_ptr<ListData>>(&list.data());if(!lp||!std::holds_alternative<std::int64_t>(index.data()))throw Error(x->pos,"This assignment needs a List and an Int index.");auto n=std::get<std::int64_t>(index.data());if(n<0||static_cast<std::size_t>(n)>=(*lp)->items.size())throw Error(x->pos,"List index is out of bounds.");(*lp)->items[n]=v;return;}throw Error(x->pos,"You can only assign to a name or list item.");}
  if(auto x=std::dynamic_pointer_cast<ast::If>(s)){execute_block(x->condition? (evaluate(x->condition).truth(x->pos)?x->then_block:x->else_block):x->else_block,std::make_shared<Environment>(env_));return;}
  if(auto x=std::dynamic_pointer_cast<ast::Repeat>(s)){auto n=evaluate(x->count);if(!std::holds_alternative<std::int64_t>(n.data()))throw Error(x->pos,"repeat needs an Int count.");auto count=std::get<std::int64_t>(n.data());if(count<0)throw Error(x->pos,"repeat count cannot be negative.");for(std::int64_t i=0;i<count;++i)execute_block(x->body,std::make_shared<Environment>(env_));return;}
  if(auto x=std::dynamic_pointer_cast<ast::For>(s)){auto v=evaluate(x->values);auto l=std::get_if<std::shared_ptr<ListData>>(&v.data());if(!l)throw Error(x->pos,"for needs a List or range after 'in'.");for(auto&item:(*l)->items){auto local=std::make_shared<Environment>(env_);local->define(x->name,item);execute_block(x->body,local);}return;}
  if(auto x=std::dynamic_pointer_cast<ast::While>(s)){while(evaluate(x->condition).truth(x->pos)){if(++loop_steps_>10000000)throw Error(x->pos,"This loop ran too long.","Check that its condition can become false.");execute_block(x->body,std::make_shared<Environment>(env_));}return;}
  if(auto x=std::dynamic_pointer_cast<ast::Function>(s)){auto f=std::make_shared<FunctionData>();f->params=x->params;f->body=x->body;f->closure=env_;env_->set(x->name,f);return;}
  if(auto x=std::dynamic_pointer_cast<ast::Give>(s))throw ReturnSignal{evaluate(x->value)};
  throw Error(s->pos,"This statement is not implemented.");
}
} // namespace s
