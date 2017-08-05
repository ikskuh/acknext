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
	"INDEX",
	"GEOMETRY",
	"TESSCTRL",
	"TESSEVAL",
};

enum.PIXELFORMAT = 
{
	prefix = "FORMAT_",
	"Rgb",
	"Rgba",
	"Rgba16f",
	"Rgba32f",
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

enum.FILETYPE = 
{
	postfix = "_FILE",
	"Physical",
	"Virtual",
}

enum.ERROR = 
{
	prefix = "ERR_",
	"NoError",
	"OutOfMemory",
	"InvalidArgument",
	"InvalidOperation",
	"SDL",
}