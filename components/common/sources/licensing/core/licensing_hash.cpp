#include "license_manager_shared.h"

namespace
{

struct LicensePropertyDesc : public xtl::reference_counter
{
  stl::string name;
  stl::string value;

  LicensePropertyDesc (const char* in_name, const char* in_value)
    : name (in_name), value (in_value)
    {}
};

typedef xtl::intrusive_ptr<LicensePropertyDesc> LicensePropertyDescPtr;
typedef stl::vector<LicensePropertyDescPtr>     PropertiesArray;

bool property_desc_compare (LicensePropertyDescPtr property1, LicensePropertyDescPtr property2)
{
  return property1->name > property2->name;
}

}

namespace common
{

void calculate_license_hash (const StringArray& check_files_list, const PropertyMap& properties, unsigned char hash [16])
{
  try
  {
    //проверка корректности содержимого лицензии
    stl::string check_string;

    for (size_t i = 0, count = check_files_list.Size (); i < count; i++)
    {
      FileHash file_hash;

      FileSystem::GetFileHash (check_files_list [i], file_hash);

      check_string += check_files_list [i];

      check_string.append ((const char*)file_hash.md5, sizeof (file_hash.md5));
    }

    PropertiesArray properties_array;

    properties_array.reserve (properties.Size ());

    for (size_t i = 0, count = properties.Size (); i < count; i++)
      properties_array.push_back (LicensePropertyDescPtr (new LicensePropertyDesc (properties.PropertyName (i), properties.GetString (i)), false));

    stl::sort (properties_array.begin (), properties_array.end (), property_desc_compare);

    for (PropertiesArray::iterator iter = properties_array.begin (), end = properties_array.end (); iter != end; ++iter)
    {
      check_string += (*iter)->name.c_str ();
      check_string += (*iter)->value.c_str ();
    }

    md5 (check_string.data (), check_string.size (), hash);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::calculate_license_hash");
    throw;
  }
}

}
