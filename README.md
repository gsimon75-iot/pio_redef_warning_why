# pio_redef_warning_why
Investigating why a 'warning: &lt;symbol> redefined' happens in a stripped-down PlatformIO project 

## Symptom

During the comment-removal phase the `#if`-s in a `.ino` files aren't evaluated (and that's correct) but copied verbatim to the output, but for some reason `#define`-s **are**, so a construct like this:
```
#if ...
#   define XYZ 1
#else
#   define XYZ 2
#endif
```
will cause warnings about re-defining the symbol `XYZ`.


## The direct cause

The preprocessor is invoked as [here](https://github.com/platformio/platformio-core/commit/8a379d2db26ff0deb37be83e82d1abe72e5439f8#diff-9efd738690f0ff6a543df21bae839fc9R86):
```
xtensa-lx106-elf-g++ -o "/tmp/test/src/main.ino.cpp" -x c++ -fpreprocessed -dD -E "/tmp/tmpsojq53"
/tmp/test/src/main.ino:9:0: warning: "GPIO_PWM" redefined [enabled by default]
# define GPIO_PWM  12
^
/tmp/test/src/main.ino:4:0: note: this is the location of the previous definition
# define GPIO_PWM  4
^
```

The [manpage of g++](https://linux.die.net/man/1/g++) is somewhat vague about `-fpreprocessed`, `-E` and `-dD`:

> This suppresses things **like** macro expansion, trigraph conversion, escaped newline splicing, and processing of **most** directives...
> In this mode the integrated preprocessor is **little more than** a tokenizer for the front ends. 
> With -E, preprocessing is limited to the handling of directives **such as** "#define", "#ifdef", and "#error".
> With -fpreprocessed, predefinition of command line and **most** builtin macros is disabled.

Taking a look at the output, it seems that macro expansion is disabled, but `#define`-s are parsed and evaluated nevertheless.

As `#if`/`#else`/`#endif` is not interpreted (without macro expansion it wouldn't make much sense either), multiple but mutually exclusive `#defines` behave like redefinitions.


## Workarounds

Shutting the preprocessor warnings with `-Wp,-w` would hide some otherwise important warnings as well, so it's discouraged.

Another option is that in those conditional blocks we define all symbols with undefining them first:
```
#if ...
#   undef XYZ
#   define XYZ 1
#else
#   undef XYZ
#   define XYZ 2
#endif
```

But it also sileces the case when someone has (unintentionally) defined them previously, but at least only for these symbols, and not for all of them in general.

The option is to do it the 'Arduino' way, use the fact that the `.ino` with `void setup()`/`void loop()` gets first when all `.ino`-s are merged together, and put all `#define`-s not in a separate header but at the beginning of this 'main' `.ino` file.


All of these options are ugly, but so is the [original behaviour](Just a note:
PlatformIO behaves just as it supposed to, the problem exists with the original Arduino tools as well: https://github.com/arduino/Arduino/issues/1841)

Case closed.



