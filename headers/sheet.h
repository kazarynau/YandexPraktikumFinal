#pragma once

#include <memory>
#include <vector>
#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
 public:
  ~Sheet();

  // - проверяем границы
  // - вызываем cell.Set()
  void SetCell(Position pos, std::string text) override;

  const CellInterface* GetCell(Position pos) const override;
  CellInterface* GetCell(Position pos) override;

  // - инвалидируем кэш у всех dependent cells (InvalidateCache)
  // - отвязываем ячейку от всех referenced cells (DeleteDependentCell)
  void ClearCell(Position pos) override;

  Size GetPrintableSize() const override;

  void PrintValues(std::ostream& output) const override;
  void PrintTexts(std::ostream& output) const override;

 private:
  std::vector<std::vector<std::unique_ptr<Cell>>> cells_;
};
