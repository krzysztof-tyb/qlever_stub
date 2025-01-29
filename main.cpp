

#include "backports/algorithm.h"
#include "backports/concepts.h"
#include <vector>
#include <string>

// #include "templates.h"


template <size_t NumStaticCols>
struct IdTableStatic {
  size_t cols = NumStaticCols;
};

template <size_t NumStaticCols>
struct CompressedExternalIdTableBase {
  IdTableStatic<NumStaticCols> currentBlock_;

  // Add a single row to the input. The type of `row` needs to be something that
  // can be `push_back`ed to a `IdTable`.
  void push(const auto& row) requires requires { currentBlock_.push_back(row); }
  {
    currentBlock_.push_back(row);
  }
};


template <typename T, typename Decoder>
concept BulkResultForDecoder = requires {
  std::constructible_from<T, std::vector<Decoder>>;
};

template <typename T>
concept CompressionWrapper = requires(const T& t) {
  typename T::Decoder;
  // Return the number of decoders that are stored.
  { t.numDecoders() } -> std::same_as<size_t>;
  // Decompress the given string, use the Decoder specified by the second
  // argument.
  { t.decompress(std::string_view{}, size_t{0}) } -> std::same_as<std::string>;
  // Compress all the strings and return the strings together with a `Decoder`
  // that can be used to decompress the strings again.
  {
    T::compressAll(std::vector<std::string>{})
  } -> BulkResultForDecoder<typename T::Decoder>;
  requires(std::constructible_from<T, std::vector<typename T::Decoder>>);
};

struct FsstSquaredCompressionWrapper {
  using BulkResult =
      std::tuple<std::string, std::vector<std::string_view>>;
  static BulkResult compressAll() {
    return {"", {"First", "Second"}};
  }
};

template <typename UnderlyingVocabulary>
struct DiskWriter {
  typename UnderlyingVocabulary::WordWriter underlyingWriter_;

  std::vector<bool> isExternalBuffer_;
  
  void finishBlock() {

    auto compressAndWrite = [this, isExternalBuffer = std::move(isExternalBuffer_)]() mutable {
        auto bulkResult = FsstSquaredCompressionWrapper::compressAll();
        auto& [buffer, views] = bulkResult;

        size_t i = 0;
        for (auto& word : views) {
          if constexpr (requires() { underlyingWriter_(word, false); }) {
            underlyingWriter_(word, isExternalBuffer.at(i));
            ++i;
          } else {
            underlyingWriter_(word);
          }
        }
    };

  }

};


int main() {

    return 0;
}
