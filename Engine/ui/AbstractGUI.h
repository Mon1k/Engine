#ifndef _ABSTRACTGUI_H_
#define _ABSTRACTGUI_H_

#include "../AbstractNode.h"
#include "../d3dclass.h"
#include "../inputclass.h"

#include <functional>
#include <vector>

class UIManager;

class AbstractGui: public AbstractNode
{
public:
    AbstractGui()
    {
        m_visible = true;
        m_IsFocused = false;

        m_width = 0;
        m_height = 0;
        m_x = 0;
        m_y = 0;

        m_manager = 0;
        m_parent = 0;
        m_handlers.clear();
    };
    virtual void Shutdown() = 0;
    virtual bool Render() = 0;
    virtual void update()
    {

    }

    virtual void hide()
    {
        m_visible = false;
    }

    virtual void show()
    {
        m_visible = true;
    }

    virtual void focus()
    {
        if (!m_IsFocused) {
            m_IsFocused = true;
            proccesedEventHandlers(AbstractGui::EventType::OBJECT_FOCUS);
        }
    }

    virtual void unfocus()
    {
        if (m_IsFocused) {
            m_IsFocused = false;
            proccesedEventHandlers(AbstractGui::EventType::OBJECT_BLUR);
        }
    }

    virtual bool isFocused()
    {
        return m_IsFocused;
    }

    virtual bool isVisible()
    {
        return m_visible;
    }

    virtual void setVisible(bool visible)
    {
        m_visible = visible;
    }

    virtual bool onMousePress(int x, int y, int button)
    {
        return proccesedEventHandlers(EventType::MOUSE_DOWN);
    }

    virtual bool onKeyboardPress(InputClass::EventKey)
    {
        return proccesedEventHandlers(EventType::KEYBOARD_DOWN);
    }

    virtual bool onScroll(int diff)
    {
        return proccesedEventHandlers(EventType::SCROLL);
    }

    virtual void addEventHandler(int event, std::function<void()> handler)
    {
        Event eventObject;
        eventObject.event = event;
        eventObject.handler = handler;
        m_handlers.push_back(eventObject);
    }

    virtual bool proccesedEventHandlers(int event)
    {
        if (!isVisible()) {
            return false;
        }

        bool isEvent = false;
        for (size_t i = 0; i < m_handlers.size(); i++) {
            if (m_handlers[i].event == event) {
                m_handlers[i].handler();
                isEvent = true;
            }
        }

        return isEvent;
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
        update();
    }

    virtual void move(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        update();
    }

    virtual void frame(float) {}

public:
    struct Event {
        std::function<void()> handler;
        int event;
        int lparam;
        int wparam;
    };
    enum EventType: int {
        MOUSE_DOWN = 0,
        MOUSE1_DOWN = 1,
        MOUSE2_DOWN = 2,
        MOUSE3_DOWN = 3,
        MOUSE_WHEEL = 4,
        
        KEYBOARD_DOWN = 100,

        OBJECT_FOCUS = 200,
        OBJECT_BLUR = 201,

        SCROLL = 300,
        RESIZE = 301,

        // more 65536 is user events
    };

    D3DClass* m_D3D;
    D3DXMATRIX m_baseViewMatrix;
    bool m_visible;
    bool m_IsFocused;

    int m_width, m_height, m_x, m_y;
    
    UIManager* m_manager;
    AbstractGui* m_parent;
    std::vector<Event> m_handlers;
};

#endif