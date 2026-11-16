// Exercise 5: add a new path function by following an existing one.
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int failures = 0;
void check(bool condition, const std::string& message) {
    std::cout << (condition ? "  PASS  " : "  FAIL  ") << message << '\n';
    if (!condition) ++failures;
}

// Given (from HW8).
fs::path metadataDirectory(const fs::path& root) {
    return root / ".minigit";
}

// TODO: return root/.minigit/index.txt. Reuse metadataDirectory(root).
fs::path indexFile(const fs::path& root) {
    // Reuse metadataDirectory so ".minigit" is written in only one place.
    // No leading '/' on "index.txt": a rooted right side would replace the left.
    return metadataDirectory(root) / "index.txt";
}

int main() {
    const fs::path root = "course/repo";
    check(indexFile(root) == root / ".minigit" / "index.txt", "index file for course/repo");
    check(indexFile(root).filename() == "index.txt", "file name is index.txt");
    check(indexFile(root).parent_path() == metadataDirectory(root), "index file is inside .minigit");

    // My own check: a nested root, to catch code that only works for one root.
    const fs::path nestedRoot = "course/team/repo";
    check(indexFile(nestedRoot) == nestedRoot / ".minigit" / "index.txt",
          "index file for nested root course/team/repo");

    return failures == 0 ? 0 : 1;
}
