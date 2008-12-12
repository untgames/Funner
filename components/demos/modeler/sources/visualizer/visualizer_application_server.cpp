#include <direct.h>
#include <process.h>

#include <common/strlib.h>
#include <common/xml_writer.h>

#include <script/bind.h>
#include <script/environment.h>

#include "shared.h"

using namespace scene_graph;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

/*
    Константы
*/

const char*  APPLICATION_CONFIGURATION_FILE_NAME = "media/conf/application_config.xml";
const char*  SCREEN_ATTACHMENT_NAME              = "MainScreen";
const char*  INTERPRETER_NAME                    = "lua";
const char*  DESC_FILE_SUFFIX                    = ".desc";
const char*  CONDOR_SUBMIT_APPLICATION_NAME      = "condor_submit.exe";
const char*  WIN32_ENVELOPE_APPLICATION_NAME     = "modeler-envelope.exe";
const char*  OSX_ENVELOPE_APPLICATION_NAME       = "modeler-envelope";
const char*  WIN32_TRAJECTORY_APPLICATION_NAME   = "modeler-trajectory.exe";
const char*  OSX_TRAJECTORY_APPLICATION_NAME     = "modeler-trajectory";
const char*  TRAJECTORY_APPLICATION_BASE_NAME    = "modeler-trajectory";
const char*  CONDOR_BINARIES_PATH                = "condor_binaries";
const char*  MESH_CONVERTER_APPLICATION_NAME     = "mesh-converter.exe";
const char*  MODEL_REGISTRY_NAME                 = "ApplicationServer.Properties.Model";
const char*  MODEL_FILE_NAME                     = "model.xmodel";
const char*  OLD_FORMAT_MODEL_FILE_NAME          = "model.dat";
const char*  CONDOR_CONFIG_FILE_NAME             = "condor.cfg";
const char*  BATCH_TRAJECTORY_NU_FILE_FOLDER     = "condor_input_files";
const char*  BATCH_TRAJECTORY_NU_FILE_BASE_NAME  = "nu";
const char*  BATCH_TRAJECTORY_NU_FILE_SUFFIX     = ".dat";
const char*  BATCH_TRAJECTORY_LOG_FILE_NAME      = "batch_trajectory.log";
const char*  BATCH_TRAJECTORIES_COORDS_FILE_NAME = "trajecories_coords.bintrjc";
const size_t DEFAULT_ENVELOPE_LOD                = 200;
const size_t COPY_FILE_BUFFER                    = 1024 * 1024;
const int    TRAJECTORIES_COORDS_HEADER          = 'TRJC';
const int    TRAJECTORIES_COORDS_VERSION         = 1;

void file_read (const char* source, common::InputFile& file, void* data, size_t size)
{
  if (file.Read (data, size) != size)
    throw xtl::format_operation_exception (source, "Can't read data from file at %u, file size is %u",
                                           file.Tell (), file.Size ());
}

void file_write (const char* source, common::OutputFile& file, const void* data, size_t size)
{
  if (file.Write (data, size) != size)
    throw xtl::format_operation_exception (source, "Can't write %u bytes of data to file at %u",
                                           size, file.Tell ());
}

void remove_files (const char* mask)
{
  common::FileList files_to_delete = common::FileSystem::Search (mask, common::FileSearch_Files);

  for (size_t i = 0, size = files_to_delete.Size (); i < size; i++)
    common::FileSystem::Remove (files_to_delete.Item (i).name);
}

void copy_file (const char* source_file_name, const char* result_file_name)
{
  static const char* METHOD_NAME = "::copy_file";

  common::InputFile  source_file (source_file_name);
  common::OutputFile result_file (result_file_name);

  xtl::uninitialized_storage<char> copy_buffer (COPY_FILE_BUFFER);

  for (size_t i = 0, size = source_file.Size (); i < size;)
  {
    size_t read_block_size = stl::min (size - i, COPY_FILE_BUFFER);

    file_read  (METHOD_NAME, source_file, copy_buffer.data (), read_block_size);
    file_write (METHOD_NAME, result_file, copy_buffer.data (), read_block_size);

    i += read_block_size;
  }
}

}

//тестовый сервер приложения

