#include "rapidcsv.h"
#include "zlib.h"
#include <iterator>
#include <utility>

std::pair<char *, int> compress(std::string uncompressed) {
  int len = uncompressed.length();
  char *compressed = (char *)malloc(len * 2 + 1);

  z_stream strm = {0};
  strm.avail_in = (uInt)len;
  strm.next_in = (Bytef *)uncompressed.c_str();
  strm.avail_out = (uInt)len * 2;
  strm.next_out = (Bytef *)compressed;

  deflateInit(&strm, Z_BEST_COMPRESSION);
  deflate(&strm, Z_FINISH);
  deflateEnd(&strm);

  return std::make_pair(compressed, (int)strm.total_out);
}

int main() {
  rapidcsv::Document doc("datasets/agnews_train.csv");
  int n = doc.GetRowCount();
  std::vector<std::string> titles = doc.GetColumn<std::string>("Title");
  std::vector<std::string> descriptions =
      doc.GetColumn<std::string>("Description");

  std::vector<std::string> training_samples;
  std::transform(titles.begin(), titles.end(), descriptions.begin(),
                 std::back_inserter(training_samples),
                 std::plus<std::string>());
  for (auto sample : training_samples) {
    const auto &[compressed, len] = compress(sample);
  }
  std::cout << n << std::endl;
}
