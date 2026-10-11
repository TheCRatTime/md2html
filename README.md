TH "README"
# md2html
**md2html** - generate web pages.

## Description
This is a small generator for HTML from
Markdown markup.

There is a changed [md2man](https://github.com/TheCRatTime/md2man/),
just changed escape codes to roff markup.

### Features
- Minimal memory usage: (state-machine logic).
- Fast conversion.

### Build
Needed:
- C Compiler
- CMake 3.10 and greater
- GNU Make

```bash
cmake -B build
cd build/
make
```

### Authors
TheCRatTime

### License
Project is under Apache 2.0 license.

### See also
After build, you can show this README at HTML:

```bash
# in build
./md2html ../README.md > readme.html
# w3m browser
w3m readme.html
```

