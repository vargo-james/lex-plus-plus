#include "context_test.h"
#include "regex/context.h"

#include <utility>
#include <vector>

using namespace lex;

bool compare(context& c,
    const std::vector<std::pair<token_type,context::site>>& test) {
  for (auto p: test) {
    c.update(p.first);
    if (c.get() != p.second) return false;
  }
  return true;
}

void context_switching_test(ttest::error_log& log) {
  context c;  

  if (c.get() != context::DEFAULT) {
    log.append("default constructor");
  }

 c.update(token_type::L_PAREN);
 if (c.get() != context::SUBEXPR) {
   log.append("subexp");
 }
 c.update(token_type::LITERAL);
 if (c.get() != context::DEFAULT) {
   log.append("default context");
 }
 c.update(token_type::R_PAREN);
 if (c.get() != context::DEFAULT) {
   log.append("simple test");
 }

  bool comp1 = compare(c, {
        {token_type::L_PAREN,context::SUBEXPR},
        {token_type::LITERAL,context::DEFAULT},
        {token_type::R_PAREN,context::DEFAULT},
        {token_type::L_BRACKET,context::BRACKET},
        {token_type::L_COLLATE,context::COLLATE},
        {token_type::ASSERTION,context::COLLATE},
        {token_type::BACK_REF,context::COLLATE},
        {token_type::LITERAL,context::COLLATE},
        {token_type::R_PAREN,context::COLLATE},
        {token_type::R_COLLATE,context::BRACKET},
        {token_type::R_PAREN,context::BRACKET},
        {token_type::LITERAL,context::BRACKET},
        {token_type::R_BRACKET,context::DEFAULT},
      });

  if (!comp1) {
    log.append("comp1");
  }

  if (c.depth() != 0) {
    log.append("unbalanced parens");
  }

  c.update(token_type::L_PAREN);
  if (c.get() != context::SUBEXPR) {
    log.append("subexpr entrance");
  }
  c.update(token_type::L_BRACKET);
  if (c.get() != context::BRACKET) {
    log.append("bracket entrance");
  }
  c.update(token_type::L_COLLATE);
  if (c.get() != context::COLLATE) {
    log.append("collate entrance");
  }
  if (c.depth() != 1) {
    log.append("incorrect depth");
  }
}

bool depth_test(context& c, 
    const std::vector<std::pair<token_type,int>>& test) {
  int net_depth {c.depth()};
  for (auto p: test) {
    c.update(p.first);
    net_depth += p.second;
    if (c.depth() != net_depth) return false;
  }
  return true;
}

void context_depth_test(ttest::error_log& log) {
  context c;

  bool depth_comp = depth_test(c, {
        {token_type::L_PAREN, 1},
        {token_type::LITERAL, 0},
        {token_type::R_PAREN, -1},
        {token_type::L_BRACKET, 0},
        {token_type::R_PAREN, 0},
        {token_type::R_BRACKET, 0},
        {token_type::L_PAREN, 1},
        {token_type::L_PAREN, 1},
        {token_type::L_PAREN, 1},
        {token_type::R_PAREN, -1},
        {token_type::R_PAREN, -1},
        {token_type::R_PAREN, -1},
        {token_type::R_PAREN, -1},
        {token_type::R_PAREN, -1},
        {token_type::R_PAREN, -1},
        {token_type::R_PAREN, -1},
        {token_type::L_BRACE, 0},
        {token_type::L_PAREN, 0},
        {token_type::R_PAREN, 0},
        {token_type::R_BRACE, 0},
        {token_type::L_PAREN, 1},
      });
  if (!depth_comp) {
    log.append("depth");
  }
}

void bracket_context_test(ttest::error_log& log) {
  context c;
  c.update(token_type::L_BRACKET);
  if (!c.first_bracket_char()) {
    log.append("first");
  }
  if (c.after_bracket_negation()) {
    log.append("after negation");
  }
  c.update(token_type::NEGATION);
  if (c.first_bracket_char()) {
    log.append("not first");
  }
  if (!c.after_bracket_negation()) {
    log.append("not after negation");
  }
  c.update(token_type::NEGATION);
  if (c.first_bracket_char()) {
    log.append("not first 2");
  }
  if (!c.after_bracket_negation()) {
    log.append("not after negation 2");
  }
  c.update(token_type::RANGE_DASH);
  if (c.first_bracket_char()) {
    log.append("not first 3");
  }
  if (c.after_bracket_negation()) {
    log.append("after negation 2");
  }
  c.update(token_type::R_BRACKET);
  c.update(token_type::L_BRACKET);
  if (!c.first_bracket_char()) {
    log.append("first 2");
  }
  c.update(token_type::R_BRACKET);
  if (c.get() != context::DEFAULT) {
    log.append("bracket closure");
  }

  if (c.first_bracket_char() || c.after_bracket_negation()) {
    log.append("bracket cleanup");
  }

  context d;
  d.update(token_type::L_BRACKET);
  d.update(token_type::NEGATION);
  if (!d.after_bracket_negation()) {
    log.append("after negation");
  }
  d.update(token_type::R_BRACKET);
  if (d.after_bracket_negation()) {
    log.append("cleanup 2");
  }

  

}
