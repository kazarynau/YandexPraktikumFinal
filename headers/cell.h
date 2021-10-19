#pragma once

#include <functional>
#include <optional>
#include <unordered_set>
#include "common.h"
#include "formula.h"

class Sheet;

class GraphInterface {
 public:
  // Используется Position, т.к. сказано, что нельзя менять CellInterface,
  // поэтому нет смысла использовать указатели на ячейки
  virtual std::vector<Position> GetReferencedCells() const = 0;
  virtual std::vector<Position> GetDependentCells() const = 0;

  // Если ячейка не создана, создаем EmptyImpl и в нее добавляем dependent cell
  virtual void AddDependentCell(const Position pos) = 0;
  virtual void DeleteDependentCell(const Position pos) = 0;

  // https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
  // https://neerc.ifmo.ru/wiki/index.php?title=%D0%9E%D0%B1%D1%85%D0%BE%D0%B4_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83,_%D1%86%D0%B2%D0%B5%D1%82%D0%B0_%D0%B2%D0%B5%D1%80%D1%88%D0%B8%D0%BD
  virtual bool HasCircularDependency(
      std::unordered_set<CellInterface*>& visitedCells,
      const Position currentCell,
      const std::vector<Position>& referencedCells) const = 0;
};

class Cell : public CellInterface {
 public:
  Cell(Sheet& sheet);
  ~Cell();

  // - сравниваем text: если text == Impl.GetText(), выходим
  // - создаем новый Impl (старый не обновляем, пока не проверим на циклы)
  // - получаем для него referenced cells
  // - запускаем проверку HasCircularDependency()
  //   если есть, то выбрасываем CircularDependencyException и ячейку не
  //   обновляем
  // - инвалидируем кэш старого impl
  // - инвалидируем кэш у всех dependent cells старого impl (InvalidateCache)
  // - отвязываем старый impl от всех referenced cells (DeleteDependentCell)
  // - заменяем старый impl на новый
  // - для каждой referenced cell нового impl вызываем AddDependentCell
  void Set(std::string text) override;

  // - сбрасываем impl_
  void Clear();

  Value GetValue() const override;
  std::string GetText() const override;

  void InvalidateCache() const;

 private:
 private:
  class Impl;
  class EmptyImpl;
  class TextImpl;
  class FormulaImpl;

  Sheet& sheet;
  std::unique_ptr<Impl> impl_;
};

// ------------------------------------------------------------
class Cell::Impl : public GraphInterface {
 public:
  virtual CellInterface::Value GetValue(
      std::function<double(std::string_view)> getCellValueCallback) const = 0;
  virtual std::string GetText() const = 0;
  virtual ~Impl() = default;

  virtual void InvalidateCache() const = 0;
};

class Cell::FormulaImpl : public Impl {
 public:
  FormulaImpl(const SheetInterface& sheet, const std::string& text);

  void Set(const std::string& text);

  // Сначала проверяем кэш
  // Вычисляем, только если кэш пуст
  // После вычисления обновляем кэш
  CellInterface::Value GetValue(std::function<double(std::string_view)>
                                    getCellValueCallback) const override;

  std::string GetText() const override;

  std::vector<Position> GetReferencedCells() const override;
  std::vector<Position> GetDependentCells() const override;

  void AddDependentCell(const Position pos) override;
  void DeleteDependentCell(const Position pos) override;

  // Сбрасывает свой cache_
  // а также вызывает InvalidateCache для всех dependentCells_;
  void InvalidateCache() const override;

 private:
  SheetInterface& sheet_;
  std::unique_ptr<FormulaInterface> formula_;
  std::unordered_set<Position> dependentCells_;
  mutable std::optional<CellInterface::Value> cache_;
};
