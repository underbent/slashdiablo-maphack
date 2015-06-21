slashdiablo-maphack
===================

A customized maphack for reddit's slashdiablo D2 server

This maphack is based on BH maphack, written by McGod from the blizzhackers
forum. It was extensively customized for the slashdiablo realm by Deadlock39,
who created versions 0.1.1 and 0.1.2.

Currently works with client version 1.13C.

Major features include:

* Full maphack
 * Monsters, missiles displayed on map
 * Infinite light radius
 * Configurable monster colors (see wiki for details)
 * Indicators of current level's exits
* Configurable item display features (see wiki for details)
 * Modify item names and add sockets, item levels, ethereality
 * Change colors and display items on the map
* One-click item movement
 * Shift-rightclick moves between stash/open cube and inventory
 * Ctrl-rightclick moves from stash/open cube/inventory to ground
 * Ctrl-shift-rightclick moves from stash/inventory into closed cube
* Auto-party (default hotkey: 9)
* Auto-loot (default hotkey: 7)
* Use potions directly from inventory (default hotkeys: numpad * and -)
* Display gear of other players (default hotkey: 0)
* Screen showing secondary attributes such as IAS/FHR (default hotkey: 8)
* Warnings when buffs expire (see "Skill Warning" in config file)
* Stash Export
 * Export the inventory & stash of the current character to an external file
* Experience Meter
 * Show the current %, % gained, and exp/sec above the stamina bar

The hotkeys for all features can be changed in the config file.

Stash Exporting is configured through [Mustache Templates](https://mustache.github.io/mustache.5.html), see sample below:

Add this to the bottom of your BH.cfg:
```
// Stash Export
// Mustache Templates
Mustache[stats]: {{#defense}}\n\n    >{{defense}} defense{{/defense}}{{#stats}}\n\n    > {{value}} {{^skill}}{{name}}{{/skill}}{{skill}}{{/stats}}
Mustache[header-unique]: {{#quality=Unique}}**{{^name}}{{type}}{{/name}}{{name}}** (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header-magic]: {{#quality$Magic|Rare}}**{{^name}}{{type}}{{/name}}{{name}}** (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header-else]: {{#quality^Unique|Magic|Rare}}{{^isRuneword}}{{^name}}{{type}}{{/name}}{{name}}{{/isRuneword}}{{#isRuneword}}**{{runeword}}** {{type}}{{/isRuneword}} (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header]: {{>header-unique}}{{>header-magic}}{{>header-else}}
Mustache[item]: {{>header}}{{>stats}}{{^isRuneword}}{{#socketed}}\n\n  * {{>>item}}{{/socketed}}{{/isRuneword}}\n
Mustache[stash]: {{#this}}* {{>item}}\n\n{{/this}}
```

Github no longer offers binary downloads, so to get the latest release
(currently v0.1.5) or any older release go to
http://bitly.com/bundles/underbent/1.
