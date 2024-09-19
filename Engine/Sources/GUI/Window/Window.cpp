#include "./Window.h"

#include <stdexcept>
#include <utility>

namespace egg::GUI
{
    Window::Window(const std::string_view Title, const DimensionsType Width, const DimensionsType Height, const ClientAPI GraphicsAPI)
    {
        glfwWindowHint(GLFW_CLIENT_API, std::to_underlying(GraphicsAPI));

        Data = glfwCreateWindow(Width, Height, Title.data(), nullptr, nullptr);

        if (!Data) throw std::runtime_error("Failed to initialize window");
    }

    Window::~Window()
    {
        glfwDestroyWindow(Data);
    }

    void Window::SetSizeLimits(const DimensionsType MinWidth, const DimensionsType MinHeight,
                               const DimensionsType MaxWidth, const DimensionsType MaxHeight) const
    {
        glfwSetWindowSizeLimits(Data, MinWidth, MinHeight, MaxWidth, MaxHeight);
    }

    void Window::SetSizeMinimum(const DimensionsType Width, const DimensionsType Height) const
    {
        glfwSetWindowSizeLimits(Data, Width, Height, GLFW_DONT_CARE, GLFW_DONT_CARE);
    }

    void Window::SetSizeMaximum(const DimensionsType Width, const DimensionsType Height) const
    {
        glfwSetWindowSizeLimits(Data, GLFW_DONT_CARE, GLFW_DONT_CARE, Width, Height);
    }

    void Window::SetTitle(const std::string_view Title) const
    {
        glfwSetWindowTitle(Data, Title.data());
    }

    void Window::SetIcon(const GLFWimage& Image) const
    {
        glfwSetWindowIcon(Data, 1, &Image);
    }

    void Window::SetIcon(const std::vector<GLFWimage>& Images) const
    {
        glfwSetWindowIcon(Data, static_cast<DimensionsType>(Images.size()), Images.data());
    }

    void Window::SetPosition(const DimensionsType X, const DimensionsType Y) const
    {
        glfwSetWindowPos(Data, X, Y);
    }

    void Window::SetAspectRatio(const DimensionsType Numerator, const DimensionsType Denominator) const
    {
        glfwSetWindowAspectRatio(Data, Numerator, Denominator);
    }

    void Window::SetOpacity(const ScalarType Opacity) const
    {
        glfwSetWindowOpacity(Data, Opacity);
    }

    void Window::SetMonitor(GLFWmonitor& Monitor,
                            const DimensionsType XPosition, const DimensionsType YPosition,
                            const DimensionsType Width, const DimensionsType Height,
                            const DimensionsType RefreshRate) const
    {
        glfwSetWindowMonitor(Data, &Monitor, XPosition, YPosition, Width, Height, RefreshRate);
    }

    void Window::SetAttribute(const Attribute Attribute, const bool Value) const
    {
        glfwSetWindowAttrib(Data, std::to_underlying(Attribute), Value);
    }

    void Window::SetUserPointer(void* Pointer) const
    {
        glfwSetWindowUserPointer(Data, Pointer);
    }

    void Window::SetPositionCallback(const GLFWwindowposfun Callback) const
    {
        glfwSetWindowPosCallback(Data, Callback);
    }

    void Window::SetCloseCallback(const GLFWwindowclosefun Callback) const
    {
        glfwSetWindowCloseCallback(Data, Callback);
    }

    void Window::SetRefreshCallback(const GLFWwindowrefreshfun Callback) const
    {
        glfwSetWindowRefreshCallback(Data, Callback);
    }

    void Window::SetFocusCallback(const GLFWwindowfocusfun Callback) const
    {
        glfwSetWindowFocusCallback(Data, Callback);
    }

    void Window::SetIconifyCallback(const GLFWwindowiconifyfun Callback) const
    {
        glfwSetWindowIconifyCallback(Data, Callback);
    }

    void Window::SetMaximizeCallback(const GLFWwindowmaximizefun Callback) const
    {
        glfwSetWindowMaximizeCallback(Data, Callback);
    }

    void Window::SetContentScaleCallback(const GLFWwindowcontentscalefun Callback) const
    {
        glfwSetWindowContentScaleCallback(Data, Callback);
    }

    void* Window::GetUserPointer() const
    {
        return glfwGetWindowUserPointer(Data);
    }

    Window::CursorMode Window::GetInputModeCursor() const
    {
        return static_cast<CursorMode>(glfwGetInputMode(Data, GLFW_CURSOR));
    }

    bool Window::GetInputModeStickyKeys() const
    {
        return glfwGetInputMode(Data, GLFW_STICKY_KEYS);
    }

    bool Window::GetInputModeStickyMouseButtons() const
    {
        return glfwGetInputMode(Data, GLFW_STICKY_MOUSE_BUTTONS);
    }

    bool Window::GetInputModeLockKeyMods() const
    {
        return glfwGetInputMode(Data, GLFW_LOCK_KEY_MODS);
    }

    bool Window::GetInputModeRawMouseMotion() const
    {
        return glfwGetInputMode(Data, GLFW_RAW_MOUSE_MOTION);
    }

    bool Window::GetKey(const Key Value) const
    {
        return glfwGetKey(Data, std::to_underlying(Value));
    }

