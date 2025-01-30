

#include "backports/algorithm.h"
#include "backports/concepts.h"
#include <vector>
#include <string>

// #include "templates.h"


template <typename T, typename U>
constexpr static bool isSimilar =
    std::is_same_v<std::decay_t<T>, std::decay_t<U>>;

/// A concept for similarity
template <typename T, typename U>
CPP_concept SimilarTo = isSimilar<T, U>;

/// True iff `T` is similar (see above) to any of the `Ts...`.
template <typename T, typename... Ts>
CPP_concept SimilarToAny = (... || isSimilar<T, Ts>);

template <size_t NumStaticCols>
struct IdTableStatic {
  size_t cols = NumStaticCols;
};

template <typename B, typename R>
CPP_requires(CanBePushed_, requires(B& currentBlock_, const R& row) (
  currentBlock_.push_back(row)
));

template <typename B, typename R>
CPP_concept CanBePushed = CPP_requires_ref(CanBePushed_, B, R);


template <size_t NumStaticCols>
struct CompressedExternalIdTableBase {
  IdTableStatic<NumStaticCols> currentBlock_;

  // Add a single row to the input. The type of `row` needs to be something that
  // can be `push_back`ed to a `IdTable`.
  CPP_template_def(typename R)(requires CanBePushed<IdTableStatic<NumStaticCols>, R>)
  void push(const R& row)
  {
    currentBlock_.push_back(row);
  }
};

template <typename T, typename Decoder>
CPP_requires(BulkResultForDecoder_, requires(T t) (
  std::tuple_size_v<T> == 3,
  SimilarToAny<decltype(std::get<1>(t)), std::vector<std::string_view>,std::vector<std::string>>,
  SimilarTo<decltype(std::get<2>(t)), Decoder>
));

template <typename T, typename Decoder>
CPP_concept BulkResultForDecoder = CPP_requires_ref(BulkResultForDecoder_, T, Decoder);

template <typename T>
CPP_requires(CompressionWrapper_, requires(const T&t) (
  concepts::same_as<decltype(t.numDecoders()), size_t>,
  concepts::same_as<decltype(t.decompress(std::string_view{}, size_t{0})), std::string>,
  BulkResultForDecoder<decltype(T::compressAll(std::vector<std::string>{})), typename T::Decoder>,
  concepts::constructible_from<T, std::vector<typename T::Decoder>>
));

template <typename T>
CPP_concept CompressionWrapper = CPP_requires_ref(CompressionWrapper_, T);

struct FsstSquaredCompressionWrapper {
  using BulkResult =
      std::tuple<std::string, std::vector<std::string_view>>;
  static BulkResult compressAll() {
    return {"", {"First", "Second"}};
  }
};

template <typename Writer, typename Word>
CPP_requires(WithTwoArgs_, requires(Writer& underlyingWriter_, Word& word) (
  underlyingWriter_(word, false)
));

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
          if constexpr (CPP_requires_ref(WithTwoArgs_, typename UnderlyingVocabulary::WordWriter, decltype(word))) {
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
