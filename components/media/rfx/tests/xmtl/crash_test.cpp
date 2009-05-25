#include "shared.h"

const char* LIBRARY_FILE_NAME = "data/empty.xmtl";

#include <stl/hash_map>

#include <xtl/iterator.h>

typedef stl::hash_map<stl::hash_key<const char*>, int> MaterialMap;

int main ()
{
  try
  {
    printf ("Results of crash_test:\n");

    MaterialMap materials;

    MaterialLibrary library (LIBRARY_FILE_NAME);

    materials ["Cursor_Main"] = 5;
    materials ["Cursor_Pressed"] = 5;
    materials ["Window_MainMenu_TitleRus"] = 5;
    materials ["Window_Options_Button"] = 5;
    materials ["Window_MainMenu_TitleEng"] = 5;
    materials ["Window_MainMenu"] = 5;
    materials ["Window_Options_SliderHead"] = 5;
    materials ["Window_MainMenu_Button"] = 5;
    materials ["Window_MainMenu_Exit"] = 5;
    materials ["Window_MainMenu_candles_fire"] = 5;
    materials ["Window_MainMenu_Blood"] = 5;
    materials ["Location_DvorDom"] = 5;
    materials ["Window_MainMenu_Ball"] = 5;
    materials ["Window_Options"] = 5;
    materials ["Window_Options_Slider"] = 5;
    materials ["Item_chain_link_1"] = 5;
    materials ["Window_Options_FullscreenFlag"] = 5;
    materials ["Window_Options_FullscreenFlagBack"] = 5;
    materials ["Window_Options_ButtonLight"] = 5;
    materials ["Window_Options_CandleFire"] = 5;
    materials ["Item_chain_link_3"] = 5;
    materials ["Item_wheel_rim"] = 5;
    materials ["Item_chain_link_2"] = 5;
    materials ["Item_chain"] = 5;
    materials ["Item_handle"] = 5;
    materials ["Item_needle_1"] = 5;
    materials ["Item_needle_2"] = 5;
    materials ["Item_needle_3"] = 5;
    materials ["Item_wheel_0"] = 5;
    materials ["Item_wheel"] = 5;
    materials ["Location_DvorMelnica"] = 5;
    materials ["Location_Kamin"] = 5;

    MaterialLibrary library2 (LIBRARY_FILE_NAME);

    materials ["Minigame_Balls_Background"] = 5;
    materials ["Minigame_Balls_UserFields"] = 5;
    materials ["Minigame_Balls_Exit"] = 5;
    materials ["Minigame_Balls_ExitGlow"] = 5;
    materials ["Minigame_Balls_Selection"] = 5;
    materials ["Minigame_Balls_AnimalBall"] = 5;
    materials ["Minigame_Balls_FireBall"] = 5;
    materials ["Minigame_Balls_MoonBall"] = 5;
    materials ["Minigame_Balls_NatureBall"] = 5;
    materials ["Minigame_Balls_StarBall"] = 5;
    materials ["Minigame_Balls_LeftFieldLeft"] = 5;
    materials ["Minigame_Balls_LeftFieldRight"] = 5;
    materials ["Minigame_Balls_RightFieldLeft"] = 5;
    materials ["Minigame_Balls_RightFieldRight"] = 5;

    MaterialLibrary library3 (LIBRARY_FILE_NAME);

    materials.erase ("Cursor_Main");
    materials.erase ("Cursor_Pressed");
    materials.erase ("Window_MainMenu_TitleRus");
    materials.erase ("Window_Options_Button");
    materials.erase ("Window_MainMenu_TitleEng");
    materials.erase ("Window_MainMenu");
    materials.erase ("Window_Options_SliderHead");
    materials.erase ("Window_MainMenu_Button");
    materials.erase ("Window_MainMenu_Exit");
    materials.erase ("Window_MainMenu_candles_fire");
    materials.erase ("Window_MainMenu_Blood");
    materials.erase ("Location_DvorDom");
    materials.erase ("Window_MainMenu_Ball");
    materials.erase ("Window_Options");
    materials.erase ("Window_Options_Slider");
    materials.erase ("Item_chain_link_1");
    materials.erase ("Window_Options_FullscreenFlag");
    materials.erase ("Window_Options_FullscreenFlagBack");
    materials.erase ("Window_Options_ButtonLight");
    materials.erase ("Window_Options_CandleFire");
    materials.erase ("Item_chain_link_3");
    materials.erase ("Item_wheel_rim");
    materials.erase ("Item_chain_link_2");
    materials.erase ("Item_chain");
    materials.erase ("Item_handle");
    materials.erase ("Item_needle_1");
    materials.erase ("Item_needle_2");
    materials.erase ("Item_needle_3");
    materials.erase ("Item_wheel_0");
    materials.erase ("Item_wheel");
    materials.erase ("Location_DvorMelnica");
    materials.erase ("Location_Kamin");

    MaterialLibrary library4 (LIBRARY_FILE_NAME);

    materials ["Story_Intro_Background"] = 5;
    materials ["Story_Intro_Scroll"] = 5;
    materials ["Story_Intro_Boy"] = 5;
    materials ["Story_Intro_SorcererAndBoy"] = 5;
    printf ("before crash\n");
    materials ["Story_Intro_Raven"] = 5;

    printf ("before deleting material libraries\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  printf ("Success!!!\n");

  return 0;
}
