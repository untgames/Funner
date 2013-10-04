#section ClientToServer

void LoadResource (const char* name);
void UnloadResource (const char* name);

void SetViewportArea (object_id_t id, int32 left, int32 top, int32 width, int32 height);
void SetViewportZOrder (object_id_t id, int32 zorder);
void SetViewportActive (object_id_t id, bool8 is_active);
void SetViewportName (object_id_t id, const char* name);
void SetViewportTechnique (object_id_t id, const char* name);
void SetViewportBackground (object_id_t id, bool8 state, const math::vec4f& color);
void SetViewportProperties (object_id_t id, uint64 properties_id);

void CreateRenderTarget (object_id_t id, const char* name, const char* init_string);
void DestroyRenderTarget (object_id_t id);
void SetRenderTargetActive (object_id_t id, bool8 active);
void SetRenderTargetScreenArea (object_id_t id, int32 left, int32 top, int32 width, int32 height);
void SetRenderTargetBackground (object_id_t id, bool8 state, const math::vec4f& color);
void AttachViewportToRenderTarget (object_id_t id, object_id_t viewport_id);
void DetachViewportFromRenderTarget (object_id_t id, object_id_t viewport_id);
void UpdateRenderTarget (object_id_t id);

void UpdatePropertyMap (...);
void RemovePropertyMap (object_id_t id);
void RemovePropertyLayout (object_id_t id);

#section ServerToClient

void UpdatePropertyMap (...);
void RemovePropertyMap (object_id_t id);
void RemovePropertyLayout (object_id_t id);
