#include <acknext.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <cef_app.h>
#include <cef_client.h>
#include <wrapper/cef_helpers.h>
#include <cef_task.h>
#pragma GCC diagnostic pop

#include <functional>

struct RenderHandler : public CefRenderHandler
{
	VIEW * view;
	BITMAP * target;
public:
    RenderHandler(VIEW * view) :
		view(view),
	    target(bmap_create(GL_TEXTURE_2D, GL_RGBA8))
    {
	}

	~RenderHandler()
	{
		bmap_remove(target);
	}

    // CefRenderHandler interface
public:
    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
    {
		SIZE size;
		view_to_bounds(view, nullptr, &size);
        rect = CefRect(0, 0, size.width, size.height);
		// engine_log("size: (%d×%d)", rect.width, rect.height);
        return true;
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
    {
		glBindTexture(GL_TEXTURE_2D, target->object);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			width, height, 0,
			GL_BGRA, GL_UNSIGNED_BYTE,
			buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		// engine_log("Update texture (%d×%d)", width, height);

        // Ogre::HardwarePixelBufferSharedPtr texBuf = m_renderTexture->getBuffer();
        // texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        // memcpy(texBuf->getCurrentLock().data, buffer, width*height*4);
        // texBuf->unlock();
    }

    // CefBase interface
public:
    IMPLEMENT_REFCOUNTING(RenderHandler);

};


struct RequestHandler : public CefRequestHandler
{
public:
    RequestHandler()
    {
	}

	~RequestHandler()
	{

	}

    // CefRequestHandler interface
public:

	virtual ReturnValue OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefRequestCallback> callback) override
	{
		auto str = request->GetURL().ToString();
		// TODO: Implement resource access check here!
		engine_log("got request: `%s`", str.c_str());
		return RV_CONTINUE;
    }

    // CefBase interface
public:
    IMPLEMENT_REFCOUNTING(RequestHandler);

};

class ResourceHandler : public CefResourceHandler
{
private:
	ACKFILE * handle;
public:
	ResourceHandler() : handle(nullptr)
	{

	}
	~ResourceHandler() {
		file_close(handle);
	}
public:

	///
    // Begin processing the request. To handle the request return true and call
    // CefCallback::Continue() once the response header information is available
    // (CefCallback::Continue() can also be called from inside this method if
    // header information is available immediately). To cancel the request return
    // false.
    ///
    /*--cef()--*/
    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                                CefRefPtr<CefCallback> callback)override
	{
		auto str = request->GetURL().ToString();
		auto method = request->GetMethod().ToString();
		auto fileName = str.substr(6);
		this->handle = file_open_read(fileName.c_str());
		if(this->handle) {
			// engine_log("ackrequest(%s): opened '%s'", method.c_str(), fileName.c_str());
			callback->Continue();
			return true;
		} else {
			engine_log("ackrequest(%s): failed to open '%s'", method.c_str(), fileName.c_str());
			return false;
		}
	}

    ///
    // Retrieve response header information. If the response length is not known
    // set |response_length| to -1 and ReadResponse() will be called until it
    // returns false. If the response length is known set |response_length|
    // to a positive value and ReadResponse() will be called until it returns
    // false or the specified number of bytes have been read. Use the |response|
    // object to set the mime type, http status code and other optional header
    // values. To redirect the request to a new URL set |redirectUrl| to the new
    // URL. If an error occured while setting up the request you can call
    // SetError() on |response| to indicate the error condition.
    ///
    /*--cef()--*/
    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                                    int64& response_length,
                                    CefString& redirectUrl)override
	{
		response->SetStatus(200);
		// response->SetMimeType("text/html");
		response_length = file_size(this->handle);
	}

    ///
    // Read response data. If data is available immediately copy up to
    // |bytes_to_read| bytes into |data_out|, set |bytes_read| to the number of
    // bytes copied, and return true. To read the data at a later time set
    // |bytes_read| to 0, return true and call CefCallback::Continue() when the
    // data is available. To indicate response completion return false.
    ///
    /*--cef()--*/
    virtual bool ReadResponse(void* data_out,
                              int bytes_to_read,
                              int& bytes_read,
                              CefRefPtr<CefCallback> callback) override
	{
		if(file_eof(this->handle))
			return false;
		bytes_read = file_read(this->handle, data_out, bytes_to_read);
		return true;
	}

    ///
    // Request processing has been canceled.
    ///
    /*--cef()--*/
    virtual void Cancel() override
	{
		engine_log("Cancel the shit out of this!");
	}
