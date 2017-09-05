enum.STAGETYPE = 
{
	postfix = "_STAGE",
	"Entity",
	"PostProcessing",
	"Custom",
}

enum.SHADERVAR = 
{
	postfix = "_VAR",
	noDefault = true,
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

	-- Material
	"vecAlbedo",
	"vecEmission",
	"vecAttributes",
	"texColor",
	"texAttributes",
	"texEmission",
	"texNormalmap",
	
	-- Light
	"iLightCount",
	
	"fGamma",
	"vecTime",
	
	"vecFogColor",
	
	"iDebugMode",
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

enum.HULLTYPE = 
{
	prefix = "HULL_",
	"Sphere",
	"Box",
	"Cylinder",
	"Capsule",
	"Polygon",
	"Convex",
}