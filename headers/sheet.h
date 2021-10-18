#pragma once

#include <memory>
#include <vector>
#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
 public:
  ~Sheet();

  // НАДО КАК-ТО ИНАЧЕ, т.к. при вызове сета у села мы начинаем инвалидировать
  // чужие кэши и т.д.
  // - проверяем границы
  // - создаем новый cell (старый не обновляем, пока не проверим на циклы)
  // - получаем для него referenced cells
  // - запускаем проверку HasCircularDependency()
  //   если есть, то выбрасываем CircularDependencyException
  //   если нет, то обновляем ячейку (например, swap)
  void SetCell(Position pos, std::string text) override;

  const CellInterface* GetCell(Position pos) const override;
  CellInterface* GetCell(Position pos) override;

  void ClearCell(Position pos) override;

  Size GetPrintableSize() const override;

  void PrintValues(std::ostream& output) const override;
  void PrintTexts(std::ostream& output) const override;

  // Эта функция будет использоваться в AST, где не обрабатываются ошибки.
  // Поэтому она возвращает double. Если же double вернуть невозможно, то
  // бросается исключение. Исключение будет отловлено в formula.Evaluate()
  // - Преобразуем str в позицию
  // - проверяем, валидна ли позиция
  // - получаем значение
  //   если значение double, возвращаем double
  //   если значение string, то пытаемся преобразовать в double,
  //     если получилось, возвращаем double
  //     если нет - бросаем исключение
  //   если значение ошибка, то заново бросаем исключение
  double GetCellValue(std::string_view str) const override;

 private:
  // https://neerc.ifmo.ru/wiki/index.php?title=%D0%98%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%BE%D0%B1%D1%85%D0%BE%D0%B4%D0%B0_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83_%D0%B4%D0%BB%D1%8F_%D0%BF%D0%BE%D0%B8%D1%81%D0%BA%D0%B0_%D1%86%D0%B8%D0%BA%D0%BB%D0%B0
  // https://neerc.ifmo.ru/wiki/index.php?title=%D0%9E%D0%B1%D1%85%D0%BE%D0%B4_%D0%B2_%D0%B3%D0%BB%D1%83%D0%B1%D0%B8%D0%BD%D1%83,_%D1%86%D0%B2%D0%B5%D1%82%D0%B0_%D0%B2%D0%B5%D1%80%D1%88%D0%B8%D0%BD
  bool HasCircularDependency(
      const Position posToUpdate,
      const std::vector<Position>& referencedCells) const;

 private:
  std::vector<std::vector<std::unique_ptr<CellInterface>>> cells_;
};
