/*=======================================================================
              WebP load / save plugin for the GIMP
                 Copyright 2012 - Nathan Osman

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
=======================================================================*/

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <GL/gl.h>

const char LOAD_PROCEDURE[] = "file-atx-load";
const char SAVE_PROCEDURE[] = "file-atx-save";
const char BINARY_NAME[]    = "file-atx";

// Predeclare our entrypoints
void query();
void run(const gchar *, gint, const GimpParam *, gint *, GimpParam **);

// Declare our plugin entry points
GimpPlugInInfo PLUG_IN_INFO = {
    NULL,
    NULL,
    query,
    run
};

MAIN()

static int read_atx(const gchar * filename);

static int write_atx(const gchar * filename, gint drawable_id);

// The query function
void query()
{
    // Load arguments
    static const GimpParamDef load_arguments[] =
    {
        { GIMP_PDB_INT32,  "run-mode",     "Interactive, non-interactive" },
        { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
        { GIMP_PDB_STRING, "raw-filename", "The name entered" }
    };

    // Load return values
    static const GimpParamDef load_return_values[] =
    {
        { GIMP_PDB_IMAGE, "image", "Output image" }
    };

    // Save arguments
    static const GimpParamDef save_arguments[] =
    {
        { GIMP_PDB_INT32,    "run-mode",     "Interactive, non-interactive" },
        { GIMP_PDB_IMAGE,    "image",        "Input image" },
        { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save" },
        { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
        { GIMP_PDB_STRING,   "raw-filename", "The name entered" },
    };

    // Install the load procedure
    gimp_install_procedure(LOAD_PROCEDURE,
                           "Loads images in the Acknext texture file format",
                           "Loads images in the Acknext texture file format",
                           "Felix Queißner",
                           "Copyright Felix Queißner",
                           "2017",
                           "Acknext texture",
                           NULL,
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(load_arguments),
                           G_N_ELEMENTS(load_return_values),
                           load_arguments,
                           load_return_values);

    // Install the save procedure
    gimp_install_procedure(SAVE_PROCEDURE,
                           "Saves files in the Acknext texture file format",
                           "Saves files in the Acknext texture file format",
	                       "Felix Queißner",
                           "Copyright Felix Queißner",
                           "2017",
                           "Acknext texture",
                           "RGB*",
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(save_arguments),
                           0,
                           save_arguments,
                           NULL);

    // Register the load handlers
    gimp_register_file_handler_mime(LOAD_PROCEDURE, "image/x-acknext-texture");
    gimp_register_load_handler(LOAD_PROCEDURE, "atx", "");

    // Now register the save handlers
    gimp_register_file_handler_mime(SAVE_PROCEDURE, "image/x-acknext-texture");
    gimp_register_save_handler(SAVE_PROCEDURE, "atx", "");
}

// The run function
void run(const gchar * name,
         gint nparams,
         const GimpParam * param,
         gint * nreturn_vals,
         GimpParam ** return_vals)
{
    // Create the return value.
    static GimpParam return_values[2];
    *nreturn_vals = 1;
    *return_vals  = return_values;

    // Set the return value to success by default
    return_values[0].type          = GIMP_PDB_STATUS;
    return_values[0].data.d_status = GIMP_PDB_SUCCESS;

    // Check to see if this is the load procedure
    if(!strcmp(name, LOAD_PROCEDURE))
    {
        int new_image_id;

        // Check to make sure all parameters were supplied
        if(nparams != 3)
        {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }

        // Now read the image
        new_image_id = read_atx(param[1].data.d_string);

        // Check for an error
        if(new_image_id == -1)
        {
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
            return;
        }

        // Fill in the second return value
        *nreturn_vals = 2;

        return_values[1].type         = GIMP_PDB_IMAGE;
        return_values[1].data.d_image = new_image_id;
    }
    else if(!strcmp(name, SAVE_PROCEDURE))
    {
        gint32 image_id, drawable_id;
        int status = 1;
        GimpExportReturn export_ret;

        // Check to make sure all of the parameters were supplied
        if(nparams < 4)
        {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }

        image_id    = param[1].data.d_int32;
        drawable_id = param[2].data.d_int32;

        // Try to export the image
        gimp_ui_init(BINARY_NAME, FALSE);
        export_ret = gimp_export_image(&image_id,
                                       &drawable_id,
                                       "ATX",
                                       GIMP_EXPORT_CAN_HANDLE_RGB | GIMP_EXPORT_CAN_HANDLE_ALPHA);

        switch(export_ret)
        {
            case GIMP_EXPORT_EXPORT:
            case GIMP_EXPORT_IGNORE:

                // TODO: Now get the settings
				/*
                if(!export_dialog(&quality))
                {
                    return_values[0].data.d_status = GIMP_PDB_CANCEL;
                    return;
                }
				*/

                status = write_atx(param[3].data.d_string, drawable_id);
                gimp_image_delete(image_id);

                break;
            case GIMP_EXPORT_CANCEL:
                return_values[0].data.d_status = GIMP_PDB_CANCEL;
                return;
        }

        if(!status)
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
    }
    else
        return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
}

static uint8_t atxHeader[32] =
{
    // ID
	'A', 'C', 'K', 'N', 'E', 'X', 'T', 0,

    // Magic
    0x67, 0xE3, 0x41, 0xCC,

    // Type
    0x08, 0x00, 0x00, 0x00,

    // GUID
    0xb4, 0x0f, 0x64, 0x26,
    0x2d, 0x6c, 0x44, 0xb3,
    0xbd, 0x3d, 0x62, 0x9f,
    0xaa, 0x5a, 0xef, 0xe7
};

struct data_t
{
	uint32_t target;
	uint32_t format;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t pixelFormat;
	uint32_t pixelType;
	uint32_t bufferSize;
} __attribute__((packed));

static int limit(int i)
{
	if(i < 0) return 0;
	if(i >= 255) return 255;
	return i;
}

static int readChan(struct data_t const * data, void const * src, int x, int y, int z, int chan)
{
	int px = x;
	int py = data->height - y - 1;
	int pz = z;
	int pc = 0;
	switch(data->pixelFormat)
	{
		case GL_R:
			pc = 1;
			break;
		case GL_RG:
			pc = 2;
			break;
		case GL_RGB:
		case GL_BGR:
			pc = 3;
			break;
		case GL_RGBA:
		case GL_BGRA:
			pc = 4;
			break;
	}

	int index = pc * (data->width * data->height * pz + data->width * py + px) + chan;
	switch(data->pixelType)
	{
		case GL_UNSIGNED_BYTE: {
			uint8_t const * pixels = src;
			return pixels[index];
		}
		case GL_UNSIGNED_SHORT: {
			uint16_t const * pixels = src;
			return pixels[index] >> 8; // simple clipping, lossy
		}
		case GL_UNSIGNED_INT: {
			uint32_t const * pixels = src;
			return pixels[index] >> 24; // simple clipping, lossy
		}
		case GL_FLOAT: {
			float const * pixels = src;
			return limit(255.0 * pixels[index]);
		}
		case GL_DOUBLE: {
			double const * pixels = src;
			return limit(255.0 * pixels[index]);
		}
		default:
			abort();
	}
}

static int read_atx(const gchar * filename)
{
    // Try to open the file
    FILE * file = fopen(filename, "rb");

    if(file == NULL)
		return -1;

	uint8_t header[32];
	if(fread(header, 1, 32, file) != 32) {
		g_warning("Not an acknext texture file");
		 fclose(file);
		 return -1;
	}
	if(memcmp(header, atxHeader, 32) != 0) {
		fclose(file);
		g_warning("Not an acknext texture file");
	    return -1;
	}

	struct data_t data;
	if(fread(&data, 1, sizeof(data), file) != sizeof(data)) {
		fclose(file);
		g_error("Failed to read data...");
	    return -1;
	}

	uint8_t * pixeldata = malloc(data.bufferSize);

	if(fread(pixeldata, 1, data.bufferSize, file) != data.bufferSize) {
		fclose(file);
		g_error("Failed to read data...");
	    return -1;
	}

	fclose(file);

	int width = data.width;
	int height = data.height;

	// Specify supported pixel types
	switch(data.pixelType)
	{
		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_SHORT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
		case GL_DOUBLE:
			break;
		default:
			g_error("Unsupported pixel type: 0x%04X", data.pixelType);
			return -1;
	}

	uint8_t * imagedata = NULL;
	GimpImageType imageType;
	if(data.pixelFormat == GL_RGB || data.pixelFormat == GL_BGR)
	{
		imageType = GIMP_RGB_IMAGE;
		imagedata = malloc(3 * width * height);
	}
	else if(data.pixelFormat == GL_RGBA || data.pixelFormat == GL_BGRA)
	{
		imageType = GIMP_RGBA_IMAGE;
		imagedata = malloc(4 * width * height);
	}
	else
	{
		free(pixeldata);
		g_error("Unsupported pixel format: 0x%04X", data.pixelFormat);
		return -1;
	}

	assert(imagedata != NULL);

	// Now create the new RGBA image.
    gint32 new_image_id = gimp_image_new(width, height, GIMP_RGB);

	for(uint z = 0; z < data.depth; z++)
	{
		// Convert layer data
		if(data.pixelFormat == GL_RGB || data.pixelFormat == GL_BGR)
		{
			int ir = 0;
			int ig = 1;
			int ib = 2;
			if(data.pixelFormat == GL_BGRA) {
				ir = 2;
				ib = 0;
			}

			for(int y = 0; y < height; y++)
			{
				for(int x = 0; x < width; x++)
				{
					int r = readChan(&data, pixeldata, x, y, z, ir);
					int g = readChan(&data, pixeldata, x, y, z, ig);
					int b = readChan(&data, pixeldata, x, y, z, ib);

					imagedata[3 * (width * y + x) + 0] = limit(r);
					imagedata[3 * (width * y + x) + 1] = limit(g);
					imagedata[3 * (width * y + x) + 2] = limit(b);
				}
			}
		}
		else if(data.pixelFormat == GL_RGBA || data.pixelFormat == GL_BGRA)
		{
			int ir = 0;
			int ig = 1;
			int ib = 2;
			int ia = 3;
			if(data.pixelFormat == GL_BGRA) {
				ir = 2;
				ib = 0;
			}

			for(int y = 0; y < height; y++)
			{
				for(int x = 0; x < width; x++)
				{
					int r = readChan(&data, pixeldata, x, y, z, ir);
					int g = readChan(&data, pixeldata, x, y, z, ig);
					int b = readChan(&data, pixeldata, x, y, z, ib);
					int a = readChan(&data, pixeldata, x, y, z, ia);

					imagedata[4 * (width * y + x) + 0] = limit(r);
					imagedata[4 * (width * y + x) + 1] = limit(g);
					imagedata[4 * (width * y + x) + 2] = limit(b);
					imagedata[4 * (width * y + x) + 3] = limit(a);
				}
			}
		}

		assert(imagedata != NULL);

		char namebuf[128];
		if(data.target == GL_TEXTURE_2D)
		{
			strcpy(namebuf, "Background");
		}
		else
		{
			sprintf(namebuf, "Layer (Z=%d)", z);
		}

		// Create the new layer
		gint32 new_layer_id = gimp_layer_new(new_image_id,
									  namebuf,
									  data.width, data.height,
									  imageType,
									  100,
									  GIMP_NORMAL_MODE);

		// Get the drawable for the layer
		GimpDrawable * drawable = gimp_drawable_get(new_layer_id);

		// Get a pixel region from the layer
		GimpPixelRgn rgn;
		gimp_pixel_rgn_init(&rgn,
							drawable,
							0, 0,
							width, height,
							TRUE, FALSE);

		// Now FINALLY set the pixel data
		gimp_pixel_rgn_set_rect(&rgn,
								imagedata,
								0, 0,
								width, height);

		// We're done with the drawable
		gimp_drawable_flush(drawable);
		gimp_drawable_detach(drawable);

		// Add the layer to the image
		gimp_image_add_layer(new_image_id, new_layer_id, 0);
	}

	free(pixeldata);
	free(imagedata);

    // Set the filename
    gimp_image_set_filename(new_image_id, filename);

    return new_image_id;
}



static int write_atx(const gchar * filename, gint drawable_id)
{
	if(!gimp_drawable_is_rgb(drawable_id)) {
		g_error("Cannot export non-RGB image!");
		return 0;
	}

    // Get the drawable
    GimpDrawable * drawable = gimp_drawable_get(drawable_id);

    // Get the BPP
	gint bpp = gimp_drawable_bpp(drawable_id);

	if(bpp == 3)
		g_assert(gimp_drawable_has_alpha(drawable_id) == 0);
	else if(bpp == 4)
		g_assert(gimp_drawable_has_alpha(drawable_id) == 1);
	else
		g_assert(FALSE);

	int width = drawable->width;
	int height = drawable->height;

    // Get a pixel region from the layer
	GimpPixelRgn rgn;
    gimp_pixel_rgn_init(&rgn,
                        drawable,
                        0, 0,
                        drawable->width,
                        drawable->height,
                        FALSE, FALSE);

    // Determine the size of the array of image data to get
    // and allocate it.
	size_t data_size = drawable->width * drawable->height * bpp;
    uint8_t * image_data = malloc(data_size);

    // Get the image data
    gimp_pixel_rgn_get_rect(&rgn,
                            (guchar *)image_data,
                            0, 0,
                            drawable->width,
                            drawable->height);

    // Detach the drawable
    gimp_drawable_detach(drawable);

    // Open the file
    FILE * file = fopen(filename, "wb");
    if(!file)
    {
		g_error("Failed to open the image!");
        free(image_data);
        return 0;
    }

	fwrite(atxHeader, 1, 32, file);

	struct data_t data;
	data.target = GL_TEXTURE_2D;
	data.format = (bpp == 3) ? GL_RGB8 : GL_RGBA8;
	data.width = width;
	data.height = height;
	data.depth = 1;
	data.pixelFormat = (bpp == 3) ? GL_RGB : GL_RGBA;
	data.pixelType = GL_UNSIGNED_BYTE;
	data.bufferSize = data_size;

	fwrite(&data, sizeof(data), 1, file);

	// write image upside down because OpenGL coordinates
	const size_t stride = bpp * data.width;
	for(int y = (data.height - 1); y >= 0; y--)
	{
		fwrite(&image_data[stride * y], 1, stride, file);
	}

	fclose(file);

	free(image_data);

    return 1;
}
