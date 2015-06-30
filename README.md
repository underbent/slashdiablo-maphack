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
Mustache Default:	stash
Mustache[stats]: {{#defense}}\n\n    >{{defense}} defense{{/defense}}{{#stats}}\n\n    > {{value}}{{#range}} ({{min}}-{{max}}){{/range}} {{^skill}}{{name}}{{/skill}}{{skill}}{{/stats}}
Mustache[header-unique]: {{#quality=Unique}}**{{^name}}{{type}}{{/name}}{{name}}** (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header-magic]: {{#quality$Magic|Rare}}**{{^name}}{{type}}{{/name}}{{name}}** (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header-else]: {{#quality^Unique|Magic|Rare}}{{^isRuneword}}{{^name}}{{type}}{{/name}}{{name}}{{/isRuneword}}{{#isRuneword}}**{{runeword}}** {{type}}{{/isRuneword}} (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header]: {{>header-unique}}{{>header-magic}}{{>header-else}}{{#count}} **x{{count}}**{{/count}}
Mustache[item]: {{>header}}{{>stats}}{{^isRuneword}}{{#socketed}}\n\n  * {{>>item}}{{/socketed}}{{/isRuneword}}\n
Mustache[stash]: {{#this}}* {{>item}}\n\n{{/this}}
```

Github no longer offers binary downloads, so to get the latest release
(currently v0.1.5) or any older release go to
http://bitly.com/bundles/underbent/1.

#Release Notes for BH Maphack v1.7a
- A fork of Underbent's v1.6 by Slayergod13

##Updates to Underbent's v1.6 changes:
- BH.Injector
	- Refactored the injection process so that it no longer executes the core maphack logic inside of the loader lock.
		- This resulted in a minor frame rate increase
		- More importantly it allowed the BH.dll to load the Stormlib.dll for the purpose of reading the MPQ files
	- No longer needs to load Stormlib.dll
	- No longer writes out temporary mpq text files
	- Fixed a bug where opening the injector without any windows open would cause the injector to crash
- BH.dll
	- Now loads the MPQ data inside the maphack
- Item Module
	- Now relies on the data read from the MPQ files within the maphack dll
	
##New Features & Bug Fixes:

###BH Config
- Can now read lines of arbitrary length
- Fixed a bug where lines with a single '/' would be truncated instead of waiting for a double slash "//"

###StashExport
- New Module Capable of exporting the current characters inventory in [JSON](http://www.json.org) or custom formats using mustache templates
- Uses the MPQ data to figure out the item information
- Templates can be specified in the BH.cfg using mustache syntax: https://mustache.github.io/mustache.5.html
     - Subset of mustache implemented (and some additions):
          - Literals
               - Support for SOME escape characters added (\r\n\t)
          - Variables
          - Partials
              - Added ability to isolate the child scope to prevent infinite recursion in partials (the context would no longer have access to its parent context)
              - {{>partial}} {{>>isolated-partial}}
          - Sections
               - Inverse
               - Conditional (for truthy values)
               - Iterator (for arrays)
               - And some new additions:
                    - Comparisons:
                         - String Equality: {{#key=value}}
                         - String Inequality: {{#key!value}}
                         - Float Greater: {{#key>value}}
                         - Float Less: {{#key<value}}
                         - String In Set: {{#key$value1|value2|value3}}
                         - String Not In Set: {{#key^value1|value2|value3}}
- Added several data structures to support the StashExport module
	 - JSONObject - Used to contain the item data in a generic fashion, also makes the templating MUCH easier
	 - TableReader/Table - Used to read the txt/tbl files in the data directory, these files are used for parsing the item stats
	 - MustacheTempalte - Used for templating text

#### Features
-  Can identify item quality
-  Can identify which unique/set/runeword the item is
-  Can identify the magix prefix/suffixes
-  Attempts to collapse known aggregate stats (all res) using the aggregate name
-  Will collapse identical items into a single entry with a count (useful for runes and gems)
-  Can exclude stats on items that are fixed so that only the important stats are shown
-  Can get stats for jewels that have been placed into a socketed item
-  See sample output further down

###D2Structs
- Adjusted some structures to better state the purpose of some previously "unknown" or unspecified bytes

###ScreenInfo
- Added display for current/added/rate of gain for experience
	 - BH Toggle: "Experience Meter"
	 
###Maphack
- Refactored the rendering pipeline for the automap objects (monsters, items, missiles, etc) so that the frames could be recycled. 
	- This allows the system to reuse calculations from previous frames and only store the draw commands.
	- This can result in a large frame rate increase on slower machines
- Added ability to display chests on the automap

###ItemDisplay
- The predicate parser will no longer use exceptions for control flow.
	- The old design was resulting in a large frame rate penalty that has been aleviated
	
##New Configuration Items & Defaults:
```
// Maphack section:
// Toggles whether or not to show chests on the automap
Show Chests:			True, VK_X
// Controls how many frames to recycle the minimap doodads for (higher values save more frames)
Minimap Max Ghost: 20

// Experience Display
Experience Meter:		True, VK_NUMPAD7

// Stash Export
// Mustache Templates
Mustache Default:	stash
Mustache[stats]: {{#defense}}\n\n    >{{defense}} defense{{/defense}}{{#stats}}\n\n    > {{value}}{{#range}} ({{min}}-{{max}}){{/range}} {{^skill}}{{name}}{{/skill}}{{skill}}{{/stats}}
Mustache[header-unique]: {{#quality=Unique}}**{{^name}}{{type}}{{/name}}{{name}}** (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header-magic]: {{#quality$Magic|Rare}}**{{^name}}{{type}}{{/name}}{{name}}** (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header-else]: {{#quality^Unique|Magic|Rare}}{{^isRuneword}}{{^name}}{{type}}{{/name}}{{name}}{{/isRuneword}}{{#isRuneword}}**{{runeword}}** {{type}}{{/isRuneword}} (L{{iLevel}}){{#sockets}}[{{sockets}}]{{/sockets}}{{/quality}}
Mustache[header]: {{>header-unique}}{{>header-magic}}{{>header-else}}{{#count}} **x{{count}}**{{/count}}
Mustache[item]: {{>header}}{{>stats}}{{^isRuneword}}{{#socketed}}\n\n  * {{>>item}}{{/socketed}}{{/isRuneword}}\n
Mustache[stash]: {{#this}}* {{>item}}\n\n{{/this}}
```

##Stash Export Sample:
Raw JSON:
````json
[
  {
    "iLevel": 4,
    "name": "Viridian Small Charm of Life",
    "quality": "Magic",
    "stats": [
      {
        "name": "maxhp",
        "value": 10
      },
      {
        "name": "poisonresist",
        "value": 7
      }
    ],
    "type": "Small Charm"
  },{
    "defense": 98,
    "iLevel": 17,
    "quality": "Rare",
    "socketed": [
      {
        "iLevel": 1,
        "isGem": true,
        "quality": "Normal",
        "type": "Ruby"
      },
      {
        "iLevel": 1,
        "isGem": true,
        "quality": "Normal",
        "type": "Sapphire"
      }
    ],
    "sockets": 2,
    "stats": [
      {
        "name": "item_armor_percent",
        "value": 29
      },
      {
        "name": "tohit",
        "value": 15
      },
      {
        "name": "normal_damage_reduction",
        "value": 1
      },
      {
        "name": "fireresist",
        "value": 10
      },
      {
        "name": "item_lightradius",
        "value": 1
      },
      {
        "name": "item_fastergethitrate",
        "value": 17
      }
    ],
    "type": "Chain Mail"
  },{
    "iLevel": 99,
    "name": "Annihilus",
    "quality": "Unique",
    "stats": [
      {
        "name": "all-stats",
        "range": {
          "max": 20,
          "min": 10
        },
        "value": 14
      },
      {
        "name": "res-all",
        "range": {
          "max": 20,
          "min": 10
        },
        "value": 16
      },
      {
        "name": "additional xp gain",
        "range": {
          "max": 10,
          "min": 5
        },
        "value": 6
      }
    ],
    "type": "Small Charm"
  },{
    "iLevel": 87,
    "name": "Wizardspike",
    "quality": "Unique",
    "type": "Bone Knife"
  },
  {
    "defense": 168,
    "iLevel": 80,
    "isRuneword": true,
    "quality": "Normal",
    "runeword": "Spirit",
    "socketed": [
      {
        "iLevel": 1,
        "isRune": true,
        "quality": "Normal",
        "type": "Tal Rune"
      },
      {
        "iLevel": 1,
        "isRune": true,
        "quality": "Normal",
        "type": "Thul Rune"
      },
      {
        "iLevel": 1,
        "isRune": true,
        "quality": "Normal",
        "type": "Ort Rune"
      },
      {
        "iLevel": 1,
        "isRune": true,
        "quality": "Normal",
        "type": "Amn Rune"
      }
    ],
    "sockets": 4,
    "stats": [
      {
        "name": "mana",
        "range": {
          "max": 112,
          "min": 89
        },
        "value": 100
      },
      {
        "name": "cast3",
        "range": {
          "max": 35,
          "min": 25
        },
        "value": 25
      },
      {
        "name": "abs-mag",
        "range": {
          "max": 8,
          "min": 3
        },
        "value": 7
      }
    ],
    "type": "Kurast Shield"
  },{
    "count": 8,
    "iLevel": 1,
    "isGem": true,
    "quality": "Normal",
    "type": "Chipped Ruby"
  },
  {
    "count": 7,
    "iLevel": 1,
    "isGem": true,
    "quality": "Normal",
    "type": "Flawed Emerald"
  }
 ]
````
Using the template above:
````
* **Viridian Small Charm of Life** (L4)

    > 10 maxhp

    > 7 poisonresist

* **Chain Mail** (L17)[2]

    >98 defense

    > 29 item_armor_percent

    > 15 tohit

    > 1 normal_damage_reduction

    > 10 fireresist

    > 1 item_lightradius

    > 17 item_fastergethitrate

  * Ruby (L1)


  * Sapphire (L1)
  

* **Annihilus** (L99)

    > 14 (10-20) all-stats

    > 16 (10-20) res-all

    > 6 (5-10) additional xp gain
    

* **Wizardspike** (L87)


* **Spirit** Kurast Shield (L80)[4]

    >168 defense

    > 100 (89-112) mana

    > 25 (25-35) cast3

    > 7 (3-8) abs-mag
    
    
* Chipped Ruby (L1) **x8**

* Flawed Emerald (L1) **x7**
````
Which renders as:
* **Viridian Small Charm of Life** (L4)

    > 10 maxhp

    > 7 poisonresist

* **Chain Mail** (L17)[2]

    >98 defense

    > 29 item_armor_percent

    > 15 tohit

    > 1 normal_damage_reduction

    > 10 fireresist

    > 1 item_lightradius

    > 17 item_fastergethitrate

  * Ruby (L1)
  * Sapphire (L1)
  

* **Annihilus** (L99)

    > 14 (10-20) all-stats

    > 16 (10-20) res-all

    > 6 (5-10) additional xp gain
    

* **Wizardspike** (L87)


* **Spirit** Kurast Shield (L80)[4]

    >168 defense

    > 100 (89-112) mana

    > 25 (25-35) cast3

    > 7 (3-8) abs-mag
    
* Chipped Ruby (L1) **x8**

* Flawed Emerald (L1) **x7**