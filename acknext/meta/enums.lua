enum.STAGETYPE = 
{
	postfix = "_STAGE",
	"Entity",
	"PostProcessing",
	"Custom",
}

enum.LIGHTTYPE = 
{
	postfix = "_LIGHT",
	"Ambient",
	"Directional",
	"Point",
	"Spot",
}

enum.BUFFERTYPE = 
{
	postfix = "BUFFER",
	"VERTEX",
	"INDEX",
};

enum.SHADERTYPE = 
{
	postfix = "SHADER",
	"VERTEX",
	"FRAGMENT",
	"GEOMETRY",
	"TESSCTRL",
	"TESSEVAL",
};

enum.PIXELFORMAT = 
{
	prefix = "FORMAT_",
	"Rgb8",
	"Rgba8",
	"Rgba16f",
	"Rgba32f",
}

enum.TEXTURETYPE = 
{
	prefix = "TEX_",
	"1D",
	"1D_Array",
	"2D",
	"3D",
	"2D_Array",
}

enum.SHADERVAR = 
{
	postfix = "_VAR",
	"matWorld",
	"matView",
	"matProj",
	"matWorldView",
	"matViewProj",
	"matWorldViewProj",
	-- TODO: Complete!
	"vecViewDir",
	"vecViewPos",
	"vecViewPort",

	"vecColor",
	"vecEmission",
	"vecAttributes",
	"texColor",
	"texAttributes",
	"texEmission",
}

enum.ERROR = 
{
	prefix = "ERR_",
	"NoError",
	"OutOfMemory",
	"InvalidArgument",
	"InvalidOperation",
	"SDL",
	"OutOfBounds",
	"FileSystem",
}

enum.TASKSTATE = 
{
	prefix = "TASK_",
	"DEAD",
	"READY",
	"SUSPENDED",
	"DISABLED",
	"RUNNING",
}

enum.CAMERATYPE =
{
	"Perspective",
	"Isometric",
	"Custom_Projection",
}