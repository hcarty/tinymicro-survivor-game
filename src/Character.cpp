#include "Character.h"

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
  PushConfigSection();
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
