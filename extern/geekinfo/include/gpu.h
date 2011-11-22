#ifndef GEEKINFO_GPU_H__
#define GEEKINFO_GPU_H__

#include <string>

namespace geekinfo {
  
class GPU {
public:
  GPU();
  
  const std::string& get_renderer() const { return m_renderer; }
  const std::string& get_vendor() const { return m_vendor; }
  const std::string& get_version() const { return m_version; }
  const std::string& get_extensions() const { return m_extensions; }
  
protected:
  void gather_information();
  
  std::string m_renderer;
  std::string m_vendor;
  std::string m_version;
  std::string m_extensions;
};

} // namespace geekinfo

#endif // GEEKINFO_GPU_H__

