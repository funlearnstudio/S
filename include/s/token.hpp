#pragma once

#include <string>
#include <string_view>

namespace s {

struct SourcePos { int line = 1; int column = 1; };

enum class TokenKind {
  End, Newline, Indent, Dedent,
  Identifier, Integer, Number, String, Duration,
  True, False, Say, Ask, If, Else, Repeat, For, In, While, Make, Give,
  Type, Use, Try, Fail, Set, Map, Match, Case,
  And, Or, Not,
  Plus, Minus, Star, Slash, Percent, Power,
  Equal, PlusEqual, MinusEqual, StarEqual, SlashEqual, PercentEqual,
  EqualEqual, BangEqual, Greater, Less, GreaterEqual, LessEqual,
  LeftParen, RightParen, LeftBracket, RightBracket, Comma, Colon, Dot, Range, Arrow
};

struct Token {
  TokenKind kind;
  std::string text;
  SourcePos pos;
};

std::string_view token_name(TokenKind kind);

} // namespace s
