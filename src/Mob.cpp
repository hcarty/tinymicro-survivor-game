#include "Mob.h"

void Mob::OnCreate()
{
  orxConfig_SetBool("IsMob", orxTRUE);

  // Get a movement speed specific to this mob
  auto speed = orxConfig_GetFloat("Speed");
  PushConfigSection(orxTRUE);
  orxConfig_SetFloat("Speed", speed);
  PopConfigSection();
}

void Mob::OnDelete()
{
}

void Mob::OnCollide(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
  auto colliderPartName = orxBody_GetPartName(_pstColliderPart);
  orxConfig_PushSection(colliderPartName);
  const auto kill = orxConfig_GetBool("MobKill");
  orxConfig_PopSection();

  if (ready && kill)
  {
    ready = false;
    SetSpeed(orxVECTOR_0);
    AddFX("DieFX");
    orxObject_SetLiteralLifeTime(GetOrxObject(), "fx");
  }
}

void Mob::OnFXStop(const orxSTRING _zFX, orxFX *_pstFX)
{
  auto appearFXName = "AppearFX";
  if (orxString_NCompare(_zFX, appearFXName, orxString_GetLength(appearFXName)) == 0)
  {
    ready = true;
  }
}

void Mob::Update(const orxCLOCK_INFO &_rstInfo)
{
  if (ready)
  {
    // Get a target to chase
    orxConfig_PushSection("Runtime");
    orxVECTOR target = orxVECTOR_0;
    orxConfig_GetVector("Target", &target);
    orxConfig_PopSection();

    // Move toward our target
    orxVECTOR position = orxVECTOR_0;
    GetPosition(position, orxTRUE);

    orxVECTOR speed = orxVECTOR_0;
    orxVector_Sub(&speed, &target, &position);
    orxVector_Normalize(&speed, &speed);
    PushConfigSection(orxTRUE);
    orxVector_Mulf(&speed, &speed, orxConfig_GetFloat("Speed"));
    PopConfigSection();
    SetSpeed(speed);
  }
}
