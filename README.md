# Aermoss AeNC
A programming language made with C++.

## Getting Started
1) To execute a AeNC code open command prompt and type:

```
main.exe <filename>
```

## Examples
# Hello World
``` basic
PRINT ("Hello, World!")
```

# Command Prompt
``` basic
VAR a = TRUE

WHILE (VAR a) [
    VAR command = INPUT ("> ")

    IF (VAR command == "EXIT") [
        PRINT ("Exiting...")
        VAR a = FALSE
    ]

    IF (VAR command != "EXIT") [
        SYSTEM VAR command
    ]
]
```
