This page has some ideas and plans for the future, that were not implemented yet, and the planned processes on how this repository should be organized.


# Structure for each project
1. Short description
2. Summary of new components used/new techniques
3. BOM - bill of materials
4. Circuit diagram
5. Recap
6. [Optional] Extra credit: List of extensions to the project


# Coding Conventios
The structure of Arduino sketches is similar:
- `setup` function
- `loop` function

We also need to configure a few constants

## Comments
I use the following section comments at the beginning of a section
```c
// *** SECTION NAME ***
```

## Configuration
For simple programs, the configuration is in the main file (`*.ino`) under the Configuration section. For bigger projects, the configuration is in a separate file (`Configuration.h`) te keep things nice and simple, and for easier reconfiguration. 

# Administrativia / TODO:
These are not to me, with future plans

- [x] Week 2: playing a tune with the buzzer
- [x] Week 3: building the hi-tech office nagger
  - [ ] Adjust pitch and interval with the knob:
    - [ ] Use the switch on the knob to move between pitch and interval adjustments
- [ ] Week 4: music memory game with switches
- [ ] Week 5: Using the screen: draw the pitch/interval on the screen
- [ ] Clock + temperature display with RTC component
- [ ] Perfect hour timer
      Set the length of the perfect hour, start the timer, bzzz when the time is up
- [ ] Building a flip-flop (don't have transistors)

## Ideas
- https://www.thingiverse.com/thing:2846251
