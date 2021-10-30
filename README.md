# Aermoss AeNC
A programming language made with C++.

## Getting Started
1) To execute a AeNC code open command prompt and type:

```
main <filename>
```

## Examples
# Hello World
``` basic
include "ioaenc"

func main() {
    ioaenc.print("Hello, World!")
}
```

# Command Prompt
``` basic
include "ioaenc"

func main() {
    var destroyed = false

    while (not var destroyed) {
        var command = ioaenc.input(">")

        id (var command == "exit") {
            ioaenc.print("Exiting...")
            var destroyed = true
        }

        if (var command != "exit") {
            system(var command)
        }
    }
}
```
