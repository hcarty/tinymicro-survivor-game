#pragma once

#include <string>
#include "Object.h"

class GameMenu : public Object
{
public:
protected:
  void OnCreate();
  void OnDelete();
  void Update(const orxCLOCK_INFO &_rstInfo);

private:
  std::string inputSet{""};
};
