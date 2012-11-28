#include "shared.h"

using namespace social;
using namespace social::facebook;

void FacebookSessionImpl::PerformRequestNotify (const RequestCallback& callback, bool succeeded, const char* status, const common::ParseNode& response)
{
  common::ActionQueue::PushAction (xtl::bind (callback, succeeded, stl::string (status), response), common::ActionThread_Main);
}

void FacebookSessionImpl::PerformRequestImpl (common::Action& action, const stl::string& url, const RequestCallback& callback, common::Log log)
{
  try
  {
    common::StdFile input_file (url.c_str (), common::FileMode_Read);

    xtl::uninitialized_storage<char> buffer (input_file.Size () + 1);

    size_t bytes_copied = 0;

    while (!input_file.Eof ())
    {
      if (action.IsCanceled ())
        return;

      size_t read_size = input_file.Read (buffer.data () + bytes_copied, buffer.size () - bytes_copied);

      if (!read_size)
        break;

      bytes_copied += read_size;
    }

    if (!input_file.Eof ())
      throw xtl::format_operation_exception ("", "Internal error: can't read input file");

    buffer.data () [bytes_copied] = 0;

    log.Printf ("Response received: '%s'", buffer.data ());

    common::Parser   parser ("json", bytes_copied, buffer.data (), "json");
    common::ParseLog parse_log = parser.Log ();

    for (size_t i = 0; i < parse_log.MessagesCount (); i++)
      switch (parse_log.MessageType (i))
      {
        case common::ParseLogMessageType_Error:
        case common::ParseLogMessageType_FatalError:
          throw xtl::format_operation_exception ("", "Parser error: '%s'", parse_log.Message (i));
          break;
        default:
          break;
      }

    common::ParseNode iter = parser.Root ().First ();

    if (!iter)
      throw xtl::format_operation_exception ("", "There is no root node in response");

    if (action.IsCanceled ())
      return;

    //TODO check for errors (http://developers.facebook.com/docs/reference/api/errors/)

    PerformRequestNotify (callback, true, "Ok", iter);
  }
  catch (xtl::exception& e)
  {
    if (action.IsCanceled ())
      return;

    log.Printf ("Request failed, error '%s'", e.what ());
    PerformRequestNotify (callback, false, e.what (), common::ParseNode ());
  }
  catch (...)
  {
    if (action.IsCanceled ())
      return;

    log.Printf ("Request failed, unknown error");
    PerformRequestNotify (callback, false, "Unknown error", common::ParseNode ());
  }
}

/*
   Выполнение запроса
*/

void FacebookSessionImpl::PerformRequest (const char* method_name, const char* params, const RequestCallback& callback)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::PerformRequest";

  if (!method_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "method_name");

  if (!params)
    throw xtl::make_null_argument_exception (METHOD_NAME, "params");

  if (token.empty ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't perform request when is not logged in");

  CleanupRequestsActions (); //TODO this should be called from other place!!!!!!!!

  stl::string url = common::format ("https://graph.facebook.com/%s?%s&access_token=%s", method_name, params, token.c_str ());

  url = percent_escape (url.c_str ());

  log.Printf ("Performing request '%s'", url.c_str ());

  pending_actions.push_back (common::ActionQueue::PushAction (xtl::bind (&FacebookSessionImpl::PerformRequestImpl, _1, url, callback, log), common::ActionThread_Background));
}

void FacebookSessionImpl::CleanupRequestsActions ()
{
  //Where should we call this

  for (ActionsList::iterator iter = pending_actions.begin (), end = pending_actions.end (); iter != end;)
  {
    if (iter->IsCompleted () || iter->IsCanceled ())
    {
      ActionsList::iterator next = iter;

      ++next;

      pending_actions.erase (iter);

      iter = next;
    }
    else
      ++iter;
  }
}