    bool Window::GetMouseButton(const MouseButton Value) const
    {
        return glfwGetMouseButton(Data, std::to_underlying(Value));
    }

    std::pair<Window::CoordinatesType, Window::CoordinatesType> Window::GetCursorPosition() const
    {
        CoordinatesType XPosition, YPosition;
        glfwGetCursorPos(Data, &XPosition, &YPosition);
        return { XPosition, YPosition };
    }

    void Window::SetSizeCallback(const GLFWwindowsizefun Callback) const
    {
        glfwSetWindowSizeCallback(Data, Callback);
    }

    void Window::SetFrameBufferSizeCallback(const GLFWframebuffersizefun Callback) const
    {
        glfwSetFramebufferSizeCallback(Data, Callback);
    }

    void Window::SetKeyCallback(const GLFWkeyfun Callback) const
    {
        glfwSetKeyCallback(Data, Callback);
    }

    GLFWcharfun Window::SetCharCallback(const GLFWcharfun Callback) const
    {
        return glfwSetCharCallback(Data, Callback);
    }

    GLFWmousebuttonfun Window::SetMouseButtonCallback(const GLFWmousebuttonfun Callback) const
    {
        return glfwSetMouseButtonCallback(Data, Callback);
    }

    GLFWcursorposfun Window::SetCursorPositionCallback(const GLFWcursorposfun Callback) const
    {
        return glfwSetCursorPosCallback(Data, Callback);
    }

    GLFWcursorenterfun Window::SetCursorEnterCallback(const GLFWcursorenterfun Callback) const
    {
        return glfwSetCursorEnterCallback(Data, Callback);
    }

    GLFWscrollfun Window::SetScrollCallback(const GLFWscrollfun Callback) const
    {
        return glfwSetScrollCallback(Data, Callback);
    }

    GLFWdropfun Window::SetDropCallback(const GLFWdropfun Callback) const
    {
        return glfwSetDropCallback(Data, Callback);
    }

    void Window::MakeContextCurrent() const
    {
        glfwMakeContextCurrent(Data);
    }

    void Window::SwapBuffers() const
    {
        glfwSwapBuffers(Data);
    }

    void Window::Restore() const
    {
        glfwRestoreWindow(Data);
    }

    void Window::Maximize() const
    {
        glfwMaximizeWindow(Data);
    }

    void Window::Show() const
    {
        glfwShowWindow(Data);
    }

    void Window::Hide() const
    {
        glfwHideWindow(Data);
    }

    void Window::Focus() const
    {
        glfwFocusWindow(Data);
    }

    void Window::RequestAttention() const
    {
        glfwRequestWindowAttention(Data);
    }

    void Window::Iconify() const
    {
        glfwIconifyWindow(Data);
    }

    void Window::SetShouldClose(const bool ShouldClose) const
    {
        glfwSetWindowShouldClose(Data, ShouldClose);
    }

    void Window::SetInputMode(const InputMode Mode, const CursorMode Value) const
    {
        glfwSetInputMode(Data, std::to_underlying(Mode), std::to_underlying(Value));
    }

    void Window::SetCursorPosition(const CoordinatesType XPosition, const CoordinatesType YPosition) const
    {
        glfwSetCursorPos(Data, XPosition, YPosition);
    }

    void Window::SetCursor(GLFWcursor* Cursor) const
    {
        glfwSetCursor(Data, Cursor);
    }

    std::string_view Window::GetTitle() const
    {
        return glfwGetWindowTitle(Data);
    }

    std::pair<Window::DimensionsType, Window::DimensionsType> Window::GetPosition() const
    {
        DimensionsType XPosition, YPosition;
        glfwGetWindowPos(Data, &XPosition, &YPosition);
        return { XPosition, YPosition };
    }

    std::pair<Window::DimensionsType, Window::DimensionsType> Window::GetSize() const
    {
        DimensionsType Width, Height;
        glfwGetWindowSize(Data, &Width, &Height);
        return { Width, Height };
    }

    std::pair<Window::DimensionsType, Window::DimensionsType> Window::GetFrameBufferSize() const
    {
        DimensionsType Width, Height;
        glfwGetFramebufferSize(Data, &Width, &Height);
        return { Width, Height };
    }

    std::array<Window::DimensionsType, 4> Window::GetFrameSize() const
    {
        DimensionsType Left, Top, Right, Bottom;
        glfwGetWindowFrameSize(Data, &Left, &Top, &Right, &Bottom);
        return { Left, Top, Right, Bottom };
    }

    std::pair<Window::ScalarType, Window::ScalarType> Window::GetContentScale() const
    {
        ScalarType XScale, YScale;
        glfwGetWindowContentScale(Data, &XScale, &YScale);
        return { XScale, YScale };
    }

    Window::ScalarType Window::GetOpacity() const
    {
        return glfwGetWindowOpacity(Data);
    }

    GLFWmonitor* Window::GetMonitor() const
    {
        return glfwGetWindowMonitor(Data);
    }

    bool Window::GetAttributeValue(const Attribute Value) const
    {
        return glfwGetWindowAttrib(Data, std::to_underlying(Value));
    }

    bool Window::GetShouldClose() const
    {
        return glfwWindowShouldClose(Data);
    }
}
