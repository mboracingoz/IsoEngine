#pragma once

namespace IsoForge
{
class Panel
{
public:
    explicit Panel(const char* title)
        : m_Title(title)
    {
    }

    virtual ~Panel() = default;

    virtual void OnImGuiRender() = 0;

    [[nodiscard]] const char* GetTitle() const
    {
        return m_Title;
    }

    [[nodiscard]] bool& GetOpenRef()
    {
        return m_IsOpen;
    }

    [[nodiscard]] bool IsOpen() const
    {
        return m_IsOpen;
    }

    void SetOpen(bool open)
    {
        m_IsOpen = open;
    }

protected:
    const char* m_Title = "Panel";
    bool m_IsOpen = true;
};
}
