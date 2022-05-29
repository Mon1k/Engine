#ifndef _ABSTRACTNODE_H_
#define _ABSTRACTNODE_H_

class AbstractNode
{
public:
    AbstractNode() {
        m_id = 0;
    };
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