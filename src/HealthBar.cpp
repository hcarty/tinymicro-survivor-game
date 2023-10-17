#include "HealthBar.h"

void HealthBar::Add(orxFLOAT healthDiff)
{
  level += healthDiff;
  if (level < 0.0)
  {
    level = 0.0;
  }
  if (level > MAX_LEVEL)
  {
    level = MAX_LEVEL;
  }
}

orxBOOL HealthBar::IsEmpty()
{
  return level <= 0.0;
}

void HealthBar::OnCreate()
{
  orxConfig_SetBool("IsHealthBar", orxTRUE);
}

void HealthBar::OnDelete()
{
}

void HealthBar::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Update health bar scale
  auto bar = GetChild("*.HealthBarLevel");
  orxVECTOR scale;
  bar->GetScale(scale);
  scale.fX = level / MAX_LEVEL;
  bar->SetScale(scale);

  Object::Update(_rstInfo);
}
