#ifndef ENGINE_SOURCES_GUI_WINDOW_FILE_WINDOW_H
#define ENGINE_SOURCES_GUI_WINDOW_FILE_WINDOW_H

#include "../../Constants/Window.h"

#include <array>
#include <string_view>
#include <vector>

namespace egg::GUI
{
    class Window
    {
    public:
        enum class ClientAPI
        {
            None = GLFW_NO_API,
            OpenGL = GLFW_OPENGL_API,
            OpenGLES = GLFW_OPENGL_ES_API
        };

        enum class Attribute
        {
            Decorated = GLFW_DECORATED,
            Resizable = GLFW_RESIZABLE,
            Floating = GLFW_FLOATING,
            AutoIconify = GLFW_AUTO_ICONIFY,
            FocusOnShow = GLFW_FOCUS_ON_SHOW
        };

        enum class InputMode
        {
            Cursor = GLFW_CURSOR,
            StickyKeys = GLFW_STICKY_KEYS,
            StickyMouseButtons = GLFW_STICKY_MOUSE_BUTTONS,
            LockKeyMods = GLFW_LOCK_KEY_MODS,
            RawMouseMotion = GLFW_RAW_MOUSE_MOTION
        };

        enum class CursorMode
        {
            Normal = GLFW_CURSOR_NORMAL,
            Hidden = GLFW_CURSOR_HIDDEN,
            Disabled = GLFW_CURSOR_DISABLED
        };

        enum class Key
        {
            Space = GLFW_KEY_SPACE,
            Apostrophe = GLFW_KEY_APOSTROPHE,
            Comma = GLFW_KEY_COMMA,
            Minus = GLFW_KEY_MINUS,
            Period = GLFW_KEY_PERIOD,
            Slash = GLFW_KEY_SLASH,
            Zero = GLFW_KEY_0,
            One = GLFW_KEY_1,
            Two = GLFW_KEY_2,
            Three = GLFW_KEY_3,
            Four = GLFW_KEY_4,
            Five = GLFW_KEY_5,
            Six = GLFW_KEY_6,
            Seven = GLFW_KEY_7,
            Eight = GLFW_KEY_8,
            Nine = GLFW_KEY_9,
            Semicolon = GLFW_KEY_SEMICOLON,
            Equal = GLFW_KEY_EQUAL,
            A = GLFW_KEY_A,
            B = GLFW_KEY_B,
            C = GLFW_KEY_C,
            D = GLFW_KEY_D,
            E = GLFW_KEY_E,
            F = GLFW_KEY_F,
            G = GLFW_KEY_G,
            H = GLFW_KEY_H,
            I = GLFW_KEY_I,
            J = GLFW_KEY_J,
            K = GLFW_KEY_K,
            L = GLFW_KEY_L,
            M = GLFW_KEY_M,
            N = GLFW_KEY_N,
            O = GLFW_KEY_O,
            P = GLFW_KEY_P,
            Q = GLFW_KEY_Q,
            R = GLFW_KEY_R,
            S = GLFW_KEY_S,
            T = GLFW_KEY_T,
            U = GLFW_KEY_U,
            V = GLFW_KEY_V,
            W = GLFW_KEY_W,
            X = GLFW_KEY_X,
            Y = GLFW_KEY_Y,
            Z = GLFW_KEY_Z,
            LeftBracket = GLFW_KEY_LEFT_BRACKET,
            BackSlash = GLFW_KEY_BACKSLASH,
            RightBracket = GLFW_KEY_RIGHT_BRACKET,
            GraveAccent = GLFW_KEY_GRAVE_ACCENT,
            World1 = GLFW_KEY_WORLD_1,
            World2 = GLFW_KEY_WORLD_2,
            Escape = GLFW_KEY_ESCAPE,
            Enter = GLFW_KEY_ENTER,
            Tab = GLFW_KEY_TAB,
            Backspace = GLFW_KEY_BACKSPACE,
            Insert = GLFW_KEY_INSERT,
            Delete = GLFW_KEY_DELETE,
            Right = GLFW_KEY_RIGHT,
            Left = GLFW_KEY_LEFT,
            Down = GLFW_KEY_DOWN,
            Up = GLFW_KEY_UP,
            PageUp = GLFW_KEY_PAGE_UP,
            PageDown = GLFW_KEY_PAGE_DOWN,
            Home = GLFW_KEY_HOME,
            End = GLFW_KEY_END,
            CapsLock = GLFW_KEY_CAPS_LOCK,
            ScrollLock = GLFW_KEY_SCROLL_LOCK,
            NumLock = GLFW_KEY_NUM_LOCK,
            PrintScreen = GLFW_KEY_PRINT_SCREEN,
            Pause = GLFW_KEY_PAUSE,
            F1 = GLFW_KEY_F1,
            F2 = GLFW_KEY_F2,
            F3 = GLFW_KEY_F3,
            F4 = GLFW_KEY_F4,
            F5 = GLFW_KEY_F5,
            F6 = GLFW_KEY_F6,
            F7 = GLFW_KEY_F7,
            F8 = GLFW_KEY_F8,
            F9 = GLFW_KEY_F9,
            F10 = GLFW_KEY_F10,
            F11 = GLFW_KEY_F11,
            F12 = GLFW_KEY_F12,
            F13 = GLFW_KEY_F13,
            F14 = GLFW_KEY_F14,
            F15 = GLFW_KEY_F15,
            F16 = GLFW_KEY_F16,
            F17 = GLFW_KEY_F17,
            F18 = GLFW_KEY_F18,
            F19 = GLFW_KEY_F19,
            F20 = GLFW_KEY_F20,
            F21 = GLFW_KEY_F21,
            F22 = GLFW_KEY_F22,
            F23 = GLFW_KEY_F23,
            F24 = GLFW_KEY_F24,
            F25 = GLFW_KEY_F25,
            Keypad0 = GLFW_KEY_KP_0,
            Keypad1 = GLFW_KEY_KP_1,
            Keypad2 = GLFW_KEY_KP_2,
            Keypad3 = GLFW_KEY_KP_3,
            Keypad4 = GLFW_KEY_KP_4,
            Keypad5 = GLFW_KEY_KP_5,
            Keypad6 = GLFW_KEY_KP_6,
            Keypad7 = GLFW_KEY_KP_7,
            Keypad8 = GLFW_KEY_KP_8,
            Keypad9 = GLFW_KEY_KP_9,
            KeyPadDecimal = GLFW_KEY_KP_DECIMAL,
            KeyPadDivide = GLFW_KEY_KP_DIVIDE,
            KeyPadMultiply = GLFW_KEY_KP_MULTIPLY,
            KeyPadSubtract = GLFW_KEY_KP_SUBTRACT,
            KeyPadAdd = GLFW_KEY_KP_ADD,
            KeyPadEnter = GLFW_KEY_KP_ENTER,
            KeyPadEqual = GLFW_KEY_KP_EQUAL,
            LeftShift = GLFW_KEY_LEFT_SHIFT,
            LeftControl = GLFW_KEY_LEFT_CONTROL,
            LeftAlt = GLFW_KEY_LEFT_ALT,
            LeftSuper = GLFW_KEY_LEFT_SUPER,
            RightShift = GLFW_KEY_RIGHT_SHIFT,
            RightControl = GLFW_KEY_RIGHT_CONTROL,
            RightAlt = GLFW_KEY_RIGHT_ALT,
            RightSuper = GLFW_KEY_RIGHT_SUPER,
            Menu = GLFW_KEY_MENU,
            MaxValue = GLFW_KEY_LAST
        };