MyApplicationServer::MyApplicationServer ()
  : shell_environment (new script::Environment),
    shell (INTERPRETER_NAME, shell_environment),
    wait_files_timer (xtl::bind (&MyApplicationServer::CheckNewFiles, this), 1000, syslib::TimerState_Paused)
{
  common::FileSystem::SetDefaultFileBufferSize (0);     //???????? обход бага!!!!!!!!!!!

  static const char* METHOD_NAME = "MyApplicationServer::MyApplicationServer";

  engine::AttachmentRegistry::Register ("MeshResourceManager", resource_manager);

    //получение текущей папки

  char* buffer;

  if ((buffer = _getcwd (0, 0)) == 0)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get current directory, _getcwd error");
  else
  {
    working_directory = buffer;

    free (buffer);
  }

    //регистрация шлюзов

  shell_environment->BindLibraries ("*");

    //создание глобальной точки входа

  script::InvokerRegistry& lib = shell_environment->Library ("Modeler");

    //регистрация функций

  lib.Register ("CalculateEnvelope", script::make_invoker<void (size_t)> (xtl::bind (&MyApplicationServer::CalculateEnvelope, this, _1)));
  lib.Register ("CalculateTrajectory", script::make_invoker<void (double, double, double, size_t)> (xtl::bind (&MyApplicationServer::CalculateTrajectory, this, _1, _2, _3, _4)));
  lib.Register ("CalculateTrajectories", script::make_invoker<void (size_t, size_t)> (xtl::bind (&MyApplicationServer::CalculateTrajectories, this, _1, _2)));
  lib.Register ("Cleanup", script::make_invoker<void ()> (xtl::bind (&MyApplicationServer::Cleanup, this)));
  lib.Register ("SaveModel", script::make_invoker<void ()> (xtl::bind (&MyApplicationServer::SaveModel, this)));

    //чтение конфигурации

  common::Parser   parser     (APPLICATION_CONFIGURATION_FILE_NAME);
  common::ParseLog parser_log (parser.Log ());

  for (size_t i = 0; i < parser_log.MessagesCount (); i++)
  {
    common::ParseLogMessageType message_type = parser_log.MessageType (i);

    if (message_type == common::ParseLogMessageType_Error || message_type == common::ParseLogMessageType_FatalError)
      throw xtl::format_operation_exception (METHOD_NAME, "Configuration parse error: '%s'", parser_log.Message (i));
  }

  common::ParseIterator iter = parser.Root ().First ("Configuration");

  win32_plugin_path = common::get<const char*> (*iter, "Win32PluginPath");
  osx_plugin_path   = common::get<const char*> (*iter, "MacOSXPluginPath", "");
  condor_path       = common::get<const char*> (*iter, "CondorPath", "");
  use_condor        = common::get<bool>        (*iter, "UseCondor", true);

  if (use_condor)
    CreateCondorBinaries ();

  camera = OrthoCamera::Create ();

  camera->SetLeft   (-3);
  camera->SetRight  (3);
  camera->SetBottom (-3);
  camera->SetTop    (3);
  camera->SetZNear  (0);
  camera->SetZFar   (100);

  camera->SetPosition (0, 0, 4);
  camera->LookTo      (math::vec3f (0.0f), NodeOrt_Z, NodeOrt_Y, NodeTransformSpace_World);

  camera->BindToScene (scene, NodeBindMode_AddRef);

  screen.SetBackgroundColor (0.f, 0.05f, 0.2f, 0);

  render::Viewport viewport;

  viewport.SetArea       (0, 0, 100, 100);
  viewport.SetCamera     (&*camera);
  viewport.SetRenderPath ("ModelerRender");

  screen.Attach (viewport);

  engine::AttachmentRegistry::Register<render::Screen> (SCREEN_ATTACHMENT_NAME, screen);
}

void MyApplicationServer::ExecuteCommand (const char* command)
{
  if (!xtl::xstrcmp (command, "close"))
  {
    syslib::Application::Exit (0);
    return;
  }

  try
  {
    shell.Execute (command);
  }
  catch (std::exception& exception)
  {
    common::Console::Print (exception.what ());
  }
}

void MyApplicationServer::OpenProjectPath (const char* path)
{
  static const char* METHOD_NAME = "MyApplicationServer::OpenProjectPath";

  if (!path)
    throw xtl::make_null_argument_exception (METHOD_NAME, "path");

  stl::string model_name = common::format ("%s%s", path, MODEL_FILE_NAME);

  if (!common::FileSystem::IsFileExist (model_name.c_str ()))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't open path '%s', no model data file", path);

  LoadModel (model_name.c_str ());

  UnloadModels ();

  project_path = path;

  stl::string precomputed_trajectories_mask = project_path + "trajectory*.binmesh";

  common::FileList precomputed_trajectories = common::FileSystem::Search (precomputed_trajectories_mask.c_str (), common::FileSearch_Files);

  for (size_t i = 0, size = precomputed_trajectories.Size (); i < size; i++)
    try
    {
      LoadTrajectory (precomputed_trajectories.Item (i).name);
    }
    catch (std::exception& exception)
    {
      common::Console::Printf ("Can't load trajectory from file '%s', exception '%s'\n", precomputed_trajectories.Item (i).name, exception.what ());
    }
    catch (...)
    {
      common::Console::Printf ("Can't load trajectory from file '%s', unknown exception\n", precomputed_trajectories.Item (i).name);
    }

  try
  {
    LoadEnvelope ();
  }
  catch (...)
  {
    try
    {
      CalculateEnvelope (200);
    }
    catch (std::exception& exception)
    {
      common::Console::Printf ("Can't calculate envelope, exception '%s'\n", exception.what ());
    }
    catch (...)
    {
      common::Console::Printf ("Can't calculate envelope, unknown exception\n");
    }
  }
}

