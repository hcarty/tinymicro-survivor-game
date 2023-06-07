/**
 * @file Object.cpp
 * @date 30-May-2023
 */

#include "Object.h"

void Object::OnCreate()
{
  orxConfig_SetBool("IsObject", orxTRUE);
}

void Object::OnDelete()
{
}

void Object::Update(const orxCLOCK_INFO &_rstInfo)
{
}

ScrollObject *Object::GetChildByName(const orxSTRING childName)
{
  for (ScrollObject *child = this->GetOwnedChild(); child; child = child->GetOwnedSibling())
  {
    if (orxString_Compare(child->GetModelName(), childName) == 0)
    {
      return child;
    }
  }

  // If we get this far, fail hard
  orxASSERT(false, "Unable to find child object named %s", childName);
  return orxNULL;
}
