# Answers: exercises 1 to 4

## Exercise 1. Predict the output

Predictions written first, then checked with a small program (Linux, g++ 13, C++17).

| Line | Prediction | Actual | Right? |
| --- | --- | --- | --- |
| a | `repo/.minigit/HEAD` | `repo/.minigit/HEAD` | yes |
| b | `/HEAD` | `/HEAD` | yes |
| c | `notes.txt` | `notes.txt` | yes |
| d | `docs` | `docs` | yes |
| e | `1` | `1` | yes |
| f | `.txt` | `.txt` | yes |

Notes:
- **b** is the surprise in this exercise. `"/HEAD"` starts with `/`, so it is a *rooted* path, and `operator/` throws away everything on the left. The result is `/HEAD`, not `repo/HEAD`.
- **e** prints `1` because a bare file name has no folder in front of it, so `parent_path()` is empty. That is why `writeTextFile` checks `!parent.empty()` before calling `create_directories`.
- **f** includes the dot: `extension()` returns `.txt`, not `txt`.

Program used to check:

```cpp
#include <filesystem>
#include <iostream>
int main() {
    using std::filesystem::path;
    std::cout << "a: " << (path("repo") / ".minigit" / "HEAD").generic_string() << '\n';
    std::cout << "b: " << (path("repo") / "/HEAD").generic_string() << '\n';
    std::cout << "c: " << path("docs/notes.txt").filename().string() << '\n';
    std::cout << "d: " << path("docs/notes.txt").parent_path().string() << '\n';
    std::cout << "e: " << path("notes.txt").parent_path().empty() << '\n';
    std::cout << "f: " << path("notes.txt").extension().string() << '\n';
}
```

## Exercise 2. Find the bug: lost newlines

**Why:** `std::getline` reads up to a `'\n'` and then *throws the `'\n'` away*. The loop adds `line` but never puts the newline back, so `"a\nb\n"` becomes `"a"` + `"b"` = `"ab"`.

(Adding `text += line + '\n';` is not a full fix either: it would add a `'\n'` to a last line that did not have one, so the text still might not match.)

**Fix:** copy the whole stream buffer in one step, which keeps every character:

```cpp
std::string readAll(const std::filesystem::path& file) {
    std::ifstream input(file, std::ios::binary);
    std::ostringstream contents;      // needs #include <sstream>
    contents << input.rdbuf();
    return contents.str();
}
```

## Exercise 3. Find the bugs: two path mistakes

**Bug 1, `metadataDirectory`:** `root.string() + "/.minigit"` glues strings together with a hard-coded `/`. On Windows the separator is `\`, so the result mixes separators and does not match `root / ".minigit"`.
Fix: `return root / ".minigit";`

**Bug 2, `headFile`:** `/ "/HEAD"` has a leading `/`, which makes the right side a rooted path. A rooted right side *replaces* the left side, so the result is just `/HEAD` on every platform (see Exercise 1b). That is why the second test fails everywhere.
Fix: `return metadataDirectory(root) / "HEAD";`

## Exercise 4. Fill in the blanks

```cpp
bool save(const std::filesystem::path& file, const std::string& text) {
    if (!file.parent_path().empty()) {                           // (1)
        std::filesystem::create_directories(file.parent_path()); // (2)
    }
    std::ofstream output(file);
    if (!output) {                                               // (3)
        return false;
    }
    output << text;
    return static_cast<bool>(output);                            // (4)
}
```

Note on (4): `return output;` does **not** compile. A stream's conversion to `bool` is `explicit`, so it works inside `if (...)` but not in a `return`. Use `static_cast<bool>(output)` or `!output.fail()`. I checked this: g++ says *cannot convert 'std::ofstream' to 'bool' in return*.