void MyApplicationServer::UnloadEnvelope ()
{
  if (envelope.visual_model)
  {
    envelope.visual_model->Unbind ();
    envelope.visual_model = 0;

    envelope.resource_binding.Unload ();
  }
}

void MyApplicationServer::UnloadModels ()
{
  UnloadEnvelope ();

  for (VisualModels::iterator iter = trajectories.begin (), end = trajectories.end (); iter != end; ++iter)
  {
    iter->second.visual_model->Unbind ();
    iter->second.resource_binding.Unload ();
  }

  trajectories.clear ();
}

void MyApplicationServer::Cleanup ()
{
  UnloadModels ();

  stl::string meshes_to_delete_mask = project_path + "*.*mesh";
  remove_files (meshes_to_delete_mask.c_str ());

  stl::string desc_to_delete_mask = project_path + "*.*desc";
  remove_files (desc_to_delete_mask.c_str ());
}

void MyApplicationServer::LoadTrajectory (const char* file_name)
{
  if (!common::FileSystem::IsFileExist (file_name))
    throw xtl::format_operation_exception ("MyApplicationServer::LoadTrajectory",
                                           "Can't load trajectory '%s', no such file", file_name);

  media::rms::Group resource_group;

  resource_group.Add (file_name);

  VisualModelResource new_trajectory;

  new_trajectory.resource_binding = resource_manager.CreateBinding (resource_group);
  new_trajectory.resource_binding.Load ();

  VisualModels::iterator iter = trajectories.find (file_name);

  if (iter != trajectories.end ())
  {
    iter->second.visual_model->Unbind ();
    iter->second.resource_binding.Unload ();
    trajectories.erase (iter);
  }

  new_trajectory.visual_model = VisualModel::Create ();

  new_trajectory.visual_model->SetMeshName (file_name);
  new_trajectory.visual_model->BindToScene (scene, NodeBindMode_AddRef);
  new_trajectory.visual_model->SetName     ("Trajectory");
  new_trajectory.visual_model->Scale       (-2.f, 2.f, 2.f);

  trajectories[file_name] = new_trajectory;

  common::Console::Printf ("%u trajectories loaded.\n", trajectories.size ());
}

void MyApplicationServer::OnNewXmeshTrajectory (const char* desc_trajectory_file_name)
{
  static const char* METHOD_NAME = "MyApplicationServer::OnNewXmeshEnvelope";

  common::InputFile desc_file (desc_trajectory_file_name);

  size_t desc_file_data;

  desc_file.Read (&desc_file_data, sizeof (desc_file_data));

  stl::string xmesh_trajectory_file_name (desc_trajectory_file_name, xtl::xstrlen (desc_trajectory_file_name) - xtl::xstrlen (DESC_FILE_SUFFIX));

  if (!desc_file_data || !common::FileSystem::IsFileExist (xmesh_trajectory_file_name.c_str ()) ||
      (common::FileSystem::GetFileSize (xmesh_trajectory_file_name.c_str ()) != desc_file_data))
    throw xtl::format_operation_exception (METHOD_NAME, "There was an error while calculating trajectory %s",
                                           xmesh_trajectory_file_name.c_str ());

  stl::string application_name  = common::format ("%s\\%s%s", working_directory.c_str (), win32_plugin_path.c_str (), MESH_CONVERTER_APPLICATION_NAME),
              binmesh_file_name = common::format ("%.*sbinmesh", xmesh_trajectory_file_name.length () - xtl::xstrlen ("xmesh"),
                                                  xmesh_trajectory_file_name.c_str ());

  if (_spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), xmesh_trajectory_file_name.c_str (), binmesh_file_name.c_str (), 0) == -1)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't call %s: %s", MESH_CONVERTER_APPLICATION_NAME, get_spawn_error_name ());

  WaitForFile (binmesh_file_name.c_str (), xtl::bind (&MyApplicationServer::LoadTrajectory, this, _1));
}

void MyApplicationServer::CalculateTrajectory (double nu1, double nu2, double nu3, size_t lod)
{
  stl::string application_name        = common::format ("%s\\%s%s", working_directory.c_str (), win32_plugin_path.c_str (), WIN32_TRAJECTORY_APPLICATION_NAME),
              model_name              = common::format ("%smodel.dat", project_path.c_str ()),
              trajectory_name         = common::format ("%strajectory_%g_%g_%g.xmesh", project_path.c_str (), nu1, nu2, nu3),
              binmesh_trajectory_name = common::format ("%strajectory_%g_%g_%g.binmesh", project_path.c_str (), nu1, nu2, nu3),
              nu1_string              = common::format ("%g", nu1),
              nu2_string              = common::format ("%g", nu2),
              nu3_string              = common::format ("%g", nu3),
              lod_string              = common::format ("%u", lod),
              waited_desc_file_name   = trajectory_name + DESC_FILE_SUFFIX;

  common::FileSystem::Remove (trajectory_name.c_str ());
  common::FileSystem::Remove (binmesh_trajectory_name.c_str ());
  common::FileSystem::Remove (waited_desc_file_name.c_str ());

  if (_spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), "args-input", nu1_string.c_str (),
        nu2_string.c_str (), nu3_string.c_str (), model_name.c_str (), trajectory_name.c_str (), lod_string.c_str (), 0) == -1)
    throw xtl::format_operation_exception ("MyApplicationServer::CalculateTrajectory", "Can't call %s: %s", WIN32_TRAJECTORY_APPLICATION_NAME, get_spawn_error_name ());

  WaitForFile (waited_desc_file_name.c_str (), xtl::bind (&MyApplicationServer::OnNewXmeshTrajectory, this, _1));
}

