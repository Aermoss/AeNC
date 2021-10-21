# Aermoss AeNC
A programming language made with C++.

## Getting Started
1) Run the interpreter and type your AeNC code file name

## Examples
# Hello World
``` basic
PRINT "Hello, World!"
```

# Command Line
``` basic
VAR a = TRUE

WHILE (VAR a) [
    INPUT "> " VAR command

    IF (VAR command == "EXIT") [
        PRINT "Exiting..."
        VAR a = FALSE
    ]

    IF (VAR command != "EXIT") [
        SYSTEM VAR command
    ]
]
```
