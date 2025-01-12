/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) 2024-25  SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */



#ifndef ImplPtr_h
#define ImplPtr_h

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

/**
 * Helper template class for implementation pointers (pointers to a private
 * class or structure that hold the member variables of a higher-level class
 * that is part of a public API).
 *
 * This pointer class maintains constness of its parent class, i.e. if it is
 * used in a const class the class this pointer points to will also be const.
 *
 * This class automatically deletes the class it points to in its destructor.
 **/
template<class _Impl>
class ImplPtr : private boost::noncopyable
{
#if defined( BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS ) || defined( BOOST_NO_CXX11_NULLPTR )
    typedef typename boost::scoped_ptr<_Impl>::unspecified_bool_type unspecified_bool_type;
#endif

public:
    typedef _Impl element_type;

    explicit
    ImplPtr( _Impl * impl_r = 0 )    : _impl( impl_r ) {}

public:
    void reset( _Impl * impl_r = 0 ) { _impl.reset( impl_r ); }

    void swap( ImplPtr rhs )         { _impl.swap( rhs._impl ); }

public:
#if defined( BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS ) || defined( BOOST_NO_CXX11_NULLPTR )
    operator unspecified_bool_type() const { return _impl; }
#else
    explicit operator bool() const { return _impl.get() != 0; }
#endif

    const _Impl & operator*()  const { return *_impl; }
    const _Impl * operator->() const { return _impl.get(); }
    const _Impl * get()        const { return _impl.get(); }

    _Impl &       operator*()        { return *_impl; }
    _Impl *       operator->()       { return _impl.get(); }
    _Impl *       get()              { return _impl.get(); }

private:
    boost::scoped_ptr<_Impl> _impl;
};

template<class _Impl>
inline bool operator==( ImplPtr<_Impl> & lhs, ImplPtr<_Impl> & rhs ) { return lhs.get() == rhs.get(); }

template<class _Impl>
inline bool operator!=( ImplPtr<_Impl> & lhs, ImplPtr<_Impl> & rhs ) { return lhs.get() != rhs.get(); }

template<class _Impl>
inline bool operator< ( ImplPtr<_Impl> & lhs, ImplPtr<_Impl> & rhs ) { return lhs.get() <  rhs.get(); }

#endif // ImplPtr_h