void MyApplicationServer::LoadEnvelope ()
{
  UnloadEnvelope ();

  stl::string mesh_name = project_path + "envelope.binmesh";

  media::rms::Group resource_group;

  resource_group.Add (mesh_name.c_str ());

  envelope.resource_binding = resource_manager.CreateBinding (resource_group);
  envelope.resource_binding.Load ();

  if (!common::FileSystem::IsFileExist (mesh_name.c_str ()))
    throw xtl::format_operation_exception ("MyApplicationServer::LoadEnvelope", "Can't load envelope '%s', no such file", mesh_name.c_str ());

  envelope.visual_model = VisualModel::Create ();

  envelope.visual_model->SetMeshName (mesh_name.c_str ());
  envelope.visual_model->BindToScene (scene, NodeBindMode_AddRef);
  envelope.visual_model->SetName     ("Envelope");
}

void MyApplicationServer::OnNewXmeshEnvelope (const char* desc_envelope_file_name)
{
  static const char* METHOD_NAME = "MyApplicationServer::OnNewXmeshEnvelope";

  common::InputFile desc_file (desc_envelope_file_name);

  size_t desc_file_data;

  desc_file.Read (&desc_file_data, sizeof (desc_file_data));

  stl::string xmesh_envelope_file_name (desc_envelope_file_name, xtl::xstrlen (desc_envelope_file_name) - xtl::xstrlen (DESC_FILE_SUFFIX));

  if (!desc_file_data || !common::FileSystem::IsFileExist (xmesh_envelope_file_name.c_str ()) ||
      (common::FileSystem::GetFileSize (xmesh_envelope_file_name.c_str ()) != desc_file_data))
    throw xtl::format_operation_exception (METHOD_NAME, "There was an error while calculating envelope");

  stl::string application_name  = common::format ("%s\\%s%s", working_directory.c_str (), win32_plugin_path.c_str (), MESH_CONVERTER_APPLICATION_NAME),
              binmesh_file_name = common::format ("%senvelope.binmesh", project_path.c_str ());

  if (_spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), xmesh_envelope_file_name.c_str (), binmesh_file_name.c_str (), 0) == -1)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't call %s: %s", MESH_CONVERTER_APPLICATION_NAME, get_spawn_error_name ());

  WaitForFile (binmesh_file_name.c_str (), xtl::bind (&MyApplicationServer::LoadEnvelope, this));
}

void MyApplicationServer::CalculateEnvelope (size_t lod)
{ //????Продумать двойной вызов
  stl::string application_name      = common::format ("%s\\%s%s", working_directory.c_str (), win32_plugin_path.c_str (), WIN32_ENVELOPE_APPLICATION_NAME),
              model_name            = common::format ("%smodel.dat", project_path.c_str ()),
              envelope_name         = common::format ("%senvelope.xmesh", project_path.c_str ()),
              binmesh_envelope_name = common::format ("%senvelope.binmesh", project_path.c_str ()),
              lod_string            = common::format ("%u", lod),
              waited_desc_file_name = envelope_name + DESC_FILE_SUFFIX;

  common::FileSystem::Remove (envelope_name.c_str ());
  common::FileSystem::Remove (binmesh_envelope_name.c_str ());
  common::FileSystem::Remove (waited_desc_file_name.c_str ());

  if (_spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), model_name.c_str (), envelope_name.c_str (), lod_string.c_str (), 0) == -1)
    throw xtl::format_operation_exception ("MyApplicationServer::CalculateEnvelope", "Can't call %s: %s", WIN32_ENVELOPE_APPLICATION_NAME, get_spawn_error_name ());

  WaitForFile (waited_desc_file_name.c_str (), xtl::bind (&MyApplicationServer::OnNewXmeshEnvelope, this, _1));
}

void MyApplicationServer::OnNewCondorBatchTrajectory (const char* desc_trajectory_file_name)
{
  static const char* METHOD_NAME = "MyApplicationServer::OnNewCondorBatchTrajectory";

  CondorTrajectoriesNames::iterator iter = condor_trajectories_names.find (desc_trajectory_file_name);

  if (iter == condor_trajectories_names.end ())
    throw xtl::format_operation_exception (METHOD_NAME, "Trajectory '%s' was not waited.", desc_trajectory_file_name);

  stl::string xmesh_trajectory_file_name (desc_trajectory_file_name, xtl::xstrlen (desc_trajectory_file_name) - xtl::xstrlen (DESC_FILE_SUFFIX)),
              new_xmesh_trajectory_file_name (iter->second, 0, iter->second.length () - xtl::xstrlen (DESC_FILE_SUFFIX));

  common::FileSystem::Rename (desc_trajectory_file_name, iter->second.c_str ());
  common::FileSystem::Rename (xmesh_trajectory_file_name.c_str (), new_xmesh_trajectory_file_name.c_str ());

  OnNewXmeshTrajectory (iter->second.c_str ());

  condor_trajectories_names.erase (iter);

  if (condor_trajectories_names.empty ())
    common::Console::Printf ("Trajectories calculation complete.\n");
}

