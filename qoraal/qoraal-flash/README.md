# qoraal-flash
FLASH tools

# NVOL
## Overview
NVOL is a persistent FLASH registry, often referred to as EEPROM emulation. It offers a basic API for reading and writing indexed key/value pairs to and from a FLASH memory, similar to a registry. It's important to mention that both the key and value are stored in FLASH, allowing for modification of the registry during runtime. Some of the features of NVOL include:

- Internal data management and wear leveling using 2 sectors.
- Configurable sector sizes.
- Reliability with robustness against power failures and asynchronous resets.
- Ensure data integrity with automatic recovery of corrupt sectors or entries during startup.
- Indexed lookup with a built-in hash table.
- Configurable key sizes and types.
- Configurable value sizes.
- Configurable value cache in RAM.
- Efficient and lightweight implementaiton.
- Support transactions.

The library provides two options to manage data storage: Option 1: Immediately save entries to FLASH memory whenever changed. Option 2: Store entries in RAM and save to FLASH memory on demand.

## Background
The disadvantage of a FLASH memory is that it cannot be erased or written in single bytes. FLASH memory can only be erased and written in large blocks. A typical erase
block size may be 4K or 8K bytes. For NVOL, the FLASH implementations should support partial writes, where an erased block may be written multiple times as long as bits only changed from “1” (erased state) to “0” (programmed state).

For the FLASH, the page is considered the smallest block size that can be erased. Each sector can be made up of one or more of these pages.

The first sector is initially erased. New registry entries are added sequentially to the FLASH. When an entry is updated, the old entry is marked as invalid and a new entry is written at the next available FLASH address. Once the first sector reaches capacity, all valid entries are copied to the second sector and the first sector is then erased. This process repeats itself.

NVOL efficiently handles and keeps track of valid entries and their locations on FLASH. The sectors are managed dynamically.


# NVOL String Registry Example

NVOL can efectively be tested in a Github codespace.Use the command from the codespace terminal ```$ sh build_and_run.sh```.


When the example program is launched, a command shell will open and it will display the following:
```
Navaro Qoraal Demo v 'Jan 28 2025'
Enter 'help' or '?' to view available commands. 

[Qoraal] #>
```

First, we will run a simple script to populate our registry with values. At the prompt type:
```
source reg.sh
```

This should fill the registry with values. To list all the commands for testing the registry type ``` ? reg```, it should display the following list:

```
# >? reg
reg [key] [value]
regadd <key> <value>
regdel <key>
regerase 
regstats 
regtest [repeat]
# >
```

We can now use the reg command to view and update the registry:

1. list the entire contents of the regitry:
```
# >reg
player.age:             Ancient
player.gender:          Otherworldly
player.level:           legendary
player.location:        Mount Olympus
player.name:            cool_cat
player.points:          9000
player.power:           invisibility
player.species:         Dragon
player.status:          awakened
player.weapon:          lightning bolt
test:                   123
user.address:           123 Main St.
user.age:               30
user.children:          0
user.email:             johnsmith@example.com
user.favorite_color:    blue
user.gender:            male
user.marital_status:    single
user.name:              John Smith
user.occupation:        contributor
user.phone_number:      555-555-5555

    21 entries found.
```

2. Change the "favorite_color" of the user:
```
# >reg user.favorite_color yellow
OK
```

3. View the updated entry:
```
# >reg user.favorite_color
user.favorite_color:    yellow
```

4. Delete an entry:
```
# >regdel user.gender
OK
```

5. Filter only "player" entries:
```
# >reg player
player.age:             Ancient
player.gender:          Otherworldly
player.level:           legendary
player.location:        Mount Olympus
player.name:            cool_cat
player.points:          9000
player.power:           invisibility
player.species:         Dragon
player.status:          awakened
player.weapon:          lightning bolt

    10 entries found.
```

6. Look at the status of the registry:
```
# >regstats
NVOL3 : : '_regdef_nvol3_entry' 20 / 255 records loaded
record  : 256 recordsize
        : 0x000000 1st sector version 0x0155 flags 0xaaaaffff
        : 0x010000 2nd sector version 0x0000 flags 0xffffffff
        : 0x010000 sector size
        : 20 loaded
        : 20 inuse
        : 2 invalid
        : 0 error
        : 640 lookup table bytes
        : 53 dict hash size
        : dict hash - max 2, empty 34, used 19
```

These commands are all implemented in ```src/registry/registrycmd.c```. The implementation is intuitive and self-explanatory and should requiring no further explanation.

The shell is a project in and of itself, but is only included in this example for demonstration purposes. It is easy to extend. Use ```?``` to see the complete list of commands implemented for this example.

# NLOG Example

NLOG is a FLASH log writing logs over several sectors and deleting the next sector to continue the circular logging. There is a simple API to append logs and an iterator to retrive log messages again.

First, we will run a simple script to populate our system and assert logs with values. At the prompt type:
```
source log.sh
```

To view the contents of the log, enter the command ```log```:
```
[Qoraal] #> log
severity<=6   (log [severity] [count])
---------------------------------------
000014 (0) - 1970-02-03 10:49:39:  og message 3 LOG
000013 (0) - 1970-02-03 10:49:39:  og message 3 REPORT
000012 (0) - 1970-02-03 10:49:39:  og message 3 WARNING
000011 (0) - 1970-02-03 10:49:39:  og message 3 ERROR
000010 (0) - 1970-02-03 10:49:39:  og message 3 ASSERT
000009 (0) - 1970-02-03 10:49:38:  og message 2 LOG
000008 (0) - 1970-02-03 10:49:38:  og message 2 REPORT
000007 (0) - 1970-02-03 10:49:38:  og message 2 WARNING
000006 (0) - 1970-02-03 10:49:38:  og message 2 ERROR
000005 (0) - 1970-02-03 10:49:38:  og message 2 ASSERT
000004 (0) - 1970-02-03 10:49:37:  og message 1 LOG
000003 (0) - 1970-02-03 10:49:37:  og message 1 REPORT
000002 (0) - 1970-02-03 10:49:37:  og message 1 WARNING
000001 (0) - 1970-02-03 10:49:37:  og message 1 ERROR
000000 (0) - 1970-02-03 10:49:37:  og message 1 ASSERT
```

To view only the error and assert logs can be done as follow:

Note: On the demo platform to seperate instances of the logger is created, one for logging all messages and a second one logging only the assert messages.

# Porting
