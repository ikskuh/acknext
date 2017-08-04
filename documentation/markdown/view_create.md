# view_create

## Synopsis
Creates a new view.

## Definition
```c
VIEW * view_create(WIDGET * parent);
```

### Parameters

- parent : [[WIDGET]]
	The parent widget that will contain this view.
- foo
	bar baz baz
	
### Returns
The newly created [[VIEW]].

## Example
```c
// test the floor texturevec_set(temp, my.x);
temp.z -= 500; // trace downwards 500 quants below
c_trace (my.x,temp,IGNORE_ME|IGNORE_PASSABLE|IGNORE_MODELS|IGNORE_SPRITES|SCAN_TEXTURE); 
// now TEX_NAME is set to the floor name below the MY entity
...
// look if the YOU enemy can be shot at from my position, and if yes, hurt him
c_trace (my.x,your.x,IGNORE_ME|IGNORE_PASSABLE|ACTIVATE_SHOOT); 
// if entity YOU was visible from MY position, its SHOOT event was triggered

while(true)
{
	void;
	yield();
}
```

## See also
- [[Gui]]
- [[Scene]]