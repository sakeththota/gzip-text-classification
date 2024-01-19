#include "rapidcsv.h"
#include "zlib.h"
#include <algorithm>
#include <cstring>
#include <utility>
#include <future>
#include <thread>

std::pair<char *, int> compress(char *uncompressed, int len) {
  char *compressed = (char *)malloc(len * 2 + 1);

  z_stream strm = {0};
  strm.avail_in = (uInt)len;
  strm.next_in = (Bytef *)uncompressed;
  strm.avail_out = (uInt)len * 2;
  strm.next_out = (Bytef *)compressed;

  deflateInit(&strm, Z_BEST_COMPRESSION);
  deflate(&strm, Z_FINISH);
  deflateEnd(&strm);

  return std::make_pair(compressed, (int)strm.total_out);
}

struct Sample {
  char *text;
  int len;
  int label;

  Sample(std::string text, int len, int label) : len(len), label(label) {
    this->text = new char[len + 1];
    std::strcpy(this->text, text.c_str());
  }
};

std::vector<Sample> parse_data(std::string filename) {
  rapidcsv::Document doc(filename);
  int n = doc.GetRowCount();
  std::vector<int> classes = doc.GetColumn<int>("Class Index");
  std::vector<std::string> titles = doc.GetColumn<std::string>("Title");
  std::vector<std::string> descriptions =
      doc.GetColumn<std::string>("Description");
  std::vector<Sample> samples;
  for (int i = 0; i < n; ++i) {
    std::string concatenated = titles[i] + " " + descriptions[i];
    samples.emplace_back(concatenated, concatenated.length(), classes[i]);
  }
  return samples;
}

struct NCD {
  int label;
  float ncd;

  NCD(int label, float ncd) : label(label), ncd(ncd) {}
};

NCD compute_ncd(Sample x1, Sample x2, size_t c_x1) {
  size_t c_x2 = compress(x2.text, x2.len).second;
  char *x1x2 = new char[x1.len + x2.len + 1];
  std::strcpy(x1x2, x1.text);
  std::strcat(x1x2, x2.text);
  size_t c_x1x2 = compress(x1x2, x1.len + x2.len).second;
  return NCD(x2.label, (c_x1x2 - std::min(c_x1, c_x2)) / (float)std::max(c_x1, c_x2));
}

int main() {
  std::vector<Sample> training_samples = parse_data("datasets/agnews_train.csv");
  std::vector<Sample> test_samples = parse_data("datasets/agnews_test.csv");

  // compute ncd for each pair of test and training samples
  int num_correct = 0;
  for (auto &test_sample : test_samples) {
    int c_x1 = compress(test_sample.text, test_sample.len).second;
    std::vector<std::future<NCD>> futures;
    for (auto& training_sample : training_samples) {
      futures.push_back(std::async(std::launch::async, compute_ncd, test_sample, training_sample, c_x1));
    }

    for(auto& future : futures) {
      future.wait();
    }

    std::vector<NCD> ncds;
    for (auto& future : futures) {
      ncds.push_back(future.get());
    }

    // k-NN w/ k=2
    std::sort(ncds.begin(), ncds.end(),
              [](const NCD &a, const NCD &b) { return a.ncd < b.ncd; });
    std::vector<int> label_freqs(4);
    for (size_t i = 0; i < ncds.size() && i < 2; ++i) {
      label_freqs[ncds[i].label - 1]++;
    }
    size_t predicted = 0;
    for (size_t i = 0; i < 4; ++i) {
      if (label_freqs[i] > label_freqs[predicted]) {
        predicted = i + 1;
      }
    }

    std::cout << "Predicted: " << predicted << std::endl;
    std::cout << "Actual: " << test_sample.label << std::endl;
    if (predicted == test_sample.label)
      num_correct++;
  }
}
