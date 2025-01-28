#pragma once


#include "backports/algorithm.h"
#include "backports/concepts.h"
#include <vector>
#include <string>

template <typename T>
CPP_concept WordCallback = ranges::invocable<T, std::string_view, bool>;
// Concept for a callable that compares to `string_view`s.
template <typename T>
CPP_concept WordComparator = ranges::predicate<T, std::string_view, std::string_view>;\

CPP_template_def(typename W, typename C)(requires WordComparator<W> CPP_and WordCallback<C>)
void mergeVocabulary(const std::string& basename, size_t numFiles,
                                   W comparator,
                                   C& internalWordCallback,
                                   size_t memoryToUse) {
}

template <typename T>
CPP_requires(HasMake_, requires() (
  T::make(0)
));

template <typename T, typename MakeFromUint64t>
std::vector<T> readGapComprList(size_t nofElements, off_t from,
                                      size_t nofBytes,
                                      MakeFromUint64t makeFromUint64t) {
  std::vector<T> result;

  // TODO<joka921> make this hack unnecessary, probably by a proper output
  // iterator.
  if constexpr (CPP_requires_ref(HasMake_, T)) {
    uint64_t id = 0;
    for (size_t i = 0; i < result.size(); ++i) {
      id += result[i].get();
      result[i] = T::make(id);
    }
  } else {
    T id = 0;
    for (size_t i = 0; i < result.size(); ++i) {
      id += result[i];
      result[i] = id;
    }
  }
  result.resize(nofElements);
  return result;
}


// template <typename T>
// concept CompressionWrapper = requires(const T& t) {
//   typename T::Decoder;
//   // Return the number of decoders that are stored.
//   { t.numDecoders() } -> std::same_as<size_t>;
//   // Decompress the given string, use the Decoder specified by the second
//   // argument.
//   { t.decompress(std::string_view{}, size_t{0}) } -> std::same_as<std::string>;
//   // Compress all the strings and return the strings together with a `Decoder`
//   // that can be used to decompress the strings again.
//   {
//     T::compressAll(std::vector<std::string>{})
//   } -> BulkResultForDecoder<typename T::Decoder>;
//   requires(std::constructible_from<T, std::vector<typename T::Decoder>>);
// };

template <typename T>
CPP_requires(CompressionWrapper_, requires(const T& t) (
  // Return the number of decoders that are stored.
  std::is_same_v<size_t, decltype(t.numDecoders())>,
  // Decompress the given string, use the Decoder specified by the second
  // argument.
  t.decompress(std::string_view{}, size_t{0}),
  // Compress all the strings and return the strings together with a `Decoder`
  // that can be used to decompress the strings again.
  T::compressAll(std::vector<std::string>{})
));


template <typename T>
CPP_concept CompressionWrapper =
  CPP_requires_ref(CompressionWrapper_, T) &&
  concepts::defs::constructible_from<T, std::vector<typename T::Decoder>>;


template <size_t S>
struct IdTable {
  CPP_template_def(size_t NewNumColumns)(requires(NewNumColumns == 0))
  IdTable asStaticView() const {
    return IdTable<NewNumColumns>{};
  }
};

template <typename T>
CPP_requires(HasAsStaticView, requires(T& table) (
  table.template asStaticView<0>()
));

template <typename T>
static IdTable<0> toView(const T& table) {
    if constexpr (CPP_requires_ref(HasAsStaticView, T)) {
        return table.template asStaticView<0>();
    } else {
        return table;
    }
}

