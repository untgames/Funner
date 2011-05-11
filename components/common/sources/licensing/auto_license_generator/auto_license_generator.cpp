#include <common/xml_writer.h>

#include <license_manager_shared.h>

using namespace common;

namespace
{

const char* ALLOWED_COMPONENT = "*";
const char* COMPONENT_NAME    = "common.licensing.loaders.AutoLicenseGenerator";

//генерация и загрузка лицензии
void load_license ()
{
  time_t current_time;

  time (&current_time);

  tm* current_date = gmtime (&current_time);

  if (!current_date)
    throw xtl::format_operation_exception ("common::AutoLicenseGenerator::load_license", "Can't get current date");

  stl::string since_date_string = common::format ("%04d-%02d-%02d", current_date->tm_year + 1900, current_date->tm_mon + 1, current_date->tm_mday);

  if (current_date->tm_mon == 11)
  {
    current_date->tm_year++;
    current_date->tm_mon = 0;
  }
  else
    current_date->tm_mon++;

  stl::string till_date_string = common::format ("%04d-%02d-%02d", current_date->tm_year + 1900, current_date->tm_mon + 1, current_date->tm_mday);

  unsigned char license_hash [16];

  PropertyMap properties;

  properties.SetProperty ("SinceDate", since_date_string.c_str ());
  properties.SetProperty ("TillDate", till_date_string.c_str ());
  properties.SetProperty ("AllowedComponents", ALLOWED_COMPONENT);

  calculate_license_hash (stl::vector<CheckFile> (), properties, license_hash);

  stl::string license_hash_string;

  for (size_t i = 0; i < 16; i++)
    license_hash_string += common::format ("%02x", license_hash [i]);

  stl::string license_string = common::format ("<?xml version='1.0' encoding='utf-8'?><License SinceDate='%s' TillDate='%s' LicenseHash='%s'><AllowedComponents><Component Wildcard='%s'/></AllowedComponents></License>",
                                               since_date_string.c_str (), till_date_string.c_str (), license_hash_string.c_str (), ALLOWED_COMPONENT);

  MemFile license_file;

  license_file.Write (license_string.data (), license_string.size ());

  LicenseManager::Load (license_file.Path ());
}

/*
    Компонент, регистрирующий создателя лицензии
*/

class AutoLicenseGeneratorComponent
{
  public:
    AutoLicenseGeneratorComponent ()
    {
      load_license ();
    }
};

}

extern "C"
{

ComponentRegistrator<AutoLicenseGeneratorComponent> AutoLicenseGenerator (COMPONENT_NAME);

}
