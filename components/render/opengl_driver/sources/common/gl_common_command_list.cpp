#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    Константы
*/

const size_t COMMAND_ARRAY_RESERVE_SIZE = 16; //резервируемое число команд в буфере

}

/*
    Описание реализации построителя списка команд
*/

typedef stl::vector<detail::ICommand*> CommandArray;

struct CommandListBuilder::Impl: public xtl::reference_counter, public ICommandListExecuter
{
  CommandArray commands; //команды

///Конструктор
  Impl ()
  {
    commands.reserve (COMMAND_ARRAY_RESERVE_SIZE);
  }

///Деструктор
  ~Impl ()
  {
    DeleteCommands ();
  }
  
///Удаление команд
  void DeleteCommands ()
  {
    for (CommandArray::iterator iter=commands.begin (), end=commands.end (); iter!=end; ++iter)
      delete *iter;

    commands.clear ();
  }

///Выполнение очереди команд
  void ExecuteCommands ()
  {
    try
    {
      for (CommandArray::iterator iter=commands.begin (), end=commands.end (); iter!=end; ++iter)
      {
        (*iter)->Execute ();
      }
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }

///Подсчёт ссылок
  void AddRef () { addref (this); }
  void Release () { release (this); }
};

/*
    Конструктор / деструктор
*/

CommandListBuilder::CommandListBuilder ()
  : impl (0)
{
}

CommandListBuilder::~CommandListBuilder ()
{
  if (impl)
    release (impl);
}

/*
    Добавление команды
*/

void CommandListBuilder::AddCore (detail::ICommand* command)
{
  if (!impl)
    impl = new Impl;

  impl->commands.push_back (command);
}

/*
    Завершение списка команд
*/

CommandListBuilder::ExecuterPtr CommandListBuilder::Finish ()
{
  if (!impl)
    impl = new Impl;

    //возвращение класса, выполнящего команды вручную

  ExecuterPtr executer = impl;

    //очистка списка

  release (impl);

  impl = 0;

  return executer;
}
