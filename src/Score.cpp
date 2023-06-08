#include "Score.h"

void Score::OnCreate()
{
  orxConfig_SetBool("IsScore", orxTRUE);
}

void Score::OnDelete()
{
}

void Score::Update(const orxCLOCK_INFO &_rstInfo)
{
  // We only update the score if the game isn't over yet
  orxConfig_PushSection("Runtime");
  auto gameOver = orxConfig_GetBool("GameOver");
  orxConfig_PopSection();

  if (!gameOver)
  {
    // Update score based on elapsed time
    current += _rstInfo.fDT;

    // Update the text to match the floor of the current score
    const size_t MAX_LABEL_LENGTH = 32;
    orxCHAR label[MAX_LABEL_LENGTH] = "";
    orxString_NPrint(label, MAX_LABEL_LENGTH, "Score: %g", orxMath_Floor(current));
    orxObject_SetTextString(GetOrxObject(), label);
  }

  Object::Update(_rstInfo);
}
