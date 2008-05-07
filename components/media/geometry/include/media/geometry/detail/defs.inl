/*
    VertexWeight
*/

inline VertexWeight::VertexWeight ()
  : joint_index (0), joint_weight (0.0f)
  {}

inline VertexWeight::VertexWeight (size_t in_joint_index, float in_joint_weight)
  : joint_index (in_joint_index), joint_weight (in_joint_weight)
  {}

/*
    VertexInfluence
*/

inline VertexInfluence::VertexInfluence ()
  : first_weight (0), weights_count (0)
  {}

inline VertexInfluence::VertexInfluence (size_t in_first_weight, size_t in_weights_count)
  : first_weight (in_first_weight), weights_count (in_weights_count)
  {}
