inline ContextLock::ContextLock (ContextState& in_context_state)
  : context_state (&in_context_state)
{
  context_state->GetContext ().Lock ();
}

inline ContextLock::~ContextLock ()
{
  context_state->GetContext ().Unlock ();
}
