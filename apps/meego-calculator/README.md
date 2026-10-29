# meego-calculator

The Nokia N9's calculator, written anew in QML with the MeeGo Harmattan
Qt Quick components (`com.nokia.meego 2.0`), for the MeeGo session on
Maemo Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's
"Calculator" is closed source; this app imitates its look and behaviour,
it contains none of Nokia's code or files (nor Nemo's: the code is new).

- **The calculation line** above, in the calculator's orange: the number
  being written or the calculation so far (`1234,5×3`), smaller as it
  grows; backspace at its side (a long press clears); a long press on the
  line copies it or pastes a number.
- **The keys** on black, the N9's yellow square under a pressed one: the
  numbers with the decimal point (the locale's) and `=`, the functions
  C, ±, ÷, ×, −, +; in portrait the functions above the numbers, in
  landscape beside them.
- One operation at a time, as the N9's: a second operator first works
  out the one before (`2 + 3 ×` shows `5×`). Up to 15 digits; results
  with 12 significant digits (no `0.30000000000000004`), very large or
  small ones with an exponent; division by zero is "Not defined".
- The keyboard works too: digits, `.` or `,`, `+ - * /`, Enter or `=`,
  Backspace, Escape or Delete to clear.

## The look: the N9's theme

Through `image://theme/`: the pressed keys' squares
(`meegoui-calculator-button-down`, `-function-button-down`), the
backspace icon, the components' own. These are Nokia's (the "blanco"
theme of one's own N9 or N950) and are not in this repository; without
them the keys still work, only without the square.

## Building

Qt 5 (Quick) and qt-components' `com.nokia.meego` 2.0 at run time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
