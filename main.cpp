#include "rapidcsv.h"
#include "zlib.h"
#include <utility>

std::pair<char*, int> compress(std::string uncompressed) {
  int len = uncompressed.length();
  char* compressed = (char*) malloc(len*2 + 1);

  z_stream strm = {0};
  strm.avail_in = (uInt)len;
  strm.next_in = (Bytef *)uncompressed.c_str();
  strm.avail_out = (uInt)len*2;
  strm.next_out = (Bytef *)compressed;
  deflateInit(&strm, Z_BEST_COMPRESSION);
  int result = deflate(&strm, Z_FINISH);
  assert(result == Z_STREAM_END && "Probably not enough output buffer was allocated");
  deflateEnd(&strm);

  return std::make_pair(compressed, (int)strm.total_out);
}

std::pair<char*, int> decompress(char* compressed, int len) {
  char* uncompressed = (char*) malloc((len-1)/2);

  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = (uInt)len;
  strm.next_in = (Bytef *)compressed; // input char array
  strm.avail_out = (uInt)((len-1)/2); // size of output
  strm.next_out = (Bytef *)uncompressed; // output char array
    
  inflateInit(&strm);
  inflate(&strm, Z_NO_FLUSH);
  inflateEnd(&strm);
  
  return std::make_pair(uncompressed, (int)strm.total_out);

}

int main() {
  rapidcsv::Document doc("datasets/agnews_train.csv");

  std::vector<std::string> class_indices = doc.GetColumn<std::string>("Class Index");
  std::vector<std::string> titles = doc.GetColumn<std::string>("Title");
  std::vector<std::string> descriptions = doc.GetColumn<std::string>("Description");

  for (int i=0; i<class_indices.size(); ++i) {
    std::pair<char*, int> compression = compress(titles[i]+descriptions[i]);
    auto& [compressed, len] = compression;
    std::cout << compressed << " " << len << std::endl;
  }
  
  std::cout << class_indices.size() << std::endl;
}