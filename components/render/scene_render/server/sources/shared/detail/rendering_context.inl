inline RenderingContext::RenderingContext (manager::Frame& in_frame, server::RenderManager& in_render_manager, ITraverseResultCache& in_traverse_result_cache)
  : frame (in_frame)
  , render_manager (in_render_manager)
  , traverse_result ()
  , traverse_result_cache (in_traverse_result_cache)
{
}

inline server::RenderManager& RenderingContext::RenderManager ()
{
  return render_manager;
}

inline server::TraverseResult& RenderingContext::TraverseResult ()
{
  if (traverse_result)
    return *traverse_result;

  try
  {
    traverse_result = &traverse_result_cache.Result ();

    return *traverse_result;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::RenderingContext::TraverseResult");
    throw;
  }
}
