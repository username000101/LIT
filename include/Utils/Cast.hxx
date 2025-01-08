#ifndef LIT_UTILS_CAST_HXX
#define LIT_UTILS_CAST_HXX

#include <memory>

#include <td/telegram/td_api.h>

namespace utils {
    template <typename RawType> [[nodiscard]] inline std::shared_ptr<RawType> tl_uptr_cast(td::tl::unique_ptr<RawType>&& tdtptr_object) {
        return std::make_shared<RawType>(std::forward<RawType>(*tdtptr_object.release()));
    }

    template <typename RawType> [[nodiscard]] inline std::shared_ptr<RawType> td_object_cast(td::tl_object_ptr<RawType>&& tdtptr_object) {
        return std::make_shared<RawType>(std::forward<RawType>(*tdtptr_object.release()));
    }

}

#endif // LIT_UTILS_CAST_HXX