        enum class KeyState
        {
            Press = GLFW_PRESS,
            Release = GLFW_RELEASE,
            Repeat = GLFW_REPEAT
        };

        enum class MouseButton
        {
            One = GLFW_MOUSE_BUTTON_1,
            Two = GLFW_MOUSE_BUTTON_2,
            Three = GLFW_MOUSE_BUTTON_3,
            Four = GLFW_MOUSE_BUTTON_4,
            Five = GLFW_MOUSE_BUTTON_5,
            Six = GLFW_MOUSE_BUTTON_6,
            Seven = GLFW_MOUSE_BUTTON_7,
            Eight = GLFW_MOUSE_BUTTON_8,
            Left = GLFW_MOUSE_BUTTON_LEFT,
            Right = GLFW_MOUSE_BUTTON_RIGHT,
            Middle = GLFW_MOUSE_BUTTON_MIDDLE,
            MaxValue = GLFW_MOUSE_BUTTON_LAST,
        };

        enum class MouseButtonState
        {
            Press = GLFW_PRESS,
            Release = GLFW_RELEASE
        };

        using DimensionsType = Constants::Window::DimensionsType;
        using ScalarType = Constants::Window::ScalarType;
        using CoordinatesType = Constants::Window::CoordinatesType;

        Window(std::string_view Title, DimensionsType Width, DimensionsType Height, ClientAPI GraphicsAPI);

        ~Window();

