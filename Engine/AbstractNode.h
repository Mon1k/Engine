#ifndef _ABSTRACTNODE_H_
#define _ABSTRACTNODE_H_

class AbstractNode
{
public:
    AbstractNode() {
        m_id = 0;
    }
    
    virtual void setId(int id)
    {
        m_id = id;
    }

    virtual int getId()
    {
        return m_id;
    }

    virtual bool compareId(int id)
    {
        return m_id == id;
    }

    virtual AbstractNode* getById(int id)
    {
        return m_id == id ? this : 0;
    }

private:
    int m_id;
};

#endif