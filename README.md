# maug

Augmented minimal standard library for C89 and legacy compilers.

**The API is currently in flux and is not yet completely final!**

Current maug-based projects may be found under [The maug topic on GitHub](https://github.com/topics/maug)!

## Design Constraints

Maug has the following design goals and constraints:

- Must work in C89 compilers. C99-like features like stdint or snprintf are
  available by internal implementation.
- Memory allocation uses a flat model (calloc/free), with no handle management.
- Standard library calls are allowed, so long as they are universally
  available even in very old C89 compilers.

## Roadmap

- Clearly enumerate which headers rely on which to create modular hierarchy.

- Solidify API.

- Setup Makefile to compile .solib/.dll/.ovl dynamic libs.

- Square-tile-based isometric engine.

- Reimplement retrotile with vectors for tiledefs.

- Replace iffy semi-vectors in mhtml/mcss with vectors.

- Replace obj vertice arrays with vectors in retroglu.

- Implement lambdas in mlisp with separators for pruning sequentally added env args.

- Implement resolution detection in Windows CE.

- Implement timing/blit mode setting options on all platforms.

## Modules

### Makefiles

Inside of the `make` subdirectory are a number of Makefiles that can be included from a project's Makefile in order to more conveniently build assets.

### retroflt

`#include <retroflt.h>`

RetroFlat is a rough, quick-and-dirty compatibility layer for making graphical programs that work on Win16 (32-bit via OpenWatcom's Win386), MS-DOS (32-bit via DOS/32a or DOS4GW via Allegro), and possibly other platforms in the future.

This documentation is also available at [https://indigoparadox.github.io/maug/group__maug__retroflt.html](https://indigoparadox.github.io/maug/group__maug__retroflt.html).

### mtypes

`#include <mtypes.h>`

Rough common approximation of types for interoperability between our projects.

Includes common types from stdint.h, guaranteed to be of the named size:

 * int8\_t
 * uint8\_t
 * int16\_t
 * uint16\_t
 * int32\_t
 * uint32\_t

When using this header, do not include stdint.h. This header will include it if the current platform supports it. Otherwise, it will improvise.

### uprintf

`#include <uprintf.h>`

An augmented printf for tracing, debugging, and error reporting.

To use, include uprintf.h and call `logging\_init()` in program startup.

Be sure to call `logging\_shutdown()` on program cleanup.

Logging can then be performed by calling:

 * `debug\_printf( lvl, format, ... )` - Prints debug message.
   * `lvl` - Immediacy level of debug message. See DEBUG\_THRESHOLD below.
   * `format` - `printf()`-style format string.
   * `...` - Variable arguments to `format`.
 * `error\_printf( format, ... )` - Prints to stderr.
   * `format` - `printf()`-style format string.
   * `...` - Variable arguments to `format`.

uprintf's behavior can be modified by defining the following before inclusion:

| Define            | Description                                             |
| ------------------|---------------------------------------------------------|
| platform\_file    | Type to use instead of stdlib's FILE for log ptr.       |
| platform\_fopen   | Function to use instead of fopen() to create a log ptr. |
| platform\_fclose  | Function to close log ptr.                              |
| platform\_fflush  | Function to flush log ptr to buffer.                    |
| platform\_fprintf | Function to print to log ptr with a format string.      |
| DEBUG\_THRESHOLD  | debug\_printf() w/ this or higher lvl will be displayed.|
| DEBUG\_LOG        | If defined, debug will be output to LOG\_FILE\_NAME.    |
| ANCIENT\_C        | Use static functions where vararg macros unsupported.   |

## Troubleshooting

### size of segment x exceeds 64k by y bytes

RetroFlat can be quite bulky for 16-bit DOS. Please try adding RETROFLAT\_DOS\_MEM\_LARGE=1 to your Makefile before Makefile.inc is included to enable a large code model.

### Cache loading errors which retrogxc.h is never included

For 16-bit builds, `GLOBAL\_DEFINES += -DRETROFLAT\_NO\_RETROGXC` must be specified in the Makefile explicitly, or the cache will be enabled in the DOS stubs.

