MC2MT
====

This tool converts from Minecraft worlds to Minetest worlds.
It is very fast (limited mainly by zlib's compression speed) and multithreaded.

Features
---

  * Very fast (on the order of 10 000 blocks/sec, where a block is 16^3 nodes).
  * Converts doors properly, even when they cross block borders.
  * Lighting is correctly converted with no recalculation required.

Credits
----

Copyright (c) 2016 ShadowNinja <shadowninja@minetest.net>

License: LGPLv2.1+
