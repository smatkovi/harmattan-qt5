# meego-calendar

The Nokia N9's calendar, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`, the Qt 5 port of qt-components),
for the MeeGo session on Maemo Leste, Arch Linux ARM and FreeBSD and for
Hildon. Nokia's calendar is closed source; this app imitates its look and
behaviour, it contains none of Nokia's code or files (nor Nemo's: the
code is new).

- **Month**: the month and year on the N9's bar with the arrows to the
  months before and after, the weekdays from the locale's first day,
  six weeks of days on the light grid; today and the selected day in the
  N9's cells, a dot under the days with events. A sideways swipe over the
  days changes the month. Beside the grid (landscape) or below it
  (portrait): the selected day's events, each with its colour, time and
  place; "No events" adds one when tapped.
- **Tool bar**: today, a new event, the menu (go to a date with the date
  picker, the upcoming events of the next 60 days).
- **Event**: its title on its colour, the day and time, the place, the
  repetition, the reminder, the description; change it (the same sheet)
  or delete it (with its repetitions).
- **New or changed event** (a sheet, Cancel and Save): title, place, all
  day, starts and ends (the components' date and time pickers; moving the
  start keeps the length), repeat (never, every day, week, month, year),
  alert, one of six colours, description.

Events are kept in a SQLite database of the app's own
(`~/.local/share/meego/meego-calendar/events.sqlite`); repetitions are
unfolded for the days shown. The alert is saved with the event but does
not ring yet (alarmd's events, as meego-clock makes them, are next).

## The look: the N9's theme

The app draws with the meegotouch theme's pictures through
`image://theme/`: the calendar's month bar, grid, day cells and event dots
(`meegotouch-calendar-*`, `meegotouch-monthgrid-daycell-*`), the tool bar
icons and the components' own. These are Nokia's (the "blanco" theme of
one's own N9 or N950) and are not in this repository; without them the
app works with plain colours.

## Building

Qt 5 (Quick, SQL with its SQLite driver) and qt-components'
`com.nokia.meego` 2.0 at run time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
