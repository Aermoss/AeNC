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
print ("Hello, World!")
```

# Command Prompt
``` basic
var destroyed = false

while (not var destroyed) {
    var command = input ("> ")

    id (var command == "EXIT") {
        print ("Exiting...")
        var destroyed = true
    }

    if (var command != "EXIT") {
        system (var command)
    }
}
```
