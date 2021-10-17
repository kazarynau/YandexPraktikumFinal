#pragma once

#include <memory>
#include <vector>
#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
 public:
  ~Sheet();

  void SetCell(Position pos, std::string text) override;

  const CellInterface* GetCell(Position pos) const override;
  CellInterface* GetCell(Position pos) override;

  void ClearCell(Position pos) override;

  Size GetPrintableSize() const override;

  void PrintValues(std::ostream& output) const override;
  void PrintTexts(std::ostream& output) const override;

  // - Преобразуем str в позицию
  // - проверяем, валидна ли позиция
  // - получаем значение
  //   если значение double, возвращаем double
  //   если значение string, то пытаемся преобразовать в double,
  //     если получилось, возвращаем double
  //     если нет - ошибку
  //   если значение ошибка, то возвращаем ошибку
  FormulaInterface::Value GetCellValue(std::string_view str) const override;

 private:
  std::vector<std::vector<std::unique_ptr<CellInterface>>> cells_;
};
