# TODO List / Concept Ideas

## Fix / Update / Refactor

- Store textures without gamma (ALWAAAAYS)

## Implement
- MODEL
- Collider API
	- "Collider Model" in addition to `hull_create` functions
	  to merge model loading with collider loading
	- How to: Animated models and collision?
		- 100% static colliders
		- bone-attached colliders
		- "colliders between bones"
- UI stuff
	- Minimal engine ui (Panels)
- Terrain System
	- Add "terrain compiler" for preprocessing terrain
		into custom model format
- Tools
	- Create model "compiler"
	- Create shader compiler (JSON + Files → SHD)
	- Allow adding external resources in MIEP
- Add pure makefile based example
	- Use of resource compilation
- File API
	- benutzerdefiniert (bereitstellung von read/write impl)
	- wrapper um streams (gzip inflate/deflate)
	- backref und marker

## Fancy Stuff
- ATX GIMP Plugin
	- Allow export of 1D, 1D Array, 2D Array and 3D textures
- /usr/bin/file magic.mgc file
	- Add more sugar
- Blender export script (because fuck yeah)
	- Start with it
	
Graphics Pipeline:
plan fürs postprocessing: SSAO, Bloom/HDR, Reinhard Tone Mapping als Default-Pipeline
Forward+ für mehr Licht pro Level
und transparenz mach ich erst mal nix spannendes, nur halt mit forward+ beleuchten, weil cool
ach und Screen Space Reflections sollte ich auch noch reinmachen, hab ja schließlich PBR und das kommt mit Spiegelungen einfach immer geiler
und ein sky a'la https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter16.html