#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {
class Expr;
}

class ParsingError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class FormulaAST {
 public:
  explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr);
  FormulaAST(FormulaAST&&) = default;
  FormulaAST& operator=(FormulaAST&&) = default;
  ~FormulaAST();

  double Execute(
      std::function<double(std::string_view)> getCellValueCallback) const;
  void PrintCells(std::ostream& out) const;
  void Print(std::ostream& out) const;
  void PrintFormula(std::ostream& out) const;

  std::forward_list<Position>& GetCells() { return cells_; }

  const std::forward_list<Position>& GetCells() const { return cells_; }

 private:
  std::unique_ptr<ASTImpl::Expr> root_expr_;

  // physically stores cells so that they can be
  // efficiently traversed without going through
  // the whole AST
  std::forward_list<Position> cells_;
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);

// -----------------------------------------------------------
class Expr {
 public:
  virtual ~Expr() = default;
  virtual void Print(std::ostream& out) const = 0;
  virtual void DoPrintFormula(std::ostream& out,
                              ExprPrecedence precedence) const = 0;
  virtual double Evaluate(
      std::function<double(std::string_view)> getCellValueCallback) const = 0;

  // higher is tighter
  virtual ExprPrecedence GetPrecedence() const = 0;

  void PrintFormula(std::ostream& out,
                    ExprPrecedence parent_precedence,
                    bool right_child = false) const;
};