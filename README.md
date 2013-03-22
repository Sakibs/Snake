My adaptation of the popular snake game made the ncurses library in C for the terminal.


Version Log
-----------
v1.3
External
- implemented food
- snakey grows when eating food
- beep sounds when eating food
- cursor is now hidden
- snakey can no longer turn around and eat his own neck
Internal
- snakey no longer an array, he is now represented by a struct with a length variable and an array of parts inside the struct.
- efficient functions to handle menu display
Bugs
- when eating the first bite of food, there is a delay in the beep
- when resetting, the food for some reason is not reset even though the set_food_coord function is called!
Todo
- add score system
- better menu interface

v1.2
- set head of snake to index 0
- added collision detection for head to body
- added game over functionality
- added reset functionailty
- created window to display menu

v1.1
- added pause/play functionality

v1.0
- completed tutorial : http://xn--ncoder-9ua.dk/blog/2010/04/using-ncurses-in-c-for-text-user-interfaces-featuring-xcode/


REFERENCES
----------
http://xn--ncoder-9ua.dk/blog/2010/04/using-ncurses-in-c-for-text-user-interfaces-featuring-xcode/
