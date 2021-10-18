#pragma once

#include <optional>
#include <unordered_set>
#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
 public:
  Cell();
  ~Cell();

  // - выясняем, что надо создавать: текст или формулу
  // - инвалидируем свой кэш
  // - инвалидируем кэш у всех dependent cells (InvalidateCache)
  // - отвязываемся от всех referenced cells (DeleteDependentCell)
  // - создаем необходимый impl
  // - получаем referenced cells
  // - для каждой referenced cell вызываем AddDependentCell, в качестве
  // аргумента передаем себя
  void Set(std::string text) override;

  // - инвалидируем кэш у всех dependent cells (InvalidateCache)
  // - отвязываемся от всех referenced cells (DeleteDependentCell)
  // - сбрасываем impl_
  void Clear();

  Value GetValue() const override;
  std::string GetText() const override;
  std::vector<Position> GetReferencedCells() const override;

  // Если ячейка не создана, создаем EmptyImpl и в нее добавляем dependent cell
  void AddDependentCell(const CellInterface* dependent);
  void DeleteDependentCell(const CellInterface* dependent);
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
      std::function<FormulaInterface::Value(std::string_view)>
          getCellValueCallback) const = 0;
  virtual std::string GetText() const = 0;
  virtual ~Impl() = default;

  virtual std::vector<Position> GetReferencedCells() const = 0;
  virtual void AddDependentCell(const CellInterface* dependent) = 0;
  virtual void DeleteDependentCell(const CellInterface* dependent) = 0;
  virtual void InvalidateCache() const = 0;
};

class Cell::FormulaImpl : public Impl {
 public:
  FormulaImpl(const std::string& text);

  void Set(const std::string& text);

  // Сначала проверяем кэш
  // Вычисляем, только если кэш пуст
  // После вычисления обновляем кэш
  CellInterface::Value GetValue(
      std::function<FormulaInterface::Value(std::string_view)>
          getCellValueCallback) const override;

  std::string GetText() const override;

  std::vector<Position> GetReferencedCells() const override;

  void AddDependentCell(const CellInterface* dependent) override;
  void DeleteDependentCell(const CellInterface* dependent) override;

  // Сбрасывает свой cache_
  // а также вызывает InvalidateCache для всех dependentCells_;
  void InvalidateCache() const override;

 private:
  std::unique_ptr<FormulaInterface> formula_;
  std::unordered_set<CellInterface*> dependentCells_;
  mutable std::optional<CellInterface::Value> cache_;
};