#pragma once

#include "s/ast.hpp"
#include <vector>

namespace s {
class Parser {
public:
  explicit Parser(std::vector<Token> tokens):tokens_(std::move(tokens)){}
  ast::Program parse();
private:
  std::vector<Token> tokens_; std::size_t at_=0;
  const Token& peek(int offset=0) const;
  bool check(TokenKind) const; bool match(TokenKind); const Token& take(TokenKind,const std::string&);
  void line_end(); ast::StmtPtr statement(); ast::Block block();
  std::shared_ptr<ast::Function> function(SourcePos start);
  std::shared_ptr<ast::Type> type_decl(SourcePos start);
  ast::TypeRef type_ref();
  ast::ExprPtr expression(int min_prec=0); ast::ExprPtr prefix(); ast::ExprPtr postfix(ast::ExprPtr);
  bool expression_start(TokenKind) const; int precedence(TokenKind) const;
  std::vector<ast::ExprPtr> bracket_items(TokenKind close);
};
} // namespace s