#ifndef _ABSTRACTGUI_H_
#define _ABSTRACTGUI_H_

#include "../AbstractNode.h"
#include "../d3dclass.h"

#include <functional>

class AbstractGui: public AbstractNode
{
public:
    AbstractGui() {
        m_visible = true;
        m_IsFocused = false;

        m_width = 0;
        m_height = 0;
        m_x = 0;
        m_y = 0;

        m_handler = 0;
    };
    virtual void Shutdown() = 0;
    virtual bool Render() = 0;
    void hide() {
        m_visible = false;
    }
    void show() {
        m_visible = true;
    }
    bool isVisible() {
        return m_visible;
    }
    void setVisible(bool visible) {
        m_visible = visible;
    }

    virtual bool isFocused()
    {
        return m_IsFocused;
    }

    virtual void onMousePress(int x, int y, int button)
    {
        if (isIntersect(x, y) && m_handler) {
            m_handler();
        }
    }
    virtual void onKeyboardPress(int key, char symbol) {}

    virtual void addEventHandler(std::function<int()> handler) {
        m_handler = handler;
    }

    virtual bool isIntersect(int x, int y)
    {
        if (x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height) {
            return true;
        }

        return false;
    }

    virtual void setPosition(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    virtual void frame(float) {}

public:
    D3DClass* m_D3D;
    D3DXMATRIX m_baseViewMatrix;
    bool m_visible;
    bool m_IsFocused;

    int m_width, m_height, m_x, m_y;
    std::function<int()> m_handler;
};

#endif