# Repository instructions

## Code Style

All code should follow the Zephyr coding style guidelines.
Use `clang-format` to automatically format code where applicable.
Ensure that all new code is properly indented and follows the project's style conventions.

Ensure all files are properly terminated with a newline character.

## Git commit messages

Write all commit messages using the Conventional Commits format:

```
<type>[optional scope]: <description>
```

Use a clear imperative description, for example `feat(uzlib): add decompression support`.

## Documentation

All documentation, including `README.md` files,
should follow the Markdown format.
Documentation intended to be published should be placed in the appropriate `docs` directory
within the repository.
Ensure that all new features, changes,
and usage instructions are clearly documented.

It should follow Semantic Line Breaks (see https://sembr.org/).
Long lines should be broken into multiple lines
at semantic or logical points,
such as after punctuation or conjunctions,
to improve readability
and make diff easier to review.
