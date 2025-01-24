
#include "IReader.h"
namespace ps {

class PcapReader : public IPacketReader {
public:
  PcapReader(std::string);
  void Read(std::shared_ptr<IPacketWriter>) override;

  void setInputFileName(std::string);

private:
  std::string m_inputFileName;
};

} // namespace ps