void MyApplicationServer::OnNewTrajectoriesCoords (const char* desc_file_name, size_t lod)
{
  static const char* METHOD_NAME = "MyApplicationServer::OnNewTrajectoriesCoords";

  common::InputFile desc_file (desc_file_name);

  size_t desc_file_data;

  desc_file.Read (&desc_file_data, sizeof (desc_file_data));

  stl::string trajectories_coords_file_name (desc_file_name, xtl::xstrlen (desc_file_name) - xtl::xstrlen (DESC_FILE_SUFFIX));

  if (!desc_file_data || !common::FileSystem::IsFileExist (trajectories_coords_file_name.c_str ()) ||
      (common::FileSystem::GetFileSize (trajectories_coords_file_name.c_str ()) != desc_file_data))
    throw xtl::format_operation_exception (METHOD_NAME, "There was an error while calculating trajectories coordinates for batch processing");

  common::InputFile trajectories_coords_file (trajectories_coords_file_name.c_str ());

  //проверка заголовка файла

  int header;

  file_read (METHOD_NAME, trajectories_coords_file, &header, sizeof (header));

  if (header != TRAJECTORIES_COORDS_HEADER)
    throw xtl::format_operation_exception (METHOD_NAME, "File '%s' has invalid header '%c%c%c%c', must be '%c%c%c%c'", trajectories_coords_file_name,
      ((char*)(&header))[3], ((char*)(&header))[2], ((char*)(&header))[1], ((char*)(&header))[0], ((char*)(&TRAJECTORIES_COORDS_HEADER))[3],
      ((char*)(&TRAJECTORIES_COORDS_HEADER))[2], ((char*)(&TRAJECTORIES_COORDS_HEADER))[1], ((char*)(&TRAJECTORIES_COORDS_HEADER))[0]);

  int version;

  file_read (METHOD_NAME, trajectories_coords_file, &version, sizeof (version));

  if (version != TRAJECTORIES_COORDS_VERSION)
    throw xtl::format_operation_exception (METHOD_NAME, "File '%s' has unsupported version %d, supported version - %d", trajectories_coords_file_name,
                                           version, TRAJECTORIES_COORDS_VERSION);

  size_t coords_count;

  file_read (METHOD_NAME, trajectories_coords_file, &coords_count, sizeof (coords_count));

  if (condor_path.empty () || !use_condor)
  {
    common::Console::Printf ("Calculating %u trajectories...\n", coords_count);

    for (size_t i = 0; i < coords_count; i++)
    {
      float nu1, nu2, nu3;

      file_read (METHOD_NAME, trajectories_coords_file, &nu1, sizeof (nu1));
      file_read (METHOD_NAME, trajectories_coords_file, &nu2, sizeof (nu2));
      file_read (METHOD_NAME, trajectories_coords_file, &nu3, sizeof (nu3));

      CalculateTrajectory (nu1, nu2, nu3, lod);
    }

    return;
  }

  if (!condor_path.empty () && use_condor)
  {
    common::Console::Printf ("Calculating %u trajectories with Condor...\n", coords_count);

    if (!common::FileSystem::IsDir (BATCH_TRAJECTORY_NU_FILE_FOLDER)) //????Очищать папку по завершении рассчётов
      common::FileSystem::Mkdir (BATCH_TRAJECTORY_NU_FILE_FOLDER);

    for (size_t i = 0; i < coords_count; i++)
    {
      float nu1, nu2, nu3;

      file_read (METHOD_NAME, trajectories_coords_file, &nu1, sizeof (nu1));
      file_read (METHOD_NAME, trajectories_coords_file, &nu2, sizeof (nu2));
      file_read (METHOD_NAME, trajectories_coords_file, &nu3, sizeof (nu3));

      stl::string waited_file_name  = common::format ("batch_trajectory.%u.xmesh.desc", i);

      condor_trajectories_names[waited_file_name.c_str ()] = common::format ("%strajectory_%f_%f_%f.xmesh.desc", project_path.c_str (), nu1, nu2, nu3);

      WaitForFile (waited_file_name.c_str (), xtl::bind (&MyApplicationServer::OnNewCondorBatchTrajectory, this, _1));

      stl::string nu_file_name = common::format ("%s\\%s.%d%s", BATCH_TRAJECTORY_NU_FILE_FOLDER, BATCH_TRAJECTORY_NU_FILE_BASE_NAME, i,
                                                 BATCH_TRAJECTORY_NU_FILE_SUFFIX);

      //????Добавить обработку ошибок сохранения файла

      FILE* nu_file = fopen (nu_file_name.c_str (), "w");

      fprintf (nu_file, "%f %f %f", nu1, nu2, nu3);

      fclose (nu_file);
    }

    stl::string condor_config_file_name = common::format ("%s%s", project_path.c_str (), CONDOR_CONFIG_FILE_NAME);

    FILE* condor_config_file = fopen (condor_config_file_name.c_str (), "w");

    if (!condor_config_file)
      throw xtl::format_operation_exception ("Can't open condor config file '%s'\n", condor_config_file_name.c_str ());

    fprintf (condor_config_file, "Executable = %s\\%s.$$(OpSys).$$(Arch)\n", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME);
    fprintf (condor_config_file, "Log = %s%s\n", project_path.c_str (), BATCH_TRAJECTORY_LOG_FILE_NAME);
    fprintf (condor_config_file, "Arguments = text-file-input %s.$(Process)%s %s batch_trajectory.$(Process).xmesh %u\n",
        BATCH_TRAJECTORY_NU_FILE_BASE_NAME, BATCH_TRAJECTORY_NU_FILE_SUFFIX, OLD_FORMAT_MODEL_FILE_NAME, lod);
//    fprintf (condor_config_file, "output = %sbatch_trajectory.$(Process).xmesh.output\n", project_path.c_str ());
    fprintf (condor_config_file, "transfer_input_files = %s%s, %s%s, %s\\%s.$(Process)%s\n", project_path.c_str (),
             BATCH_TRAJECTORIES_COORDS_FILE_NAME, project_path.c_str (), OLD_FORMAT_MODEL_FILE_NAME, BATCH_TRAJECTORY_NU_FILE_FOLDER,
             BATCH_TRAJECTORY_NU_FILE_BASE_NAME, BATCH_TRAJECTORY_NU_FILE_SUFFIX);
    fprintf (condor_config_file, "Universe = Vanilla\n");
    fprintf (condor_config_file, "Requirements = %s && Disk >= %d\n", condor_trajectory_requirements.c_str (), (int)(lod * 2.f * 45.f / 1024.f + 1));
    fprintf (condor_config_file, "Rank = kflops\n");
    fprintf (condor_config_file, "should_transfer_files = YES\n");
    fprintf (condor_config_file, "when_to_transfer_output = ON_EXIT\n");
    fprintf (condor_config_file, "Queue %u\n", coords_count);

    fclose (condor_config_file);

    stl::string application_name = common::format ("%s%s", condor_path.c_str (), CONDOR_SUBMIT_APPLICATION_NAME);

    if (_spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), condor_config_file_name.c_str (), 0) == -1)
      throw xtl::format_operation_exception ("MyApplicationServer::OnNewTrajectoriesCoords",
                                             "Can't call %s: %s", CONDOR_SUBMIT_APPLICATION_NAME, get_spawn_error_name ());
  }
}

