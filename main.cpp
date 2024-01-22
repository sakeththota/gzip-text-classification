#include "rapidcsv.h"
#include "zlib.h"
#include <algorithm>
#include <cstring>
#include <utility>

using std::string;
using std::vector;

struct Sample {
  char *text;
  size_t len;
  size_t label;

  Sample(std::string text, size_t len, int label) : len(len), label(label) {
    this->text = new char[len + 1];
    std::strcpy(this->text, text.c_str());
  }
};

struct NCD {
  size_t label;
  float ncd;

  NCD(size_t label, float ncd) : label(label), ncd(ncd) {}

  bool operator<(const NCD &other) const { return ncd < other.ncd; }
};

std::pair<char *, size_t> compress(char *uncompressed, int len) {
  char *compressed = (char *)malloc(len + 1);

  z_stream strm = {0};
  strm.avail_in = (int)len;
  strm.next_in = (Bytef *)uncompressed;
  strm.avail_out = (int)len;
  strm.next_out = (Bytef *)compressed;

  deflateInit(&strm, Z_BEST_COMPRESSION);
  deflate(&strm, Z_FINISH);
  deflateEnd(&strm);

  return std::make_pair(compressed, (size_t)strm.total_out);
}

std::vector<Sample> parse_data(std::string filename) {
  rapidcsv::Document doc(filename);
  size_t n = doc.GetRowCount();
  std::vector<int> classes = doc.GetColumn<int>("Class Index");
  std::vector<std::string> titles = doc.GetColumn<std::string>("Title");
  std::vector<std::string> descriptions =
      doc.GetColumn<std::string>("Description");
  std::vector<Sample> samples;
  for (size_t i = 0; i < n; ++i) {
    std::string concatenated = titles[i] + " " + descriptions[i];
    samples.emplace_back(concatenated, concatenated.length(), classes[i]);
  }
  return samples;
}

int main() {
  std::vector<Sample> training_samples =
      parse_data("datasets/agnews_train.csv");
  std::vector<Sample> test_samples = parse_data("datasets/agnews_test.csv");

  // compute ncd for each pair of test and training samples
  size_t num_correct = 0;
  size_t curr_sample = 1;
  for (auto &test_sample : test_samples) {
    const auto &[z_x1, c_x1] = compress(test_sample.text, test_sample.len);

    std::vector<NCD> ncds;
    for (auto &train_sample : training_samples) {
      const auto &[z_x2, c_x2] = compress(train_sample.text, train_sample.len);

      char *x1x2 = new char[test_sample.len + train_sample.len + 1];
      std::strcpy(x1x2, test_sample.text);
      std::strcat(x1x2, train_sample.text);
      const auto &[z_x1x2, c_x1x2] =
          compress(x1x2, test_sample.len + train_sample.len);

      ncds.emplace_back(train_sample.label, (c_x1x2 - std::min(c_x1, c_x2)) /
                                                (float)std::max(c_x1, c_x2));
    }

    // k-NN w/ k=2
    std::sort(ncds.begin(), ncds.end());
    size_t label_freqs[4] = {0};
    for (size_t i = 0; i < ncds.size() && i < 2; ++i) {
      label_freqs[ncds[i].label - 1]++;
    }
    size_t predicted =
        std::max_element(label_freqs, label_freqs + 4) - label_freqs + 1;

    std::cout << "Predicted: " << predicted << std::endl;
    std::cout << "Actual: " << test_sample.label << std::endl;
    if (predicted == test_sample.label)
      num_correct++;
    std::cout << "Accuracy: " << num_correct / (float)curr_sample << std::endl
              << std::endl;
    curr_sample++;
  }
}
