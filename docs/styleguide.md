# Style Guide

Use a consistent coding style provided by [Google C++ Style guide](https://google.github.io/styleguide/cppguide.html) which I recommend reading.

## vscode configurations

If you are using vscode you can consider adding the following settings to your `settings.json` file.

```json
{
  "files.insertFinalNewline": true,
  "files.trimFinalNewlines": true
}
```

## [`cpplint`](https://github.com/cpplint/cpplint)

cpplint or cpplint.py is an open source lint-like tool developed by Google, designed to ensure that C++ code conforms to Google's coding style guides. Therefore cpplint implements what Google considers best practices in C++ coding.

Add the following lines to your vscode `settings.json` file in order to enable C++ linting,

```json
{
  "cpplint.cpplintPath": "/usr/local/bin/cpplint",
  "cpplint.lineLength": 80,
  "cpplint.lintMode": "workspace",
  "cpplint.root": "${workspaceFolder}",
  "cpplint.verbose": 5,
  "cpplint.extensions": ["cc", "hh"],
  "clang-format.executable": "/usr/bin/clang-format"
}
```

Also, as an alternative you can use [`cpplint`](https://github.com/cpplint/cpplint) from your command line to lint your code,

```shell
cpplint --recursive --root=.. --extensions=hh,cc .
```