        void SetSizeLimits(DimensionsType MinWidth, DimensionsType MinHeight, DimensionsType MaxWidth, DimensionsType MaxHeight) const;

        void SetSizeMin(DimensionsType Width, DimensionsType Height) const;

        void SetSizeMax(DimensionsType Width, DimensionsType Height) const;

        void SetTitle(std::string_view Title) const;

        void SetIcon(const GLFWimage& Image) const;

        void SetIcon(const std::vector<GLFWimage>& Images) const;

        void SetPosition(DimensionsType X, DimensionsType Y) const;

        void SetAspectRatio(DimensionsType Numerator, DimensionsType Denominator) const;

        void SetOpacity(ScalarType Opacity) const;

        void SetMonitor(GLFWmonitor& Monitor,
                        DimensionsType XPosition, DimensionsType YPosition,
                        DimensionsType Width, DimensionsType Height,
                        DimensionsType RefreshRate
        ) const;

        void SetAttribute(Attribute Attribute, bool Value) const;

        void SetUserPointer(void* Pointer) const;

        void SetPositionCallback(GLFWwindowposfun Callback) const;

        void SetCloseCallback(GLFWwindowclosefun Callback) const;

        void SetRefreshCallback(GLFWwindowrefreshfun Callback) const;

        void SetFocusCallback(GLFWwindowfocusfun Callback) const;

        void SetIconifyCallback(GLFWwindowiconifyfun Callback) const;

        void SetMaximizeCallback(GLFWwindowmaximizefun Callback) const;

        void SetContentScaleCallback(GLFWwindowcontentscalefun Callback) const;

        void SetSizeCallback(GLFWwindowsizefun Callback) const;

        void SetFrameBufferSizeCallback(GLFWframebuffersizefun Callback) const;

        void SetKeyCallback(GLFWkeyfun Callback) const;

        GLFWcharfun SetCharCallback(GLFWcharfun Callback) const;

        GLFWmousebuttonfun SetMouseButtonCallback(GLFWmousebuttonfun Callback) const;

        GLFWcursorposfun SetCursorPositionCallback(GLFWcursorposfun Callback) const;

        GLFWcursorenterfun SetCursorEnterCallback(GLFWcursorenterfun Callback) const;

        GLFWscrollfun SetScrollCallback(GLFWscrollfun Callback) const;

        GLFWdropfun SetDropCallback(GLFWdropfun Callback) const;

        void MakeContextCurrent() const;

        void SwapBuffers() const;

        void Restore() const;

        void Maximize() const;

        void Show() const;

        void Hide() const;

        void Focus() const;

        void RequestAttention() const;

        void Iconify() const;

        void SetShouldClose(bool ShouldClose) const;

        void SetInputMode(InputMode Mode, CursorMode Value) const;

        void SetCursorPosition(CoordinatesType XPosition, CoordinatesType YPosition) const;

        void SetCursor(GLFWcursor* Cursor) const;

        [[nodiscard]] std::string_view GetTitle() const;

        [[nodiscard]] std::pair<DimensionsType, DimensionsType> GetPosition() const;

        [[nodiscard]] std::pair<DimensionsType, DimensionsType> GetSize() const;

        [[nodiscard]] std::pair<DimensionsType, DimensionsType> GetFrameBufferSize() const;

        //Returns Left, Top, Right, Bottom
        [[nodiscard]] std::array<DimensionsType, 4> GetFrameSize() const;

        [[nodiscard]] std::pair<ScalarType, ScalarType> GetContentScale() const;

        [[nodiscard]] ScalarType GetOpacity() const;

        [[nodiscard]] GLFWmonitor* GetMonitor() const;

        [[nodiscard]] bool GetAttribute(Attribute Value) const;

        [[nodiscard]] bool IsShouldClose() const;

        [[nodiscard]] void* GetUserPointer() const;

        [[nodiscard]] CursorMode GetInputModeCursor() const;

        [[nodiscard]] bool GetInputModeStickyKeys() const;

        [[nodiscard]] bool GetInputModeStickyMouseButtons() const;

        [[nodiscard]] bool GetInputModeLockKeyMods() const;

        [[nodiscard]] bool GetInputModeRawMouseMotion() const;

        [[nodiscard]] bool GetKey(Key Value) const;

        [[nodiscard]] bool GetMouseButton(MouseButton Value) const;

        [[nodiscard]] std::pair<CoordinatesType, CoordinatesType> GetCursorPosition() const;

    private:
        GLFWwindow* Data {};
    };
}

#endif // ENGINE_SOURCES_GUI_WINDOW_FILE_WINDOW_H
