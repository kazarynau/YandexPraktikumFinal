#pragma once

#include <optional>
#include <unordered_set>
#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
 public:
  Cell();
  ~Cell();

  void Set(std::string text) override;

  // - сбрасываем impl_
  void Clear();

  Value GetValue() const override;
  std::string GetText() const override;
  std::vector<Position> GetReferencedCells() const override;
  // Используется Position, т.к. сказано, что нельзя менять CellInterface,
  // поэтому нет смысла использовать указатели на ячейки
  std::vector<Position> GetDependentCells() const;

  // Если ячейка не создана, создаем EmptyImpl и в нее добавляем dependent cell
  void AddDependentCell(const Position pos);
  void DeleteDependentCell(const Position pos);
  void InvalidateCache() const;

 private:
  //можете воспользоваться нашей подсказкой, но это необязательно.
  class Impl;
  class EmptyImpl;
  class TextImpl;
  class FormulaImpl;

  std::unique_ptr<Impl> impl_;
};

// ------------------------------------------------------------
class Cell::Impl {
 public:
  virtual CellInterface::Value GetValue(
      std::function<double(std::string_view)> getCellValueCallback) const = 0;
  virtual std::string GetText() const = 0;
  virtual ~Impl() = default;

  virtual std::vector<Position> GetReferencedCells() const = 0;
  virtual std::vector<Position> GetDependentCells() const = 0;

  virtual void AddDependentCell(const Position pos) = 0;
  virtual void DeleteDependentCell(const Position pos) = 0;
  virtual void InvalidateCache() const = 0;
};

class Cell::FormulaImpl : public Impl {
 public:
  FormulaImpl(const std::string& text);

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
  std::unique_ptr<FormulaInterface> formula_;
  std::unordered_set<Position> dependentCells_;
  mutable std::optional<CellInterface::Value> cache_;
};