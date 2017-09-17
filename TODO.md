# TODO List / Concept Ideas

## Fix / Update / Refactor
- add MODEL::minimumLOD, MESH::lodMask to ACFF


## Implement
- Instancing of ENTITY
	`int ent_addInstance(...)`
	`void ent_setInstance(VECTOR*,QUAT*,VECTOR*)
- MODEL
	- Animation
		- Eine Animation enthält für jeden verwendeten Bone eine
		  Sequenz aus Keyframes (position,rotation,skalierung)
		- Länge der Animation ist von vornherein festgelegt
		- Eigenschaften wie "looped" werden vom Programmierer vergeben
	- LOD for Meshes
		via LOD-Maske: mesh.visible = mesh.lodmask & (1<<lodstage)
- Collider API
	- "Collider Model" in addition to `hull_create` functions
	  to merge model loading with collider loading
	- How to: Animated models and collision?
		- 100% static colliders
		- bone-attached colliders
		- "colliders between bones"
- UI stuff
	- CEF Addon (HTML GUI because it's cool)
	- dear imgui integration
	- Minimal engine ui (Panels)
- Terrain System
	- Add "terrain compiler" for preprocessing terrain
		into custom model format
- Tools
	- Create model "compiler"
	- Create shader compiler (JSON + Files → SHD)
	- Blender export script (because fuck yeah)
- Add pure makefile based example
	- Use of resource compilation
	- Use of 
- File API
	- benutzerdefiniert (bereitstellung von read/write impl)
	- wrapper um streams (gzip inflate/deflate)
