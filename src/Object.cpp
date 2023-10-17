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

ScrollObject *Object::GetChild(const orxSTRING search)
{
  auto child = orxObject_FindChild(GetOrxObject(), search);
  orxASSERT(child, "Unable to find child object at %s", search);
  auto scrollChild = static_cast<ScrollObject *>(orxObject_GetUserData(child));
  orxASSERT(scrollChild, "Child object at path %s is not a Scroll object", search);
  return scrollChild;
}
