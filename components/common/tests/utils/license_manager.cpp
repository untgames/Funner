#include <cstdio>

#include <xtl/exception.h>

#include <common/license_manager.h>

using namespace common;

const char*  COMPONENTS_TO_CHECK []    = { "common.zip_file_system", "media.video.loaders.theora", "media.font.savers.xfont"};
const size_t COMPONENTS_TO_CHECK_COUNT = sizeof (COMPONENTS_TO_CHECK) / sizeof (*COMPONENTS_TO_CHECK);
const char*  LICENSES_PATHS []         = { "data/expired_license.xml", "data/future_license.xml",
                                           "data/invalid_date_license.xml", "data/invalid_license.xml",
                                           "data/valid_license.xml", "data/check_files_license.xml" };

int main ()
{
  printf ("Results of license_manager_test:\n");

  try
  {
    for (size_t i = 0; i < COMPONENTS_TO_CHECK_COUNT; i++)
    {
      printf ("Component '%s' allowed: ", COMPONENTS_TO_CHECK [i]);
      printf ("%s\n", LicenseManager::IsComponentAllowed (COMPONENTS_TO_CHECK [i]) ? "yes" : "no");
    }
  }
  catch (xtl::exception& e)
  {
    printf ("Exception: '%s'\n", e.what ());
  }

  for (size_t i = 0, count = sizeof (LICENSES_PATHS) / sizeof (*LICENSES_PATHS); i < count; i++)
  {
    printf ("Loading license '%s'\n", LICENSES_PATHS [i]);

    try
    {
      LicenseManager::Load (LICENSES_PATHS [i]);

      for (size_t j = 0; j < COMPONENTS_TO_CHECK_COUNT; j++)
        printf ("Component '%s' allowed: %s\n", COMPONENTS_TO_CHECK [j], LicenseManager::IsComponentAllowed (COMPONENTS_TO_CHECK [j]) ? "yes" : "no");
    }
    catch (xtl::exception& e)
    {
      printf ("Exception: '%s'\n", e.what ());
    }
  }

  return 0;
}
