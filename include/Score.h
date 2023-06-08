#pragma once

#include "Object.h"

class Score : public Object
{
public:
protected:
  void OnCreate();
  void OnDelete();
  void Update(const orxCLOCK_INFO &_rstInfo);

private:
  orxFLOAT current = 0.0;
};
