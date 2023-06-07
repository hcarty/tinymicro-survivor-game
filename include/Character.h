#pragma once

#include <string>
#include "Object.h"

class Character : public Object
{
public:
protected:
  void OnCreate();
  void OnDelete();
  void Update(const orxCLOCK_INFO &_rstInfo);
  orxBOOL OnCollide(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);

private:
  std::string inputSet{""};
};
