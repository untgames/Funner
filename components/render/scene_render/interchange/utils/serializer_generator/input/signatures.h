#section ClientToServer

void LoadResource (const char* name);
void UnloadResource (const char* name);

void CreateViewport (uint32 id);
void SetViewportArea (uint32 id, int8 left, int8 top, int8 width, int8 height);
void SetViewportZOrder (uint32 id, int8 zorder);
void SetViewportActive (uint32 id, bool8 is_active);
void SetViewportName (uint32 id, const char* name);
void SetViewportTechnique (uint32 id, const char* name);
void SetViewportBackground (uint32 id, bool8 state, const math::vec4f& color);
void DestroyViewport (uint32 id);

void CreateRenderTarget (uint32 id, const char* name);
void DestroyRenderTarget (uint32 id);
void SetRenderTargetActive (uint32 id, bool8 active);
void SetRenderTargetScreenArea (uint32 id, int8 left, int8 top, int8 width, int8 height);
void SetRenderTargetBackground (uint32 id, bool8 state, const math::vec4f& color);
void AttachViewportToRenderTarget (uint32 render_target_id, uint32 viewport_id);
void DetachViewportFromRenderTarget (uint32 render_target_id, uint32 viewport_id);
void UpdateRenderTarget (uint32 id);

#section ServerToClient

void Dummy();