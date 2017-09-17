Shader
{
	Vertex {
		Reference "/builtin/object.vert"
	},
	Fragment {
		Reference "/builtin/gamma.glsl",
		Source "myshader.frag"
	}
}

{
	"type": "acknext-shader",
	"vertex": [
		{ "type": "ref", "value": "/builtin/object.vert" }
	],
	"fragment": [
		{ "type": "ref", "value": "/builtin/gamma.glsl" },
		{ "type": "src", "value": "myshader.frag" }
	]
}

<shader>
	<vertex>
		<ref>/builtin/object.vert</ref>
	</vertex>
	<fragment>
		<ref>/builtin/gamma.glsl</ref>
		<src>myshader.frag</src>
	</fragment>
</shader>

vertex-ref:   /builtin/object.vert
fragment-ref: /builtin/gamma.glsl
fragment-src: ./myshader.frag