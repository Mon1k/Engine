#ifndef _ABSTRACTGUI_H_
#define _ABSTRACTGUI_H_

class AbstractGui
{
public:
    AbstractGui() {
        visible = true;
    };
    void Shutdown() {};
    bool Render() {
        return true;
    };

public:
    bool visible;

};

#endif