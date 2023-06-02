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

orxBOOL Mob::OnCollide(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
  if (orxString_Compare(orxBody_GetPartName(_pstColliderPart), "AttackProjectileBodyPart") == 0)
  {
    SetLifeTime(0.0);
  }

  return orxTRUE;
}

void Mob::Update(const orxCLOCK_INFO &_rstInfo)
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
