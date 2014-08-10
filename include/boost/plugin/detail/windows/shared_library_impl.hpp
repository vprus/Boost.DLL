// shared_library_impl.hpp ---------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2012 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP
#define BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP

#include <boost/config.hpp>
#include <boost/plugin/shared_library_types.hpp>
#include <boost/plugin/shared_library_load_mode.hpp>

#include <boost/noncopyable.hpp>
#include <boost/swap.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/detail/winapi/dll2.hpp> // TODO: FIXME

namespace boost { namespace plugin {

class shared_library_impl : noncopyable {
    static inline boost::system::error_code last_error_code() BOOST_NOEXCEPT {
        return boost::system::error_code(
            boost::detail::winapi::GetLastError(),
            boost::system::system_category()
        );
    }

public:
    typedef boost::detail::winapi::HMODULE_ native_handle_t;

    shared_library_impl() BOOST_NOEXCEPT
        : handle_(NULL)
    {}

    ~shared_library_impl() BOOST_NOEXCEPT {
        unload();
    }

    void load(const library_path &sh, load_mode::type mode, boost::system::error_code &ec) BOOST_NOEXCEPT {
        unload();

        boost::detail::winapi::DWORD_ flags = static_cast<boost::detail::winapi::DWORD_>(mode);
        handle_ = boost::detail::winapi::LoadLibraryExW(sh.c_str(), 0, flags);
        if (!handle_) {
            ec = last_error_code();
        }
    }

    bool is_loaded() const BOOST_NOEXCEPT {
        return (handle_ != 0);
    }

    void unload() BOOST_NOEXCEPT {
        if (handle_) {
            boost::detail::winapi::FreeLibrary(handle_);
            handle_ = 0;
        }
    }

    void swap(shared_library_impl& rhs) BOOST_NOEXCEPT {
        boost::swap(handle_, rhs.handle_);
    }

    static library_path suffix() {
        return L".dll";
    }

    void* symbol_addr(const symbol_type &sb, boost::system::error_code &ec) const BOOST_NOEXCEPT {
        // Judging by the documentation and
        // at GetProcAddress there is no version for UNICODE.
        // There can be it and is correct, as in executed
        // units names of functions are stored in narrow characters.
        void* const symbol = boost::detail::winapi::GetProcAddress(handle_, sb.data());
        if (symbol == NULL) {
            ec = last_error_code();
        }

        return symbol;
    }

    native_handle_t native() const BOOST_NOEXCEPT {
        return handle_;
    }

private:
    native_handle_t handle_;
};

}} // boost::plugin

#endif // BOOST_PLUGIN_SHARED_LIBRARY_IMPL_HPP
