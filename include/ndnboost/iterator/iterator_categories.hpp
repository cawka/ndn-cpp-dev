// (C) Copyright Jeremy Siek 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_ITERATOR_CATEGORIES_HPP
# define NDNBOOST_ITERATOR_CATEGORIES_HPP

# include <ndnboost/config.hpp>
# include <ndnboost/detail/iterator.hpp>
# include <ndnboost/iterator/detail/config_def.hpp>

# include <ndnboost/detail/workaround.hpp>

# include <ndnboost/mpl/eval_if.hpp>
# include <ndnboost/mpl/identity.hpp>
# include <ndnboost/mpl/placeholders.hpp>
# include <ndnboost/mpl/aux_/lambda_support.hpp>

# include <ndnboost/type_traits/is_convertible.hpp>

# include <ndnboost/static_assert.hpp>

namespace ndnboost {

//
// Traversal Categories
//

struct no_traversal_tag {};

struct incrementable_traversal_tag 
  : no_traversal_tag
{
//     incrementable_traversal_tag() {}
//     incrementable_traversal_tag(std::output_iterator_tag const&) {};
};
  
struct single_pass_traversal_tag
  : incrementable_traversal_tag
{
//     single_pass_traversal_tag() {}
//     single_pass_traversal_tag(std::input_iterator_tag const&) {};
};
  
struct forward_traversal_tag
  : single_pass_traversal_tag
{
//     forward_traversal_tag() {}
//     forward_traversal_tag(std::forward_iterator_tag const&) {};
};
  
struct bidirectional_traversal_tag
  : forward_traversal_tag
{
//     bidirectional_traversal_tag() {};
//     bidirectional_traversal_tag(std::bidirectional_iterator_tag const&) {};
};
  
struct random_access_traversal_tag
  : bidirectional_traversal_tag
{
//     random_access_traversal_tag() {};
//     random_access_traversal_tag(std::random_access_iterator_tag const&) {};
};

namespace detail
{  
  //
  // Convert a "strictly old-style" iterator category to a traversal
  // tag.  This is broken out into a separate metafunction to reduce
  // the cost of instantiating iterator_category_to_traversal, below,
  // for new-style types.
  //
  template <class Cat>
  struct old_category_to_traversal
    : mpl::eval_if<
          is_convertible<Cat,std::random_access_iterator_tag>
        , mpl::identity<random_access_traversal_tag>
        , mpl::eval_if<
              is_convertible<Cat,std::bidirectional_iterator_tag>
            , mpl::identity<bidirectional_traversal_tag>
            , mpl::eval_if<
                  is_convertible<Cat,std::forward_iterator_tag>
                , mpl::identity<forward_traversal_tag>
                , mpl::eval_if<
                      is_convertible<Cat,std::input_iterator_tag>
                    , mpl::identity<single_pass_traversal_tag>
                    , mpl::eval_if<
                          is_convertible<Cat,std::output_iterator_tag>
                        , mpl::identity<incrementable_traversal_tag>
                        , void
                      >
                  >
              >
          >
      >
  {};

# if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
  template <>
  struct old_category_to_traversal<int>
  {
      typedef int type;
  };
# endif

  template <class Traversal>
  struct pure_traversal_tag
    : mpl::eval_if<
          is_convertible<Traversal,random_access_traversal_tag>
        , mpl::identity<random_access_traversal_tag>
        , mpl::eval_if<
              is_convertible<Traversal,bidirectional_traversal_tag>
            , mpl::identity<bidirectional_traversal_tag>
            , mpl::eval_if<
                  is_convertible<Traversal,forward_traversal_tag>
                , mpl::identity<forward_traversal_tag>
                , mpl::eval_if<
                      is_convertible<Traversal,single_pass_traversal_tag>
                    , mpl::identity<single_pass_traversal_tag>
                    , mpl::eval_if<
                          is_convertible<Traversal,incrementable_traversal_tag>
                        , mpl::identity<incrementable_traversal_tag>
                        , void
                      >
                  >
              >
          >
      >
  {
  };
  
# if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
  template <>
  struct pure_traversal_tag<int>
  {
      typedef int type;
  };
# endif

} // namespace detail


//
// Convert an iterator category into a traversal tag
//
template <class Cat>
struct iterator_category_to_traversal
  : mpl::eval_if< // if already convertible to a traversal tag, we're done.
        is_convertible<Cat,incrementable_traversal_tag>
      , mpl::identity<Cat>
      , ndnboost::detail::old_category_to_traversal<Cat>
    >
{};

// Trait to get an iterator's traversal category
template <class Iterator = mpl::_1>
struct iterator_traversal
  : iterator_category_to_traversal<
        typename ndnboost::detail::iterator_traits<Iterator>::iterator_category
    >
{};

# ifdef NDNBOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT
// Hack because NDNBOOST_MPL_AUX_LAMBDA_SUPPORT doesn't seem to work
// out well.  Instantiating the nested apply template also
// requires instantiating iterator_traits on the
// placeholder. Instead we just specialize it as a metafunction
// class.
template <>
struct iterator_traversal<mpl::_1>
{
    template <class T>
    struct apply : iterator_traversal<T>
    {};
};
template <>
struct iterator_traversal<mpl::_>
  : iterator_traversal<mpl::_1>
{};
# endif

} // namespace ndnboost

#include <ndnboost/iterator/detail/config_undef.hpp>

#endif // NDNBOOST_ITERATOR_CATEGORIES_HPP