void MyApplicationServer::CalculateTrajectories (size_t trajectories_count, size_t lod)
{ //????Продумать двойной вызов
  stl::string application_name          = common::format ("%s\\%s%s", working_directory.c_str (), win32_plugin_path.c_str (), WIN32_ENVELOPE_APPLICATION_NAME),
              model_name                = common::format ("%smodel.dat", project_path.c_str ()),
              trajectories_coords_name  = common::format ("%s%s", project_path.c_str (), BATCH_TRAJECTORIES_COORDS_FILE_NAME),
              trajectories_count_string = common::format ("%u", trajectories_count),
              waited_desc_file_name     = trajectories_coords_name + DESC_FILE_SUFFIX;

  common::FileSystem::Remove (trajectories_coords_name.c_str ());
  common::FileSystem::Remove (waited_desc_file_name.c_str ());

  if (_spawnl (_P_NOWAIT, application_name.c_str (), application_name.c_str (), "-save_coords_only", model_name.c_str (),
         trajectories_coords_name.c_str (), trajectories_count_string.c_str (), 0) == -1)
    throw xtl::format_operation_exception ("MyApplicationServer::CalculateTrajectories",
                                           "Can't call %s: %s", WIN32_ENVELOPE_APPLICATION_NAME, get_spawn_error_name ());

  WaitForFile (waited_desc_file_name.c_str (), xtl::bind (&MyApplicationServer::OnNewTrajectoriesCoords, this, _1, lod));
}

void MyApplicationServer::CheckNewFiles ()
{
  for (WaitedFiles::iterator iter = waited_files.begin (); iter != waited_files.end (); ++iter)
  {
    if (!common::FileSystem::IsFileExist (iter->file_name.c_str ()))
      continue;

    size_t file_size = common::FileSystem::GetFileSize (iter->file_name.c_str ());

    if (file_size != iter->current_file_size)
    {
      iter->current_file_size = file_size;
      continue;
    }

    WaitedFiles::iterator next = iter;

    ++next;

    try
    {
      iter->handler (iter->file_name.c_str ());
    }
    catch (std::exception& exception)
    {
      common::Console::Printf ("Exception '%s' while calling handler for file %s\n", exception.what (), iter->file_name.c_str ());
    }
    catch (...)
    {
      common::Console::Printf ("Unknown exception while calling handler for file %s\n", iter->file_name.c_str ());
    }

    waited_files.erase (iter);
    iter = next;
  }

  if (waited_files.empty ())
    wait_files_timer.Pause ();
}

