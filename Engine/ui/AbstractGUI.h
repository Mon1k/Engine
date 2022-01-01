#ifndef _ABSTRACTGUI_H_
#define _ABSTRACTGUI_H_

#include "../d3dclass.h"

class AbstractGui
{
public:
    AbstractGui() {
        m_visible = true;

        m_width = 0;
        m_height = 0;
        m_x = 0;
        m_y = 0;

        m_id = 0;
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
    void setId(int id)
    {
        m_id = id;
    };
    int getId()
    {
        return m_id;
    };

    virtual void onPress(int x, int y, int button)
    {
    }
    bool isIntersect(int x, int y)
    {
        if (x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height) {
            return true;
        }

        return false;
    }

public:
    D3DClass* m_D3D;
    D3DXMATRIX m_baseViewMatrix;
    bool m_visible;

    int m_width, m_height, m_x, m_y;

private:
    int m_id;
};

#endif