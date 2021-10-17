#pragma once

#include "FormulaAST.h"
#include "common.h"
#include <memory>
#include <variant>

// Формула, позволяющая вычислять и обновлять арифметическое выражение.
// Поддерживаемые возможности:
// * Простые бинарные операции и числа, скобки: 1+2*3, 2.5*(2+3.5/7)
class FormulaInterface {
public:
  using Value = std::variant<double, FormulaError>;

  virtual ~FormulaInterface() = default;

  // Возвращает вычисленное значение формулы либо ошибку. На данном этапе
  // мы создали только 1 вид ошибки -- деление на 0.
  virtual Value Evaluate() const = 0;

  // Возвращает выражение, которое описывает формулу.
  // Не содержит пробелов и лишних скобок.
  virtual std::string GetExpression() const = 0;
};

// Парсит переданное выражение и возвращает объект формулы.
// Бросает FormulaException в случае, если формула синтаксически некорректна.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);

// -------------------------------------------------------------------------
// class Formula : public FormulaInterface {
//  public:
//   // Реализуйте следующие методы:
//   explicit Formula(std::string expression)
//       : ast_(ParseFormulaAST(expression)) {}

//   Value Evaluate() const override {
//     try {
//       return ast_.Execute();
//     } catch (const FormulaError& error) {
//       return error;
//     }
//     return FormulaError("");
//   }

//   std::string GetExpression() const override {
//     std::stringstream ss;
//     ast_.PrintFormula(ss);
//     return ss.str();
//   }

//  private:
//   FormulaAST makeFormulaAst(const std::string& expression) {
//     try {
//       return ParseFormulaAST(expression);
//     } catch (...) {
//     }
//     throw FormulaException("");
//   }

//  private:
//   FormulaAST ast_;
// };