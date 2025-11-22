# ACLIB - A Cool Library

Aclib is a headerfile library for C, that implements a lot of quality of life stuff, inspired by other languages' stdlib.

## Usage

Just include the headerfile, and in the file you want the implementations, define `ACLIB_IMPLEMENTATION` above the `#include`

```c
#define ACLIB_IMPLEMENTATION
#include "aclib.h"

int main() {
    // Stuff
}
```
