# Aermoss AeNC
A programming language made with C++.

## Getting Started
1) To execute a AeNC code open command prompt and type:

```
main <filename>
```

## Examples
# Hello World
``` go
include "ioaenc"

func main() {
    ioaenc.print("Hello, World!")
}
```

# Command Prompt
``` go
include "ioaenc"

func main() {
    var command = ""
    var destroyed = false

    while (not destroyed) {
        command = ioaenc.input(">")

        if (command == "exit") {
            ioaenc.print("Exiting...")
            destroyed = true
        }

        if (command != "exit") {
            system(command)
        }
    }
}
```
