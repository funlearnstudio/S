#pragma once

#include "s/token.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace s::ast {

struct Expr { explicit Expr(SourcePos p) : pos(p) {} virtual ~Expr()=default; SourcePos pos; };
using ExprPtr = std::shared_ptr<Expr>;
struct Stmt { explicit Stmt(SourcePos p) : pos(p) {} virtual ~Stmt()=default; SourcePos pos; };
using StmtPtr = std::shared_ptr<Stmt>;
using Block = std::vector<StmtPtr>;

struct Literal final : Expr {
  using Data=std::variant<std::int64_t,double,std::string,bool>;
  Literal(SourcePos p,Data v):Expr(p),value(std::move(v)){} Data value;
};
struct Variable final : Expr { Variable(SourcePos p,std::string n):Expr(p),name(std::move(n)){} std::string name; };
struct Binary final : Expr { Binary(SourcePos p,ExprPtr l,TokenKind o,ExprPtr r):Expr(p),left(std::move(l)),op(o),right(std::move(r)){} ExprPtr left; TokenKind op; ExprPtr right; };
struct Unary final : Expr { Unary(SourcePos p,TokenKind o,ExprPtr v):Expr(p),op(o),value(std::move(v)){} TokenKind op; ExprPtr value; };
struct List final : Expr { List(SourcePos p,std::vector<ExprPtr> v):Expr(p),items(std::move(v)){} std::vector<ExprPtr> items; };
struct Index final : Expr { Index(SourcePos p,ExprPtr v,ExprPtr i):Expr(p),value(std::move(v)),index(std::move(i)){} ExprPtr value,index; };
struct Member final : Expr { Member(SourcePos p,ExprPtr v,std::string n):Expr(p),value(std::move(v)),name(std::move(n)){} ExprPtr value; std::string name; };
struct Call final : Expr { Call(SourcePos p,ExprPtr c,std::vector<ExprPtr> a):Expr(p),callee(std::move(c)),args(std::move(a)){} ExprPtr callee; std::vector<ExprPtr> args; };
struct Ask final : Expr { Ask(SourcePos p,ExprPtr q):Expr(p),question(std::move(q)){} ExprPtr question; };
struct Range final : Expr { Range(SourcePos p,ExprPtr a,ExprPtr b):Expr(p),start(std::move(a)),end(std::move(b)){} ExprPtr start,end; };

struct ExprStmt final : Stmt { ExprStmt(SourcePos p,ExprPtr v):Stmt(p),value(std::move(v)){} ExprPtr value; };
struct Assign final : Stmt { Assign(SourcePos p,ExprPtr t,ExprPtr v):Stmt(p),target(std::move(t)),value(std::move(v)){} ExprPtr target,value; };
struct Say final : Stmt { Say(SourcePos p,ExprPtr v):Stmt(p),value(std::move(v)){} ExprPtr value; };
struct If final : Stmt { If(SourcePos p,ExprPtr c,Block t,Block e):Stmt(p),condition(std::move(c)),then_block(std::move(t)),else_block(std::move(e)){} ExprPtr condition; Block then_block,else_block; };
struct Repeat final : Stmt { Repeat(SourcePos p,ExprPtr c,Block b):Stmt(p),count(std::move(c)),body(std::move(b)){} ExprPtr count; Block body; };
struct For final : Stmt { For(SourcePos p,std::string n,ExprPtr v,Block b):Stmt(p),name(std::move(n)),values(std::move(v)),body(std::move(b)){} std::string name; ExprPtr values; Block body; };
struct While final : Stmt { While(SourcePos p,ExprPtr c,Block b):Stmt(p),condition(std::move(c)),body(std::move(b)){} ExprPtr condition; Block body; };
struct Function final : Stmt { Function(SourcePos p,std::string n,std::vector<std::string> a,Block b):Stmt(p),name(std::move(n)),params(std::move(a)),body(std::move(b)){} std::string name; std::vector<std::string> params; Block body; };
struct Give final : Stmt { Give(SourcePos p,ExprPtr v):Stmt(p),value(std::move(v)){} ExprPtr value; };
// Reserved 0.2 AST shape. Keeping it in the common AST prevents an object system
// from requiring a parser/interpreter rewrite when its final surface syntax lands.
struct Type final : Stmt { Type(SourcePos p,std::string n,Block b):Stmt(p),name(std::move(n)),body(std::move(b)){} std::string name; Block body; };
struct Program { Block statements; };

} // namespace s::ast
