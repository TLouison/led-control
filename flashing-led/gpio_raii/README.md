# GPIO RAII

As the name suggests, this is an RAII refactor of the `button/` codebase, focusing
on the RAII principles that make C++ much safer than when I first learned it in college.

## Learnings
- RAII principles
    - Acquire resources during construction, release them during destruction.
    - Move semantics vs. copy semantics (not really used in this codebase as
      the owner of the GPIOPins is always main so we don't need to reinitialize
      them).
- Modern C++ class design
