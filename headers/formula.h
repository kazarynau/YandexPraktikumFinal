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

  // Возвращает вычисленное значение формулы либо ошибку
  virtual Value Evaluate(const SheetInterface& sheet) const = 0;

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

  // В функции создаем лямбду getCellValue(std::string_view str) -> double.
  // Эта лямбда будет использоваться в AST, где не обрабатываются ошибки.
  // Поэтому она возвращает double. Если же double вернуть невозможно, то
  // бросается исключение. Исключение будет отловлено в formula.Evaluate()
  // Лямбда, пользуясь sheet:
  // - получает значение путем вызова sheet.GetCell(str) -> GetValue()
  //   если значение double, возвращает double
  //   если значение string, то пытается преобразовать в double,
  //     если получилось, возвращает double
  //     если нет - бросает исключение
  //   если значение ошибка, то заново бросает исключение
  Value Evaluate(const SheetInterface& sheet) const override;

  std::string GetExpression() const override;

  // Получает ячейки из ast_
  std::vector<Position> GetReferencedCells() const override;

 private:
  double GetCellValue(std::string_view str) const = 0;

 private:
  FormulaAST ast_;
};