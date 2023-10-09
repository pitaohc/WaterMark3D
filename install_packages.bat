@rem Install packages for vcpkg
@vcpkg install fmt qt5 assimp --triplet x64-windows
@vcpkg integrate install