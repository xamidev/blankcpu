# UnnamedCPU

Continuing my exploration of the lower-level world...

This project is the implementation of a CPU in a high-level language, C. It aims to have a reduced (minimalist) instruction set and still be Turing complete.

# How to test

You can run a test program like that. I'll try making a developer's manual so one can make its own programs using the custom assembly here. 

```
make
./a.out program.asm
```

# What I'd like to do

- Finish the instruction set implementation
- Implement input/output devices (like a serial output that could be redirected to the host terminal)
- Re-do the CPU in a Hardware Description Language, or even better: with transistors IRL! (i probably won't waste my life doing that tho)
