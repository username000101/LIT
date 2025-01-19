# LIT - Installation

**1.** Installing dependencies (**All information about package names is taken from [repology](https://repology.org)**)

 - TDLib: You need to build it from source and install it, use the [official repository](https://github.com/tdlib/td) for this
 - nlohmann-json: 
   - Ubuntu/Debian: nlohmann-json3-dev
   - Fedora: nlohmann-json-devel
   - Arch: nlohmann-json
 - spdlog:
   - Ubuntu/Debian: spdlog
   - Fedora: spdlog
   - Arch: spdlog
 - libarchive:
   - Ubuntu/Debian: libarchive (the repositories have version 3.7.4, but the current version is 3.7.7)
   - Fedora: libarchive (the repositories have version 3.7.4, but the current version is 3.7.7)
   - Arch: libarchive
 - CMake
   - Ubuntu/Debian: cmake
   - Fedora: cmake
   - Arch: cmake

**2.**  Cloning the repository

- Branch: the stable main is used by default, but you can try using experimental (be careful, changes in experimental may sometimes not even compile)
- Cloning: git clone [--branch experimental] https://github.com/username000101/LIT

**3.** Getting Telegram Application IDs

- Go to my.telegram.org and get your API_ID and API_HASH

**4.** Building

- Go to the project directory: cd LIT
- Create a build directory (in the example, 'build' will be used): mkdir build && cd build
- Generator: You can use both Makefile and Ninja (amazing, isn't it?)(In testing, LIT is built using Ninja)
- The LIT install prefix: You can specify the directory where LIT will store the files, just specify the parameter -DLIT_INSTALL_PREFIX=/path/to/folder when launching CMake
- Run cmake: cmake [-GNinja] [-DLIT_INSTALL_PREFIX=...] -DAPI_ID=<Your API_ID, which you received from my.telegram.org> -DAPI_HASH=<Your API_HASH, which you received from my.telegram.org> ..
- If there are no errors, write cmake --build . 
- If there are no errors... Congratulations, you have built LIT! Run it (if you are using the default directory/other directory that requires su for writing, run LIT via sudo) and follow the instructions to log in to your account



## Useful information

- [Setting up and configuring LIT](https://github.com/username000101/LIT/readme's/CONFIGURING.md)
- [Help and support](https://github.com/username000101/LIT/readme's/HELP.md)