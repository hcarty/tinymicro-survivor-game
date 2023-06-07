#pragma once

#include "Object.h"

class HealthBar : public Object
{
public:
  void Add(orxFLOAT healthDiff);
  orxBOOL IsEmpty();

protected:
  void OnCreate();
  void OnDelete();
  void Update(const orxCLOCK_INFO &_rstInfo);

private:
  const orxFLOAT MAX_LEVEL = 100.0;
  const orxFLOAT MIN_LEVEL = 0.0;
  orxFLOAT level = MAX_LEVEL;
};
