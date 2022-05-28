#ifndef _ABSTRACTRESOURCE_H_
#define _ABSTRACTRESOURCE_H_

class AbstractResource
{
public:
    AbstractResource()
    {
        m_id = 0;
    }
    virtual void Shutdown() = 0;

    void setId(int id)
    {
        m_id = id;
    };
    int getId()
    {
        return m_id;
    };

private:
    int m_id;
};

#endif