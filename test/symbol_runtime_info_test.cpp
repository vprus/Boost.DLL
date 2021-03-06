// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org


#include <boost/dll.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include <boost/test/minimal.hpp>
#include "../example/shared_lib_path.hpp"


// lib functions

typedef float (lib_version_func)();
typedef void  (say_hello_func)  ();
typedef int   (increment)       (int);

// exe function
extern "C" int BOOST_SYMBOL_EXPORT exef() {
    return 15;
}

void internal_function() {}
int internal_variable = 1;

// Unit Tests
int test_main(int argc, char* argv[]) {
    using namespace boost::dll;

    BOOST_CHECK(argc >= 2);
    boost::filesystem::path shared_library_path = shared_lib_path(argv[1], L"test_library");
    std::cout << "Library: " << shared_library_path;
    shared_library lib(shared_library_path);

    std::cout << std::endl;
    std::cout << "shared_library_: " << shared_library_path << std::endl;
    std::cout << "symbol_location: " << symbol_location(&lib.get<int>("integer_g")) << std::endl;
    std::cout << "lib.location():      " << lib.location() << std::endl;
    BOOST_CHECK(
        symbol_location(lib.get<int>("integer_g")) == lib.location()
    );

    BOOST_CHECK(
        symbol_location(lib.get<say_hello_func>("say_hello")) == lib.location()
    );

    BOOST_CHECK(
        symbol_location(lib.get<lib_version_func>("lib_version")) == lib.location()
    );

    BOOST_CHECK(
        symbol_location(lib.get<const int>("const_integer_g")) == lib.location()
    );

    // Cheking that symbols are still available, after another load+unload of the library
    { shared_library sl2(shared_library_path); }

    BOOST_CHECK(
        symbol_location(lib.get<int>("integer_g")) == lib.location()
    );

    // Checking aliases
    BOOST_CHECK(
        symbol_location(lib.get<std::size_t(*)(const std::vector<int>&)>("foo_bar")) == lib.location()
    );
    BOOST_CHECK(
        symbol_location(lib.get_alias<std::size_t(const std::vector<int>&)>("foo_bar")) == lib.location()
    );


    BOOST_CHECK(
        symbol_location(lib.get<std::size_t*>("foo_variable")) == lib.location()
    );
    BOOST_CHECK(
        symbol_location(lib.get_alias<std::size_t>("foo_variable")) == lib.location()
    );
    
    { // self
        shared_library sl(program_location());
        BOOST_CHECK(
            (boost::filesystem::equivalent(symbol_location(sl.get<int(void)>("exef")), argv[0]))
        );
    }

    std::cout << "\ninternal_function: " << symbol_location(internal_function);
    std::cout << "\nargv[0]          : " << boost::filesystem::absolute(argv[0]);
    BOOST_CHECK(
        (boost::filesystem::equivalent(symbol_location(internal_function), argv[0]))
    );

    BOOST_CHECK(
        (boost::filesystem::equivalent(symbol_location(internal_variable), argv[0]))
    );


    BOOST_CHECK(
        (boost::filesystem::equivalent(this_line_location(), argv[0]))
    );
    BOOST_CHECK(
        lib.get_alias<boost::filesystem::path()>("module_location_from_itself")() == lib.location()
    );
    return 0;
}

