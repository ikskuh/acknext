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

enum.BITMAPFMT = 
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