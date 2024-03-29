#pragma once

#include "tinymicro.h"

class Object : public ScrollObject
{
public:
protected:
  void OnCreate();
  void OnDelete();
  void Update(const orxCLOCK_INFO &_rstInfo);
  ScrollObject *GetChild(const orxSTRING search);

private:
};
