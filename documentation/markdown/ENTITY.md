# ENTITY

## Definition

```c
struct ENTITY
{
	union {
		struct VECTOR position;
		struct {
			var x, y, z;
		};
	};
	QUATERNION rotation;
	ENTITY * parent;
	MATERIAL * material;
	enum FLAGS flags;
};
```

### Synposis
An object in the 3D world, may be associated with a model and has a collision hull.

### Fields

- position : [[VECTOR]]
	A [[VECTOR]] containing the position of the entities origin.
- x, y, z : [[var]]
	An alias for the entities `position`
- rotation : [[QUATERNION]]
  A [[QUATERNION]] containing the entities rotation.
- parent : [[ENTITY]]*
  A pointer to the entities parent. If this is set, the position and rotation is
	calculated in the `parent`s coordinate space instead of the global space.
- material : [[MATERIAL]]*
  Overrides the default materials this entity uses. If this field is not **NULL**,
	the set material will be used instead of the one defined in the models meshes.
- flags : [[ENTITYFLAGS]]
  A set of flags that modify the behaviour of this entity.

## See Also
- [[Scene]]
- [[ent_create]], [[ent_remove]]