public:
	IMPLEMENT_REFCOUNTING(ResourceHandler);
};

class SchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
	SchemeHandlerFactory()
	{

	}

	~SchemeHandlerFactory()
	{

	}
public:
	virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request)
	{
		return new ResourceHandler();
	}
public:
	IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
};

// for manual render handler
class BrowserClient : public CefClient
{
public:
    BrowserClient(RenderHandler *renderHandler) :
	    m_renderHandler(renderHandler),
		m_requestHandler(new RequestHandler)
    {

	}

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() {
        return m_renderHandler;
    }

	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() {
		return m_requestHandler;
	}

    CefRefPtr<CefRenderHandler> m_renderHandler;
	CefRefPtr<CefRequestHandler> m_requestHandler;

    IMPLEMENT_REFCOUNTING(BrowserClient);
};

class AckApp : public CefApp
{
public:
	AckApp() = default;
	~AckApp() = default;
public:
	virtual void OnRegisterCustomSchemes(
        CefRawPtr<CefSchemeRegistrar> registrar) override
	{
		registrar->AddCustomScheme(
			"ack",
		    false,         // bool is_standard,
            true,          // bool is_local,
            false,         // bool is_display_isolated,
            false,         // bool is_secure,
            false,         // bool is_cors_enabled,
            true);        // bool is_csp_bypassing) = 0;
	}
public:
	IMPLEMENT_REFCOUNTING(AckApp);
};

static CefMainArgs args;
static bool cefHasMainloop = false;

ACKFUN void ackcef_init(int argc, char ** argv)
{
	::args = CefMainArgs(argc, argv);

	int result = CefExecuteProcess(::args, nullptr, nullptr);
    // checkout CefApp, derive it and set it as second parameter, for more control on
    // command args and resources.
    if (result >= 0) // child proccess has endend, so exit.
    {
        exit(result);
    }
	assert(result == -1);

	CefSettings settings;

    // checkout detailed settings options http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html
    // nearly all the settings can be set via args too.
    // settings.multi_threaded_message_loop = true; // not supported, except windows
    // CefString(&settings.browser_subprocess_path).FromASCII("sub_proccess path, by default uses and starts this executeable as child");
    // CefString(&settings.cache_path).FromASCII("");
    CefString(&settings.log_file).FromASCII("/home/felix/projects/acknext/scripts/cef.log");
    settings.log_severity = LOGSEVERITY_WARNING;
	settings.background_color = CefColorSetARGB(0xFF, 0xFF, 0xFF, 0x00);
	settings.no_sandbox = true;
	settings.windowless_rendering_enabled = true;
	settings.remote_debugging_port = 8090;
	settings.single_process = true;
	// settings.file_access_from_file_urls_allowed = true;
	// settings.universal_access_from_file_urls_allowed = true;

    // CefString(&settings.resources_dir_path).FromASCII("");
    // CefString(&settings.locales_dir_path).FromASCII("");

    bool success = CefInitialize(::args, settings, new AckApp(), nullptr);
    // CefInitialize creates a sub-proccess and executes the same executeable, as calling CefInitialize, if not set different in settings.browser_subprocess_path
    // if you create an extra program just for the childproccess you only have to call CefExecuteProcess(...) in it.
    if (!success)
    {
        // handle error
		engine_log("Failed to initialize CEF!");
        exit(-1);
    }

	CefRegisterSchemeHandlerFactory("ack", CefString(), new SchemeHandlerFactory());
}

static void cef_mainloop(void*)
{
	while(true)
	{
		CefDoMessageLoopWork();
		task_yield();
	}
}

struct AckCefView
{
	RenderHandler * renderHandler;
	CefRefPtr<CefBrowser> browser;
    CefRefPtr<BrowserClient> browserClient;
	SIZE previousSize;
	VIEW * view;
	int mouseState;
public:
	AckCefView() : mouseState(0)
	{

	}

	void init()
	{
		CefWindowInfo window_info;
		CefBrowserSettings browserSettings;

        // browserSettings.windowless_frame_rate = 60; // 30 is default

		// may use a gtk widget here...
        window_info.SetAsWindowless(0);

        browserClient = new BrowserClient(renderHandler);

        browser = CefBrowserHost::CreateBrowserSync(
			window_info,
			browserClient.get(),
			"about:blank",
			browserSettings,
			nullptr);

        // inject user-input by calling - non-trivial for non-windows - checkout the cefclient source and the platform specific cpp, like cefclient_osr_widget_gtk.cpp for linux
        // browser->GetHost()->SendKeyEvent(...);
        // browser->GetHost()->SendMouseWheelEvent(...);
	}

