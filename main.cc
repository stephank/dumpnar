#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>

#include "serialize.hh"

namespace fs = std::filesystem;
using std::cerr, std::cout, std::endl;

/** Dump file contents as NAR. */
static void dumpContents(const fs::path &path) {
  auto size = fs::file_size(path);
  cout << NarStr{"contents"} << NarInt{size};

  std::ifstream ifs(path);

  char buf[65536];
  size_t left = size;

  while (ifs && left > 0) {
    auto n = std::min(left, sizeof(buf));
    ifs.read(buf, n);
    left -= n;
    cout.write(buf, n);
  }

  if (!ifs) {
    cerr << "Error reading file " << path << ": " << std::strerror(errno)
         << endl;
    exit(1);
  }

  cout << NarPad{size};
}

/** Dump a path as NAR. */
static void dump(const fs::path &path) {
  auto st = fs::symlink_status(path);

  cout << NarStr{"("};

  switch (st.type()) {

  case fs::file_type::regular:
    cout << NarStr{"type"} << NarStr{"regular"};
    if ((st.permissions() & fs::perms::owner_exec) != fs::perms::none) {
      cout << NarStr{"executable"} << NarStr{""};
    }
    dumpContents(path);

    break;

  case fs::file_type::directory: {
    cout << NarStr{"type"} << NarStr{"directory"};

    std::set<std::string> sorted;
    for (const auto &entry : fs::directory_iterator(path)) {
      sorted.emplace(entry.path().filename());
    }

    for (auto &filename : sorted) {
      cout << NarStr{"entry"} << NarStr{"("} << NarStr{"name"}
           << NarStr{filename} << NarStr{"node"};
      dump(path / filename);
      cout << NarStr{")"};
    }

    break;
  }

  case fs::file_type::symlink:
    cout << NarStr{"type"} << NarStr{"symlink"} << NarStr{"target"}
         << NarStr{fs::read_symlink(path).string()};

    break;

  default:
    cerr << "File " << path << " has an unsupported type" << endl;

    exit(1);
  }

  cout << NarStr{")"};
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <path>" << endl;
    exit(64);
  }

  cout << NarStr{"nix-archive-1"};
  dump(argv[1]);
}
