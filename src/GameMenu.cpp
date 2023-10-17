#include "GameMenu.h"
#include "HealthBar.h"

void GameMenu::OnCreate()
{
  orxConfig_SetBool("IsCharacter", orxTRUE);
  inputSet = orxConfig_GetString("Input");
  orxInput_EnableSet(inputSet.data(), orxTRUE);
}

void GameMenu::OnDelete()
{
  orxInput_EnableSet(inputSet.data(), orxFALSE);
}

void GameMenu::Update(const orxCLOCK_INFO &_rstInfo)
{
  const auto activeMenuKey = "Active";
  const auto activeFXKey = "MenuActiveFX";
  const auto activeFXSetKey = "ActiveFXSet";

  PushConfigSection();
  // Get currently active menu object
  auto activeMenuItem = orxConfig_GetString(activeMenuKey);
  orxCHAR activeMenuSearch[256] = "";
  orxString_NPrint(activeMenuSearch, sizeof(activeMenuSearch), "*.%s", activeMenuItem);
  auto menuObject = orxObject_FindChild(GetOrxObject(), activeMenuSearch);

  if (!menuObject)
  {
    // Give up and return if the menu object is not valid
    orxLOG("Invalid active menu object %s", activeMenuItem);
    PopConfigSection();
    return;
  }

  // Have we set the FX for this object?
  auto activeFXSet = orxConfig_GetBool(activeFXSetKey);

  if (!activeFXSet && menuObject)
  {
    // Set the active FX for the active menu object
    orxObject_AddUniqueFX(menuObject, activeFXKey);
    // Record that we've set the FX
    orxConfig_SetBool(activeFXSetKey, orxTRUE);
  }

  // Record previous input set
  auto prevInputSet = orxInput_GetCurrentSet();

  // Select our input set
  orxInput_SelectSet(inputSet.data());

  // Check for menu inputs
  auto up = orxInput_HasBeenActivated("Up");
  auto down = orxInput_HasBeenActivated("Down");
  auto activate = orxInput_HasBeenActivated("Activate");

  // Restore previous input set
  orxInput_SelectSet(prevInputSet);

  if (activate)
  {
    // Get the command to evaluate
    orxConfig_PushSection(activeMenuItem);
    auto command = orxConfig_GetString("OnActivate");
    orxConfig_PopSection();

    // Evaluate the command
    orxCOMMAND_VAR _result;
    orxCommand_Evaluate(command, &_result);

    // Reset config state
    orxConfig_SetBool(activeFXSetKey, orxFALSE);
  }
  else if (up && down)
  {
    // Do nothing
  }
  else if (up || down)
  {
    // Get the section name for the next item in the menu
    orxConfig_PushSection(activeMenuItem);
    auto section = up ? orxConfig_GetString("Prev") : orxConfig_GetString("Next");
    orxConfig_PopSection();

    // Set the next item in the menu as the active item
    orxConfig_SetString(activeMenuKey, section);

    // Reset FX state for the previously selected active item
    orxConfig_SetBool(activeFXSetKey, orxFALSE);
    orxObject_RemoveFX(menuObject, activeFXKey);

    // Reset menu object color
    orxObject_SetColor(menuObject, orxNULL);
  }
  PopConfigSection();
}
