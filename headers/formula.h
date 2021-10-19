#pragma once

#include <memory>
#include <variant>
#include <vector>
#include "FormulaAST.h"
#include "common.h"

// Формула, позволяющая вычислять и обновлять арифметическое выражение.
// Поддерживаемые возможности:
// * Простые бинарные операции и числа, скобки: 1+2*3, 2.5*(2+3.5/7)
// * Значения ячеек в качестве переменных: A1+B2*C3
// Ячейки, указанные в формуле, могут быть как формулами, так и текстом. Если
// это текст, но он представляет число, тогда его нужно трактовать как число.
// Пустая ячейка или ячейка с пустым текстом трактуется как число ноль.
class FormulaInterface {
 public:
  using Value = std::variant<double, FormulaError>;

  virtual ~FormulaInterface() = default;

  // Обратите внимание, что в метод Evaluate() ссылка на таблицу передаётся
  // в качестве аргумента.
  // Возвращает вычисленное значение формулы для переданного листа либо ошибку.
  // Если вычисление какой-то из указанных в формуле ячеек приводит к ошибке, то
  // возвращается именно эта ошибка. Если таких ошибок несколько, возвращается
  // любая.
  virtual Value Evaluate(const SheetInterface& sheet) const = 0;

  // Возвращает выражение, которое описывает формулу.
  // Не содержит пробелов и лишних скобок.
  virtual std::string GetExpression() const = 0;

  // Возвращает список ячеек, которые непосредственно задействованы в вычислении
  // формулы. Список отсортирован по возрастанию и не содержит повторяющихся
  // ячеек.
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
  FormulaAST ast_;
};
