Project: simple lexer (lang) — quick orientation for coding agents

This repository contains a small C++ lexer project driven by a Flex scanner.
These notes are focused and actionable: what to read first, how to build and test,
and the repository-specific patterns you must follow when making changes.

- **Big picture:** The program is a lexer-only front-end that reads a source
  file, produces XML describing tokens and scope operations, and manages a
  global symbol table. Key runtime globals are `g_symbolTable`, `g_insert`,
  and `g_local` (see `main.cpp` and `lex.h`). The scanner (`lang.l`) creates
  or looks up `cSymbol` objects and sets `yylval.symbol` for `main.cpp` to print.

- **Key files to inspect first:**
  - `Makefile` — primary build rules (runs `flex`, compiles with `g++ -std=c++11`).
  - `lang.l` — Flex scanner; `ProcessID()` creates/inserts symbols.
  - `main.cpp` — program entry; loops on `yylex()` and prints XML tags.
  - `tokens.h` — token numeric values used by scanner and main.
  - `cSymbol.h`, `cSymbolTable.h` — symbol representation and scope handling.
  - `test/` — test runner scripts (`rund`, `runt`, `regress`) and `.lang/.correct` pairs.

- **How to build (usual, Unix-like environment):**
  - Preferred (uses Makefile):
    - `make`
  - Manual (explicit, helpful on Windows PowerShell if `make` is missing):
    - `flex -o langlex.c lang.l`
    - `g++ -Wall -g -c -O0 -std=c++11 main.cpp -o main.o`
    - `g++ -Wall -g -c -O0 -std=c++11 -Wno-sign-compare langlex.c -o langlex.o`
    - `g++ main.o langlex.o -o lang`

- **How to run tests:**
  - Tests are POSIX shell scripts and expect `xmllint` (libxml2). On Windows run
    under WSL, Git Bash, or a Unix environment.
    - From repo root (Unix shell): `./test/regress`
    - Or run a single test: `bash test/runt test1` (or `test/rund` for verbose diffs).
  - The scripts execute `./lang test/<n>.lang out.xml`, format with `xmllint`,
    then compare with `test/<n>.correct` using `diff -bwB`.

- **Project-specific patterns and conventions:**
  - Global state is used intentionally for the lexer: `g_symbolTable`, `g_insert`,
    and `g_local` are toggles handled by tokens `INSERT`, `LOOKUP`, `LOCAL`, `GLOBAL`.
  - The scanner both creates and returns `cSymbol` pointers via `yylval.symbol`.
    Changes to symbol creation should be made in `lang.l::ProcessID()`.
  - Token numbers live in `tokens.h`; update both scanner and `main.cpp` when
    adding new tokens.
  - XML output is produced directly with `std::cout` in `main.cpp`; tests rely
    on exact formatting after `xmllint` normalization, so avoid changing
    spacing/line-break conventions unless updating tests.

- **When you edit files, check these places:**
  - If changing token behavior: `tokens.h`, `lang.l`, and `main.cpp`.
  - If changing symbol layout or ID behavior: `cSymbol.h`, `cSymbolTable.h`,
    and scan for `g_symbolTable` usages.
  - If changing build flags: `Makefile` (COPTS is used across compilation units).

- **Dependencies & environment notes:**
  - Required tools: `flex`, `g++` (with C++11 support), `make` (optional), `xmllint`.
  - On Windows prefer WSL/Git Bash or install MSYS/MinGW with `flex` and `xmllint`.

- **Quick examples (common edits):**
  - Add a new token `FOO`: add `#define FOO 1021` to `tokens.h`, return it from
    `lang.l`, and handle it in `main.cpp`'s token loop.
  - Change symbol insertion policy: modify `lang.l::ProcessID()` and update
    `g_insert` handling in `main.cpp` if semantics shift.

If anything here is unclear or you'd like more detail (for example: a walkthrough
of `cSymbolTable` internals or automated Windows build instructions), tell me
which section to expand and I'll update this file accordingly.
