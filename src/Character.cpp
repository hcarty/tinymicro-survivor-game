#include "Character.h"
#include "HealthBar.h"

void Character::OnCreate()
{
  orxConfig_SetBool("IsCharacter", orxTRUE);
  inputSet = orxConfig_GetString("Input");
  orxInput_EnableSet(inputSet.data(), orxTRUE);
}

void Character::OnDelete()
{
  orxInput_EnableSet(inputSet.data(), orxFALSE);
}

void Character::Update(const orxCLOCK_INFO &_rstInfo)
{
  auto healthBar = ScrollCast<HealthBar *, ScrollObject *>(GetChildByName("HealthBar"));

  // If our health has run out, it's game over!
  if (healthBar->IsEmpty())
  {
    SetLifeTime(0.0);
    orxConfig_PushSection("Runtime");
    orxConfig_SetBool("GameOver", true);
    orxConfig_PopSection();
  }

  PushConfigSection();

  // Heal a little bit based on how much time has passed
  healthBar->Add(_rstInfo.fDT * orxConfig_GetFloat("HealthPS"));

  auto previousSet = orxInput_GetCurrentSet();
  orxInput_SelectSet(inputSet.data());

  orxVECTOR speed = {
      orxInput_GetValue("Right") - orxInput_GetValue("Left"),
      orxInput_GetValue("Down") - orxInput_GetValue("Up"),
      0.0};

  orxVector_Mulf(&speed, &speed, orxConfig_GetFloat("Speed"));
  SetSpeed(speed);

  orxInput_SelectSet(previousSet);
  PopConfigSection();

  // Save position to config so mobs can track
  orxVECTOR position = orxVECTOR_0;
  GetPosition(position, orxTRUE);
  orxConfig_PushSection("Runtime");
  orxConfig_SetVector("Target", &position);

  orxConfig_PopSection();
}

void Character::OnCollide(ScrollObject *_poCollider, orxBODY_PART *_pstPart, orxBODY_PART *_pstColliderPart, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
  orxASSERT(_poCollider);

  // Check for a health impact from the body part
  auto colliderPartName = orxBody_GetPartName(_pstColliderPart);
  if (orxConfig_HasSection(colliderPartName))
  {
    orxConfig_PushSection(colliderPartName);
    auto impact = orxConfig_GetS32("HealthImpact");
    orxConfig_PopSection();

    // Apply the effect of the impact on our health
    auto healthBar = ScrollCast<HealthBar *, ScrollObject *>(GetChildByName("HealthBar"));
    healthBar->Add(static_cast<orxFLOAT>(impact));
  }

  // Check for pickup actions
  _poCollider->PushConfigSection();
  if (orxConfig_GetBool("IsPickup"))
  {
    _poCollider->SetLifeTime(0);
    auto pickupCommand = orxConfig_GetString("OnPickup");
    if (orxString_GetLength(pickupCommand) > 0)
    {
      orxCOMMAND_VAR _result;
      orxCommand_EvaluateWithGUID(pickupCommand, GetGUID(), &_result);
    }
  }
  _poCollider->PopConfigSection();
}
