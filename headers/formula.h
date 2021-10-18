#pragma once

#include <memory>
#include <variant>
#include "FormulaAST.h"
#include "common.h"

// Формула, позволяющая вычислять и обновлять арифметическое выражение.
// Поддерживаемые возможности:
// * Простые бинарные операции и числа, скобки: 1+2*3, 2.5*(2+3.5/7)
class FormulaInterface {
 public:
  using Value = std::variant<double, FormulaError>;

  virtual ~FormulaInterface() = default;

  // Возвращает вычисленное значение формулы либо ошибку. На данном этапе
  // мы создали только 1 вид ошибки -- деление на 0.
  virtual Value Evaluate(
      std::function<FormulaInterface::Value(std::string_view)>
          getCellValueCallback) const = 0;

  // Возвращает выражение, которое описывает формулу.
  // Не содержит пробелов и лишних скобок.
  virtual std::string GetExpression() const = 0;

  virtual std::vector<Position> GetReferencedCells() const = 0;
};

// Парсит переданное выражение и возвращает объект формулы.
// Бросает FormulaException в случае, если формула синтаксически некорректна.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);

// -------------------------------------------------------------------------
class Formula : public FormulaInterface {
 public:
  // Реализуйте следующие методы:
  explicit Formula(std::string expression)
      : ast_(ParseFormulaAST(expression)) {}

  Value Evaluate(std::function<FormulaInterface::Value(std::string_view)>
                     getCellValueCallback) const override;

  std::string GetExpression() const override;

  // Получает ячейки из ast_
  std::vector<Position> GetReferencedCells() const override;

 private:
  FormulaAST ast_;
};