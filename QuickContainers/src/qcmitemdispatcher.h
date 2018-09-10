#ifndef QCMITEMDISPATCHER_H
#define QCMITEMDISPATCHER_H
// Std headers
#include <memory>
#include <unordered_map>

// Qt headers
#include <QObject>
#include <QDebug>

namespace qcm { // ::qcm

struct ItemDispatcherBase {
    using unsupported_type          = std::integral_constant<int, 0>;

    using non_ptr_type              = std::integral_constant<int, 1>;
    using ptr_type                  = std::integral_constant<int, 2>;
    using ptr_qobject_type          = std::integral_constant<int, 3>;

    // Note: Not actually used, extension for later QPointer support
    using q_ptr_type                = std::integral_constant<int, 4>;

    using shared_ptr_type           = std::integral_constant<int, 5>;
    using shared_ptr_qobject_type   = std::integral_constant<int, 6>;

    using weak_ptr_type             = std::integral_constant<int, 7>;
    using weak_ptr_qobject_type     = std::integral_constant<int, 8>;

    template <typename T>
    inline static constexpr auto debug_type() -> const char* { return "unsupported"; }
};

// FIXME: Use c++14 template using instead of traits here....
template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::non_ptr_type>() -> const char* { return "non_ptr_type"; }

template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::ptr_type>() -> const char* { return "ptr_type"; }

template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::ptr_qobject_type>() -> const char* { return "ptr_qobject_type"; }

template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::q_ptr_type>() -> const char* { return "q_ptr_type"; }

template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::shared_ptr_type>() -> const char* { return "shared_ptr_type"; }

template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::shared_ptr_qobject_type>() -> const char* { return "shared_ptr_qobject_type"; }

template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::weak_ptr_type>() -> const char* { return "weak_ptr_type"; }

template <>
inline auto ItemDispatcherBase::debug_type<ItemDispatcherBase::weak_ptr_qobject_type>() -> const char* { return "weak_ptr_qobject_type"; }

template <typename Item_type>
struct ItemDispatcher : public ItemDispatcherBase {
    // QObject and non copy constructible POD are unsupported
    using type = typename std::conditional< std::is_base_of< QObject, Item_type >::value ||
    !std::is_copy_constructible<Item_type>::value,
    ItemDispatcherBase::unsupported_type,
    ItemDispatcherBase::non_ptr_type
    >::type;
};


template <typename Item_type>
struct ItemDispatcher<Item_type*>  : public ItemDispatcherBase {
    using type = typename std::conditional< std::is_base_of< QObject, typename std::remove_pointer< Item_type >::type >::value,
    ItemDispatcherBase::ptr_qobject_type,
    ItemDispatcherBase::ptr_type
    >::type;
};

template < typename Item_type >
struct ItemDispatcher<QPointer<Item_type>>  : public ItemDispatcherBase {
    using type = ItemDispatcherBase::q_ptr_type;
};

template < typename Item_type >
struct ItemDispatcher<std::shared_ptr<Item_type>>  : public ItemDispatcherBase {
    using type = typename std::conditional< std::is_base_of< QObject, Item_type >::value,
    ItemDispatcherBase::shared_ptr_qobject_type,
    ItemDispatcherBase::shared_ptr_type
    >::type;
};

template < typename Item_type >
struct ItemDispatcher<std::weak_ptr<Item_type>>  : public ItemDispatcherBase {
    using type = typename std::conditional< std::is_base_of< QObject, Item_type >::value,
    ItemDispatcherBase::weak_ptr_qobject_type,
    ItemDispatcherBase::weak_ptr_type
    >::type;
};
}
#endif // QCMITEMDISPATCHER_H