void MyApplicationServer::WaitForFile (const char* file_name, const WaitFileHandler& handler)
{
  waited_files.push_back (WaitedFile (file_name, handler));
  wait_files_timer.Run ();
}

void MyApplicationServer::LoadModel (const char* path)
{
  static const char* METHOD_NAME = "MyApplicationServer::LoadModel";

  common::Parser   parser     (path);
  common::ParseLog parser_log (parser.Log ());

  for (size_t i = 0; i < parser_log.MessagesCount (); i++)
  {
    common::ParseLogMessageType message_type = parser_log.MessageType (i);

    if (message_type == common::ParseLogMessageType_Error || message_type == common::ParseLogMessageType_FatalError)
      throw xtl::format_operation_exception (METHOD_NAME, "Model parse error: '%s'", parser_log.Message (i));
  }

  double  A, B, C;     //момент инерции A,B,C
  double  h, g;        //постоянные интегрирования h,g
  double  mx,my,mz;    //координаты центра тяжести
  int     ini;         //???

  common::VarRegistry model_registry (MODEL_REGISTRY_NAME);

  try
  {
    common::ParseIterator root = parser.Root ().First ("Model");

    ini = common::get<int> (*root, "ini");
    model_registry.SetValue ("ini", xtl::any (stl::string (common::get<const char*> (*root, "ini")), true));
    model_registry.SetValue ("Author", xtl::any (stl::string (common::get<const char*> (*root, "Author", "")), true));
    model_registry.SetValue ("Description", xtl::any (stl::string (common::get<const char*> (*root, "Description", "")), true));

    common::ParseIterator iter = root->First ("MomentOfInertia");

    A = common::get<double> (*iter, "A");
    model_registry.SetValue ("A", xtl::any (stl::string (common::get<const char*> (*iter, "A")), true));
    B = common::get<double> (*iter, "B");
    model_registry.SetValue ("B", xtl::any (stl::string (common::get<const char*> (*iter, "B")), true));
    C = common::get<double> (*iter, "C");
    model_registry.SetValue ("C", xtl::any (stl::string (common::get<const char*> (*iter, "C")), true));

    iter = root->First ("IntegrationConstant");

    h = common::get<double> (*iter, "h");
    model_registry.SetValue ("h", xtl::any (stl::string (common::get<const char*> (*iter, "h")), true));
    g = common::get<double> (*iter, "g");
    model_registry.SetValue ("g", xtl::any (stl::string (common::get<const char*> (*iter, "g")), true));

    iter = root->First ("CenterOfMass");

    mx = common::get<double> (*iter, "mx");
    model_registry.SetValue ("mx", xtl::any (stl::string (common::get<const char*> (*iter, "mx")), true));
    my = common::get<double> (*iter, "my");
    model_registry.SetValue ("my", xtl::any (stl::string (common::get<const char*> (*iter, "my")), true));
    mz = common::get<double> (*iter, "mz");
    model_registry.SetValue ("mz", xtl::any (stl::string (common::get<const char*> (*iter, "mz")), true));

    stl::string old_format_model_file_name (path, xtl::xstrlen (path) - xtl::xstrlen (MODEL_FILE_NAME));

    old_format_model_file_name += "model.dat";

    common::OutputFile old_format_model_file (old_format_model_file_name.c_str ());

    stl::string old_format_model_data = common::format ("A=%lf, B=%lf, C=%lf, h=%lf, g=%lf, mx=%lf, my=%lf, mz=%lf, ini=%d",
                                                        A, B, C, h, g, mx, my, mz, ini);

    old_format_model_file.Write (old_format_model_data.c_str (), old_format_model_data.length () + 1);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

void MyApplicationServer::SaveModel ()
{
  double  A, B, C;     //момент инерции A,B,C
  double  h, g;        //постоянные интегрирования h,g
  double  mx,my,mz;    //координаты центра тяжести
  int     ini;         //???

  common::VarRegistry model_registry (MODEL_REGISTRY_NAME);

  A   = atof (to_string (model_registry.GetValue ("A")).c_str ());
  B   = atof (to_string (model_registry.GetValue ("B")).c_str ());
  C   = atof (to_string (model_registry.GetValue ("C")).c_str ());
  h   = atof (to_string (model_registry.GetValue ("h")).c_str ());
  g   = atof (to_string (model_registry.GetValue ("g")).c_str ());
  mx  = atof (to_string (model_registry.GetValue ("mx")).c_str ());
  my  = atof (to_string (model_registry.GetValue ("my")).c_str ());
  mz  = atof (to_string (model_registry.GetValue ("mz")).c_str ());
  ini = atoi (to_string (model_registry.GetValue ("ini")).c_str ());

  stl::string model_file_name = project_path + MODEL_FILE_NAME;

  {
    common::XmlWriter writer (model_file_name.c_str ());

    common::XmlWriter::Scope root_scope (writer, "Model");

    writer.WriteAttribute ("ini", ini);
    writer.WriteAttribute ("Author", to_string (model_registry.GetValue ("Author")).c_str ());
    writer.WriteAttribute ("Description", to_string (model_registry.GetValue ("Description")).c_str ());

    {
      common::XmlWriter::Scope scope (writer, "MomentOfInertia");

      writer.WriteAttribute ("A", A);
      writer.WriteAttribute ("B", B);
      writer.WriteAttribute ("C", C);
    }

    {
      common::XmlWriter::Scope scope (writer, "IntegrationConstant");

      writer.WriteAttribute ("h", h);
      writer.WriteAttribute ("g", g);
    }

    {
      common::XmlWriter::Scope scope (writer, "CenterOfMass");

      writer.WriteAttribute ("mx", mx);
      writer.WriteAttribute ("my", my);
      writer.WriteAttribute ("mz", mz);
    }
  }

  Cleanup ();
  OpenProjectPath (project_path.c_str ());
}

void MyApplicationServer::CreateCondorBinaries ()
{
  try
  {
    if (!common::FileSystem::IsDir (CONDOR_BINARIES_PATH))
      common::FileSystem::Mkdir (CONDOR_BINARIES_PATH);

    stl::string win32_trajectory_application_name = common::format ("%s\\%s%s", working_directory.c_str (), win32_plugin_path.c_str (), WIN32_TRAJECTORY_APPLICATION_NAME),
                win_5_0_intel_trajectory          = common::format ("%s\\%s.WINNT50.INTEL", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME),
                win_5_1_intel_trajectory          = common::format ("%s\\%s.WINNT51.INTEL", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME),
                win_5_2_intel_trajectory          = common::format ("%s\\%s.WINNT52.INTEL", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME),
                win_6_0_intel_trajectory          = common::format ("%s\\%s.WINNT60.INTEL", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME),
                win_5_0_X86_64_trajectory         = common::format ("%s\\%s.WINNT50.X86_64", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME),
                win_5_1_X86_64_trajectory         = common::format ("%s\\%s.WINNT51.X86_64", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME),
                win_5_2_X86_64_trajectory         = common::format ("%s\\%s.WINNT52.X86_64", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME),
                win_6_0_X86_64_trajectory         = common::format ("%s\\%s.WINNT60.X86_64", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME);

    copy_file (win32_trajectory_application_name.c_str (), win_5_0_intel_trajectory.c_str ());
    copy_file (win32_trajectory_application_name.c_str (), win_5_1_intel_trajectory.c_str ());
    copy_file (win32_trajectory_application_name.c_str (), win_5_2_intel_trajectory.c_str ());
    copy_file (win32_trajectory_application_name.c_str (), win_6_0_intel_trajectory.c_str ());
    copy_file (win32_trajectory_application_name.c_str (), win_5_0_X86_64_trajectory.c_str ());
    copy_file (win32_trajectory_application_name.c_str (), win_5_1_X86_64_trajectory.c_str ());
    copy_file (win32_trajectory_application_name.c_str (), win_5_2_X86_64_trajectory.c_str ());
    copy_file (win32_trajectory_application_name.c_str (), win_6_0_X86_64_trajectory.c_str ());

    condor_trajectory_requirements = "((Arch == \"X86_64\" || Arch == \"INTEL\") && (OpSys == \"WINNT50\" || OpSys == \"WINNT51\" || OpSys == \"WINNT52\" || OpSys == \"WINNT60\"))";

    if (!osx_plugin_path.empty ())
    {
      stl::string osx_trajectory_application_name = common::format ("%s\\%s%s", working_directory.c_str (), osx_plugin_path.c_str (), OSX_TRAJECTORY_APPLICATION_NAME),
                  osx_intel_trajectory            = common::format ("%s\\%s.OSX.INTEL", CONDOR_BINARIES_PATH, TRAJECTORY_APPLICATION_BASE_NAME);

      if (common::FileSystem::IsFileExist (osx_trajectory_application_name.c_str ()))
      {
        copy_file (osx_trajectory_application_name.c_str (), osx_intel_trajectory.c_str ());

        condor_trajectory_requirements += " || (Arch == \"INTEL\" && OpSys == \"OSX\")";
      }
      else
      {
        printf ("Condor will not perform calculations on OSX machines, there is no OSX executable '%s' in specified directory '%s'\n",
                osx_trajectory_application_name.c_str (), osx_plugin_path.c_str ());
      }
    }
  }
  catch (xtl::exception& exception)
  {
    use_condor = 0;
    printf ("Condor will not be used, exception while creating needed binaries: '%s'\n", exception.what ());
  }
  catch (...)
  {
    use_condor = 0;
    printf ("Condor will not be used, unknown exception while creating needed binaries\n");
  }
}
