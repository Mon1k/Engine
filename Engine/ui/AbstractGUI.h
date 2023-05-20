#ifndef _ABSTRACTGUI_H_
#define _ABSTRACTGUI_H_

#include "../AbstractNode.h"
#include "../d3dclass.h"

#include <functional>
#include <vector>

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

        m_handlers.clear();
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
        if (isIntersect(x, y)) {
            proccesedEventHandlers(EventType::MOUSE_DOWN);
        }
    }

    virtual void onKeyboardPress(int key, char symbol) {
        proccesedEventHandlers(EventType::KEYBOARD_DOWN);
    }

    virtual void addEventHandler(int event, std::function<int()> handler) {
        Event eventObject;
        eventObject.event = event;
        eventObject.handler = handler;
        m_handlers.push_back(eventObject);
    }

    virtual void proccesedEventHandlers(int event) {
        if (!isVisible()) {
            return;
        }

        int size = m_handlers.size();
        for (int i = 0; i < size; i++) {
            if (m_handlers[i].event == event) {
                m_handlers[i].handler();
            }
        }
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
    struct Event {
        std::function<int()> handler;
        int event;
        int lparam;
        int wparam;
    };
    enum EventType: int {
        MOUSE_DOWN = 0,
        
        KEYBOARD_DOWN = 100,

        OBJECT_FOCUS = 200,
        OBJECT_BLUR = 201
    };

    D3DClass* m_D3D;
    D3DXMATRIX m_baseViewMatrix;
    bool m_visible;
    bool m_IsFocused;

    int m_width, m_height, m_x, m_y;
    
    std::vector<Event> m_handlers;
};

#endif