	~AckCefView()
	{
		browser = nullptr;
		browserClient = nullptr;
		delete renderHandler;
		view_remove(this->view);
	}
};

static BUFFER * vbuffer;

static SHADER * shader;

static VERTEX mkvtx(int x, int y)
{
	VERTEX vtx;
	vtx.position = (VECTOR) { 2*float(x) - 1, 2*float(y) - 1, 0 };
	vtx.texcoord0 = (UVCOORD) { float(x), 1-float(y) };
	return vtx;
}

static void cef_init()
{
	VERTEX vertices[4] =
	{
	    mkvtx(0, 0),
	    mkvtx(0, 1),
	    mkvtx(1, 0),
	    mkvtx(1, 1),
	};

	vbuffer = buffer_create(VERTEXBUFFER);
	buffer_set(vbuffer, sizeof(VERTEX) * 4, vertices);

	shader = shader_create();

	assert(shader_addSource(shader, GL_VERTEX_SHADER,
R"glsl(#version 330
layout(location = 0) in vec3 vPosition;
layout(location = 4) in vec2 vUV;

out vec2 uv;

void main()
{
	gl_Position = vec4(vPosition.xy, 0, 1);
	uv = vUV;
}
)glsl"));

	assert(shader_addSource(shader, GL_FRAGMENT_SHADER,
R"glsl(#version 330
layout(location = 0) out vec4 color;

in vec2 uv;

uniform sampler2D texBrowser;

void main()
{
	color = texture(texBrowser, uv);
}
)glsl"));
	assert(shader_link(shader));

	task_defer(cef_mainloop, nullptr);

}

static void cef_renderframe(void * ctx)
{
	AckCefView * browser = (AckCefView*)ctx;

	SIZE size;
	POINT pt;
	view_to_bounds(browser->renderHandler->view, &pt, &size);

	if(size.width != browser->previousSize.width || size.height != browser->previousSize.height)
	{
		browser->browser->GetHost()->WasResized();
		browser->previousSize = size;
	}

	CefMouseEvent ev;
	ev.x = mouse_pos.x - pt.x;
	ev.y = mouse_pos.y - pt.y;

	browser->browser->GetHost()->SendMouseMoveEvent(ev, false);

	if(mouse_left != browser->mouseState)
	{
		browser->browser->GetHost()->SendMouseClickEvent(
			ev,
			MBT_LEFT,
			!mouse_left,
			1);
		browser->mouseState = mouse_left;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	opengl_setShader(shader);
	opengl_setIndexBuffer(nullptr);
	opengl_setVertexBuffer(vbuffer);

	shader_setUniforms(shader, browser->renderHandler->view, true);

	opengl_draw(GL_TRIANGLE_STRIP, 0, 4);

	glDisable(GL_BLEND);
}

ACKFUN VIEW * ackcef_createView()
{
	if(!cefHasMainloop) {
		cef_init();
		cefHasMainloop = true;
	}

	AckCefView * view = new AckCefView();

	view->view = view_create(cef_renderframe, view);

	view_to_bounds(view->view, nullptr, &view->previousSize);

	view->renderHandler = new RenderHandler(view->view);

	view->init();

	obj_setvar(view->view, "texBrowser", GL_SAMPLER_2D, view->renderHandler->target);

	return view->view;
}

ACKFUN bool ackcef_ready(VIEW * _view)
{
	if(!_view) return false;
	auto * view = (AckCefView *)_view->context;

	return !view->browser->IsLoading();
}


ACKFUN void ackcef_navigate(VIEW * _view, char const * url)
{
	if(!_view) return;
	if(!url) return;
	auto * view = (AckCefView *)_view->context;

	// this should be boa constrictored!
	view->browser->GetMainFrame()->LoadURL(url);
}

ACKFUN void ackcef_exec(VIEW * _view, char const * js)
{
	if(!_view) return;
	if(!js) return;
	auto * view = (AckCefView *)_view->context;

	auto frame = view->browser->GetMainFrame();
	frame->ExecuteJavaScript(
		js,
	    frame->GetURL(),
		0);
}

ACKFUN void ackcef_refresh(VIEW * _view)
{
	if(!_view) return;
	auto * view = (AckCefView *)_view->context;
	view->browser->Reload();
}
