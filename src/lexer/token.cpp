#include "s/token.hpp"

namespace s {
std::string_view token_name(TokenKind kind) {
  switch (kind) {
#define S_TOKEN(x) case TokenKind::x: return #x
    S_TOKEN(End); S_TOKEN(Newline); S_TOKEN(Indent); S_TOKEN(Dedent);
    S_TOKEN(Identifier); S_TOKEN(Integer); S_TOKEN(Number); S_TOKEN(String); S_TOKEN(Duration);
    S_TOKEN(True); S_TOKEN(False); S_TOKEN(Say); S_TOKEN(Ask); S_TOKEN(If);
    S_TOKEN(Else); S_TOKEN(Repeat); S_TOKEN(For); S_TOKEN(In); S_TOKEN(While);
    S_TOKEN(Make); S_TOKEN(Give); S_TOKEN(Type); S_TOKEN(Use); S_TOKEN(Try); S_TOKEN(Fail);
    S_TOKEN(Set); S_TOKEN(Map); S_TOKEN(And); S_TOKEN(Or); S_TOKEN(Not);
    S_TOKEN(Plus); S_TOKEN(Minus); S_TOKEN(Star); S_TOKEN(Slash); S_TOKEN(Percent);
    S_TOKEN(Power); S_TOKEN(Equal); S_TOKEN(EqualEqual); S_TOKEN(BangEqual);
    S_TOKEN(Greater); S_TOKEN(Less); S_TOKEN(GreaterEqual); S_TOKEN(LessEqual);
    S_TOKEN(LeftParen); S_TOKEN(RightParen); S_TOKEN(LeftBracket);
    S_TOKEN(RightBracket); S_TOKEN(Comma); S_TOKEN(Colon); S_TOKEN(Dot); S_TOKEN(Range);
#undef S_TOKEN
  }
  return "unknown";
}
}
