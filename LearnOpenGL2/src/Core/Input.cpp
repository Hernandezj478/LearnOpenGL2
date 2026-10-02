#include "Input.h"
#include "Window.h"

Input::Input(Window& window) : m_Window(window)
{
    m_Window.SetKeyHandler([this](int Key, int /*scancode*/, int Action, int /*mods*/) { OnKey(Key, Action); });
    m_Window.SetMouseButtonHandler([this](int Button, int Action, int /*mods*/) { OnMouseButton(Button, Action); });
    m_Window.SetCursorHandler([this](double X, double Y) { OnCursorPos(X, Y); });
    m_Window.SetScrollHandler([this](double DX, double DY) { OnScroll(DX, DY); });
    m_Window.SetFocusHandler([this](bool Focused) { OnFocus(Focused); });
}

Input::~Input()
{
    m_Window.SetKeyHandler(nullptr);
    m_Window.SetMouseButtonHandler(nullptr);
    m_Window.SetCursorHandler(nullptr);
    m_Window.SetScrollHandler(nullptr);
    m_Window.SetFocusHandler(nullptr);
}

void Input::NewFrame()
{
    m_Pressed.fill(false);
    m_Released.fill(false);
    m_ButtonPressed.fill(false);
    m_MouseDelta = glm::vec2(0.0f);
    m_ScrollDelta = glm::vec2(0.0f);
}

void Input::PostPoll()
{
    if (!m_FocusChanged)
    {
        return;
    }
    m_FocusChanged = false;
    ClearHeldState();
    m_Pressed.fill(false);
    m_Released.fill(false);
    m_ButtonPressed.fill(false);
    m_MouseDelta = glm::vec2(0.0f);
    m_ScrollDelta = glm::vec2(0.0f);

    if (glfwGetWindowAttrib(m_Window.Handle(), GLFW_FOCUSED) == GLFW_FALSE)
    {
        SetCursorCaptured(false);
    }
}

bool Input::IsDown(int Key) const
{
    return ValidKey(Key) && m_Down[Key];
}

bool Input::IsPressed(int Key) const
{
    return ValidKey(Key) && m_Pressed[Key];
}

bool Input::Released(int Key) const
{
    return ValidKey(Key) && m_Released[Key];
}

bool Input::MouseDown(int Button) const
{
    return ValidButton(Button) && m_ButtonDown[Button];
}

bool Input::MousePressed(int Button) const
{
    return ValidButton(Button) && m_ButtonPressed[Button];
}

void Input::SetCursorCaptured(bool Captured)
{
    if (Captured == m_Captured)
    {
        return;
    }
    m_Captured = Captured;
    m_HasLastCursor = false;
    GLFWwindow* handle = m_Window.Handle();
    glfwSetInputMode(handle, GLFW_CURSOR, Captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, Captured ? GLFW_TRUE : GLFW_FALSE);
    }
}

void Input::OnKey(int Key, int Action)
{
    if (!ValidKey(Key) || Action == GLFW_REPEAT)
    {
        return;
    }
    if (Action == GLFW_PRESS)
    {
        m_Down[Key] = true;
        m_Pressed[Key] = true;
    }
    else if (Action == GLFW_RELEASE)
    {
        m_Down[Key] = false;
        m_Released[Key] = true;
    }
}

void Input::OnMouseButton(int Button, int Action)
{
    if (!ValidButton(Button))
    {
        return;
    }
    if (Action == GLFW_PRESS)
    {
        m_ButtonDown[Button] = true;
        m_ButtonPressed[Button] = true;
    }
    else if (Action == GLFW_RELEASE)
    {
        m_ButtonDown[Button] = false;
    }
}

void Input::OnCursorPos(double X, double Y)
{
    if (!m_Captured)
    {
        return;
    }
    if (m_HasLastCursor)
    {
        m_MouseDelta.x += static_cast<float>(X - m_LastCursor.x);
        m_MouseDelta.y += static_cast<float>(Y - m_LastCursor.y);
    }
    m_LastCursor = { X, Y };
    m_HasLastCursor = true;
}

void Input::OnScroll(double DX, double DY)
{
    if (!m_Captured)
    {
        return;
    }
    m_ScrollDelta += glm::vec2(static_cast<float>(DX), static_cast<float>(DY));
}

void Input::OnFocus(bool Focused)
{
    if (!Focused)
    {
        ClearHeldState();
        SetCursorCaptured(false);
    }
}

void Input::ClearHeldState()
{
    m_Down.fill(false);
    m_ButtonDown.fill(false);
}
