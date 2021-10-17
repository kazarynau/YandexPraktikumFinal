#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
  Cell();
  ~Cell();

  void Set(std::string text) override;
  void Clear();

  Value GetValue() const override;
  std::string GetText() const override;

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
  virtual CellInterface::Value GetValue() const = 0;
  virtual std::string GetText() const = 0;
  virtual ~Impl() = default;
};

class Cell::FormulaImpl : public Impl {
public:
  FormulaImpl(const std::string &text);

  void Set(const std::string &text);

  CellInterface::Value GetValue() const override;

  std::string GetText() const override;

private:
  std::unique_ptr<FormulaInterface> formula_;
};