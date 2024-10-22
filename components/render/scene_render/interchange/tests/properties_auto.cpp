#include "shared.h"

using namespace common;

void dump_properties (const PropertyMap& properties)
{
  printf ("map has %u properties:\n", properties.Size ());
    
  for (size_t i=0, count=properties.Size (); i<count; i++)
  {  
    stl::string value;
    
    properties.GetProperty (i, value);
    
    printf (" #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

class Checker
{
  public:
    Checker () {}

    PropertyMapAutoWriter::Synchronizer CreateSynchronizer (const PropertyMap& properties)
    {
      return writer.CreateSynchronizer (properties);
    }

    void Sync ()
    {
      OutputStream out_stream;

      writer.Write (out_stream);

      InputStream in_stream;

      CommandBuffer buffer;

      out_stream.Swap (buffer);

      in_stream.Reset (buffer);
      
      while (in_stream.Available ())
      {
        const Command& command   = read (in_stream, xtl::type<Command> ());
        size_t         body_size = command.command_size - sizeof (Command);

        if (in_stream.Available () < body_size)
        {
          stl::string command_name = get_command_name ((CommandId)command.command_id);

          printf ("Can't read command %s with size %u: only %u bytes is available\n", command_name.c_str (), command.command_size, body_size);

          break; //no more data
        }

        switch (command.command_id)
        {
          case CommandId_UpdatePropertyMap:
            printf ("reader: update property map command received\n");
            reader.Read (in_stream);
            break;
          case CommandId_RemovePropertyMap:
            printf ("reader: remove property map command received\n");
            reader.RemoveProperties (read (in_stream, xtl::type<object_id_t> ()));
            break;
          case CommandId_RemovePropertyLayout:
            printf ("reader: remove property layout command received\n");
            reader.RemoveLayout (read (in_stream, xtl::type<object_id_t> ()));
            break;
          default:
            printf ("Unknown command %u with size %u. Will be skipped", command.command_id, command.command_size);
            in_stream.Skip (body_size);
            break;
        }
      }
    }

    PropertyMap GetProperties (object_id_t id) { return reader.GetProperties (id); }
    bool        HasProperties (object_id_t id) { return reader.HasProperties (id); }

  private:
    PropertyMapAutoWriter writer;
    PropertyMapReader     reader;
};

int main ()
{
  printf ("Results of properties_auto_test:\n");

  try
  {
    Checker checker;

    {
      object_id_t id1 = 0, id2 = 0;

      {     
        PropertyMap properties1, properties2;

        properties1.SetProperty ("X", "hello1");
        properties1.SetProperty ("Y", 1);

        PropertyMapAutoWriter::Synchronizer s1 = checker.CreateSynchronizer (properties2);

        properties2.SetProperty ("X", "hello2");
        properties2.SetProperty ("Y", 2);

        id1 = properties1.Id ();
        id2 = properties2.Id ();

        PropertyMapAutoWriter::Synchronizer s2 = checker.CreateSynchronizer (properties1);

        checker.Sync ();

        PropertyMap properties3 = checker.GetProperties (id1);
        PropertyMap properties4 = checker.GetProperties (id2);

        dump_properties (properties3);
        dump_properties (properties4);

        printf ("dummy update\n");

        checker.Sync (); 
      }

      printf ("remove properties\n");

      checker.Sync ();

      printf ("has properties: %s %s\n", checker.HasProperties (id1) ? "true" : "false", checker.HasProperties (id2) ? "true" : "false");
    }

    printf ("after exit from scope\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
